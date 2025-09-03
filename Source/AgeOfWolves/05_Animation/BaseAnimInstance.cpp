#include "BaseAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"
#include "04_Component/BaseInputComponent.h"

#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimInstance)

//@Default Setting
#pragma region Default Setting
UBaseAnimInstance::UBaseAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , LastMovementState(EMovementState::Idle)
    , MovementState(EMovementState::Idle)  // 새로운 enum 값으로 시작
    , MovementDirection(EMovementDirection::Fwd)
    , StopMotionType(EStopMotionType::None)
    , bFalling(false)
    , bShouldMove(false)
    , Velocity(FVector::ZeroVector)
    , Speed(0.0f)
    , DirectionAngle(0.0f)
    , bEnableDirectionalMovement(false)
    , bModifyBoneTransform(false)
    , BoneTransformLerpSpeed(10.0f)
    , CharacterMovementCompRef(nullptr)
    , CombatType(ECombatType::NormalCombat)
    , bIsPlayingRootMotionMontageWithFullBodySlot(false)
    , bIsRootMotionCooldown(false)
    , RootMotionCooldownTime(0.0f)
    , RootMotionCooldownDuration(3.f)
    , CurrentRootMotionCooldownTime(0.0f)
    , LastMovementDirection(EMovementDirection::Fwd)
    , bCanSkipStartState(false) 
    , bCanSkipStopState(false)    
{
    OwnerCharacterBaseRef.Reset();
    CharacterMovementCompRef.Reset();
    CombatStateAttributeListenerRef = nullptr;
}

void UBaseAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    // Combat State 속성 수치 변화 관찰 시작
    // 이는 전투 상태에 따른 상태 기계 우회 로직을 위해 필요합니다
    ListenToCombatStateAttributeChange();
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // === Character Base 참조 설정 ===
    const auto CharacterBase = Cast<ACharacterBase>(TryGetPawnOwner());
    if (!CharacterBase)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "Character Base를 찾을 수 없습니다.");
        return;
    }
    OwnerCharacterBaseRef = CharacterBase;

    // 원본 FullWeapon Mesh 저장 (기존 기능 유지)
    if (UStaticMeshComponent* FullMesh = CharacterBase->GetFullWeaponMesh())
    {
        OriginalFullWeaponMesh = FullMesh->GetStaticMesh();
        UE_LOGFMT(LogAnimInstance, Log, "원본 FullWeapon Mesh 저장 완료");
    }

    // === Character Movement Component 참조 설정 ===
    UCharacterMovementComponent* CharacterMovementComp = OwnerCharacterBaseRef->GetCharacterMovement();
    if (!CharacterMovementComp)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "Character Movement Component를 찾을 수 없습니다.");
        return;
    }
    CharacterMovementCompRef = CharacterMovementComp;

    // === Montage 콜백 등록 ===
    // Root Motion 관련 처리를 위해 몽타주 시작/종료 이벤트를 구독합니다
    OnMontageStarted.AddDynamic(this, &UBaseAnimInstance::MontageStarted);
    OnMontageEnded.AddDynamic(this, &UBaseAnimInstance::MontageEnded);

    UE_LOGFMT(LogAnimInstance, Log, "BaseAnimInstance 초기화 완료 - 상태 기계 시스템 활성화");
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // === 기본 유효성 체크 ===
    if (!OwnerCharacterBaseRef.IsValid() || !OwnerCharacterBaseRef->GetController())
    {
        return;
    }

    // === Root Motion Cooldown 처리 ===
    // Root Motion이 끝난 후 일정 시간 동안 상태 변경을 제한하는 로직
    if (bIsRootMotionCooldown)
    {
        CurrentRootMotionCooldownTime += DeltaSeconds;
        if (CurrentRootMotionCooldownTime >= RootMotionCooldownDuration)
        {
            bIsRootMotionCooldown = false;
            LastMovementState = MovementState;

            UE_LOGFMT(LogAnimInstance, Log, "Root Motion Cooldown 종료 - LastState 업데이트: {0}",
                *UEnum::GetValueAsString(LastMovementState));

            UpdateMovementSettings();
        }
    }

    // === 기본 이동 데이터 업데이트 ===
    // 이 데이터들은 상태 기계의 판단 기준이 됩니다
    bFalling = OwnerCharacterBaseRef->GetMovementComponent()->IsFalling();
    Velocity = OwnerCharacterBaseRef->GetVelocity();
    Speed = OwnerCharacterBaseRef->GetVelocity().Length();
    bShouldMove = Speed > 25.f && OwnerCharacterBaseRef->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;

    // === 새로운 상태 기계 업데이트 (핵심 로직) ===
    UpdateMovementStateMachine();

    // === 이동 방향 각도 계산 (기존 로직 유지) ===
    FindMovementDirectionAngle();
}
#pragma endregion

//@속성/정보...등
#pragma region Property or Subwidgets or Infos...etc
void UBaseAnimInstance::HandleBattoujutsuMovementBlock(ECombatType OldCombatType, ECombatType NewCombatType)
{
    // InputComponent 참조 획득
    if (!OwnerCharacterBaseRef.IsValid())
    {
        UE_LOGFMT(LogAnimInstance, Warning, "OwnerCharacterBaseRef가 유효하지 않습니다.");
        return;
    }

    APlayerController* PC = Cast<APlayerController>(OwnerCharacterBaseRef->GetController());
    if (!PC)
    {
        return; // AI Controller일 수 있으므로 경고 없이 리턴
    }

    auto* InputComp = Cast<UBaseInputComponent>(PC->InputComponent);
    if (!InputComp)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "BaseInputComponent를 찾을 수 없습니다.");
        return;
    }

    // 발도술 자세로 전환되는 경우
    if (NewCombatType == ECombatType::BattoujutsuCombat)
    {
        InputComp->SetBlockMovementInput(true);

        UE_LOGFMT(LogAnimInstance, Log, "발도술 자세 진입 - 이동 입력 블록 활성화");

        // 현재 이동 중이라면 강제로 Stop 상태로 전환
        if (MovementState == EMovementState::Cycle_Walk || MovementState == EMovementState::Cycle_Sprint)
        {
            UE_LOGFMT(LogAnimInstance, Warning, "발도술 자세 진입으로 인한 강제 정지: {0} -> Idle",
                *UEnum::GetValueAsString(MovementState));

            LastMovementState = MovementState;
            MovementState = EMovementState::Idle;
            OnMovementStateChanged();
        }
    }
    // 발도술 자세에서 벗어나는 경우
    else if (OldCombatType == ECombatType::BattoujutsuCombat && NewCombatType != ECombatType::BattoujutsuCombat)
    {
        InputComp->SetBlockMovementInput(false);

        UE_LOGFMT(LogAnimInstance, Log, "발도술 자세 해제 - 이동 입력 블록 해제");
    }
}

// =====================================================
// 상태 기계 핵심 로직 영역
// =====================================================
#pragma region State Machine Core Logic
void UBaseAnimInstance::UpdateMovementStateMachine()
{
    /*
     * 상태 기계의 메인 업데이트 함수 (LastMovementState 보호 로직 추가)
     */

     // === Root Motion 재생 중일 때 특별 처리 (수정됨) ===
    if (bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        // FullBody 몽타주가 시작되면 즉시 Idle 상태로 전환
        if (MovementState != EMovementState::Idle)
        {
            UE_LOGFMT(LogAnimInstance, Log, "FullBody 몽타주 재생 중 - 강제 Idle 전환: {0} -> Idle",
                *UEnum::GetValueAsString(MovementState));

            // === LastMovementState 보호 로직 ===
            EMovementState PreviousMovementState = MovementState;
            MovementState = EMovementState::Idle;

            // LastMovementState가 이미 Idle이 아닌 경우 (Combat State 변경에서 이미 설정됨)
            // 해당 값을 보호하고 덮어쓰지 않음
            if (LastMovementState == EMovementState::Idle)
            {
                // LastMovementState가 Idle인 경우에만 이전 상태로 업데이트
                // (정상적인 Root Motion 시작 케이스)
                LastMovementState = PreviousMovementState;
                UE_LOGFMT(LogAnimInstance, Log, "   └─ LastMovementState 업데이트: Idle -> {0} (정상 Root Motion)",
                    *UEnum::GetValueAsString(LastMovementState));
            }
            else
            {
                // LastMovementState가 이미 적절한 값으로 설정되어 있음 (Combat State 변경에서)
                // 보호하고 덮어쓰지 않음
                UE_LOGFMT(LogAnimInstance, Log, "   └─ LastMovementState 보호: {0} 유지 (Combat State 변경에서 설정됨)",
                    *UEnum::GetValueAsString(LastMovementState));
            }

            OnMovementStateChanged();
        }

        // 이후 정지 조건이 만족되면 그대로 Idle 유지
        if (CanTransitionToStop())
        {
            // 이미 Idle 상태이므로 상태 변경은 없지만 로직 일관성을 위해 유지
            LastMovementState = MovementState;
            MovementState = EMovementState::Idle;
            OnMovementStateChanged();
        }
        return;
    }

    // 이전 상태를 저장해두어 나중에 변경 여부를 확인합니다
    EMovementState PreviousState = MovementState;

    // Combat State에 따른 우회 조건들을 업데이트합니다
    // 이제 Root Motion 상태도 함께 고려됩니다
    UpdateCombatStateFlags();

    // 현재 상태에 따라 적절한 전이 처리 함수를 호출합니다
    switch (MovementState)
    {
    case EMovementState::Idle:
        HandleIdleStateTransitions();
        break;

    case EMovementState::Start:
        HandleStartStateTransitions();
        break;

    case EMovementState::Cycle_Walk:
    case EMovementState::Cycle_Sprint:
        HandleCycleStateTransitions();
        break;

    case EMovementState::Stop:
        HandleStopStateTransitions();
        break;

    default:
        // 예상치 못한 상태일 경우 안전하게 Idle로 복귀
        UE_LOGFMT(LogAnimInstance, Warning, "예상치 못한 MovementState: {0}, Idle로 복귀",
            static_cast<int32>(MovementState));
        MovementState = EMovementState::Idle;
        break;
    }

    // 상태가 실제로 변경되었을 때만 추가 처리를 수행합니다
    if (PreviousState != MovementState)
    {
        LastMovementState = PreviousState;
        OnMovementStateChanged();
    }
}

void UBaseAnimInstance::HandleIdleStateTransitions()
{
    /*
     * Idle 상태에서 가능한 전이들을 처리하는 함수
     *
     * Idle 상태는 캐릭터가 가만히 서 있는 상태입니다.
     * 여기서는 이동을 시작하거나 특수한 정지 애니메이션을 실행할 수 있습니다.
     */

     // 우선순위 1: Guard Combat 상태일 때는 바로 Cycle로 전이 가능
     // 이는 방어 상황에서 빠른 반응을 위한 특수 경로입니다
    if (CanTransitionFromIdleToCycle())
    {
        EMovementState TargetCycle = DetermineTargetCycleState();
        MovementState = TargetCycle;
        UE_LOGFMT(LogAnimInstance, Log, "Idle -> {0} (Guard Combat 우회)",
            *UEnum::GetValueAsString(TargetCycle));
        return;
    }

    // 우선순위 2: 일반적인 경우는 Start 상태를 거쳐야 함
    // 자연스러운 이동 시작 애니메이션을 위해 Start 단계를 거칩니다
    if (CanTransitionFromIdleToStart())
    {
        MovementState = EMovementState::Start;
        UE_LOGFMT(LogAnimInstance, Log, "Idle -> Start");
        return;
    }

    // 우선순위 3: 특수한 정지 조건이 발생한 경우
    // 이미 Idle이지만 명시적인 정지 애니메이션이 필요할 때 사용됩니다
    if (CanTransitionToStop())
    {
        MovementState = EMovementState::Stop;
        UE_LOGFMT(LogAnimInstance, Log, "Idle -> Stop (명시적 정지)");
        return;
    }
}

void UBaseAnimInstance::HandleStartStateTransitions()
{
    /*
     * Start 상태에서 가능한 전이들을 처리하는 함수
     *
     * Start 상태는 이동을 시작하는 가속 구간입니다.
     * 여기서는 지속적인 이동(Cycle)으로 전환하거나 중간에 멈출 수 있습니다.
     */

     // 우선순위 1: 정지 조건 체크 (가장 중요)
     // 이동 시작 중에도 갑자기 멈춰야 하는 상황이 있을 수 있습니다
    if (CanTransitionToStop())
    {
        MovementState = EMovementState::Stop;
        UE_LOGFMT(LogAnimInstance, Log, "Start -> Stop");
        return;
    }

    // 우선순위 2: Cycle 상태로 전이
    // Animation Blueprint에서 타이밍 조건(GetRelevantTimeRemaining <= 0.7)을 추가로 체크해야 합니다
    if (CanTransitionFromStartToCycle())
    {
        EMovementState TargetCycle = DetermineTargetCycleState();
        MovementState = TargetCycle;
        UE_LOGFMT(LogAnimInstance, Log, "Start -> {0}",
            *UEnum::GetValueAsString(TargetCycle));
        return;
    }
}

void UBaseAnimInstance::HandleCycleStateTransitions()
{
    /*
     * Cycle 상태들에서 가능한 전이들을 처리하는 함수 (Skip 로직 개선)
     */

     // 우선순위 1: 정지 조건 체크 (가장 중요)
    if (CanTransitionToStop())
    {
        // === Skip Stop 조건 체크 (통합 로직) ===
        // GuardCombat 상태이거나 Root Motion 시작으로 인한 Skip이 활성화된 경우
        if (bCanSkipStopState)
        {
            MovementState = EMovementState::Idle;

            // Skip 원인을 명확히 로깅
            FString SkipReason = "";
            if (IsInGuardCombatState())
            {
                SkipReason += "GuardCombat";
            }
            if (bIsPlayingRootMotionMontageWithFullBodySlot)
            {
                if (!SkipReason.IsEmpty()) SkipReason += " + ";
                SkipReason += "RootMotion";
            }

            UE_LOGFMT(LogAnimInstance, Log, "{0} -> Idle (Stop 우회: {1})",
                *UEnum::GetValueAsString(LastMovementState),
                *SkipReason);
        }
        else
        {
            MovementState = EMovementState::Stop;
            UE_LOGFMT(LogAnimInstance, Log, "{0} -> Stop (정상 전이)",
                *UEnum::GetValueAsString(MovementState));
        }
        return;
    }

    // 우선순위 2: Cycle 간 전환 (Walk <-> Sprint)
    if (CanTransitionBetweenCycles())
    {
        EMovementState TargetCycle = DetermineTargetCycleState();
        UE_LOGFMT(LogAnimInstance, Log, "{0} -> {1}",
            *UEnum::GetValueAsString(MovementState),
            *UEnum::GetValueAsString(TargetCycle));
        MovementState = TargetCycle;
        return;
    }
}

void UBaseAnimInstance::HandleStopStateTransitions()
{
    /*
     * Stop 상태에서 가능한 전이들을 처리하는 함수
     *
     * Stop 상태는 이동을 멈추는 감속 구간입니다.
     * 기본적으로는 Idle 상태로만 전이가 가능합니다.
     */

    if (CanTransitionFromStopToIdle())
    {
        // 여기서 실제 상태 변경을 수행합니다!
        MovementState = EMovementState::Idle;

        UE_LOGFMT(LogAnimInstance, Log, "Stop -> Idle 전이 완료 (상태 업데이트됨)");

        // 추가적인 정리 작업이 필요하다면 여기서 수행
        // 예: 루트 모션 관련 플래그 정리, 입력 관련 변수 초기화 등
    }
}

void UBaseAnimInstance::UpdateCombatStateFlags()
{
    /*
     * Combat State와 Root Motion에 따른 우회 플래그들을 업데이트하는 함수 (수정됨)
     *
     * 이제 Combat State뿐만 아니라 Root Motion 상태도 고려하여
     * Skip 플래그들을 종합적으로 관리합니다.
     */

    bool bWasCanSkipStates = bCanSkipStartState;

    // === Skip Start 플래그 업데이트 ===
    // GuardCombat(3) 상태에서 Start 단계를 건너뛸 수 있음
    bool bCanSkipBasedOnCombat = IsInGuardCombatState();
    bCanSkipStartState = bCanSkipBasedOnCombat;

    // === Skip Stop 플래그 업데이트 (통합 로직) ===
    // 1. GuardCombat(3) 상태이거나
    // 2. Root Motion 재생 중일 때 Skip 가능
    bool bCanSkipStopBasedOnCombat = bCanSkipBasedOnCombat;
    bool bCanSkipStopBasedOnRootMotion = bIsPlayingRootMotionMontageWithFullBodySlot;

    bCanSkipStopState = bCanSkipStopBasedOnCombat || bCanSkipStopBasedOnRootMotion;

    // === 변경 사항 로깅 ===
    if (bWasCanSkipStates != bCanSkipStartState || bWasCanSkipStates != bCanSkipStopState)
    {
        UE_LOGFMT(LogAnimInstance, Log, "Skip 플래그 업데이트: Start({0}->({1}), Stop({2}) | 원인: Combat({3}), RootMotion({4})",
            bWasCanSkipStates,
            bCanSkipStartState,
            bCanSkipStopState,
            bCanSkipBasedOnCombat,
            bCanSkipStopBasedOnRootMotion);

        UE_LOGFMT(LogAnimInstance, Log, "   └─ CombatType: {0} ({1})",
            static_cast<int32>(CombatType),
            *UEnum::GetValueAsString(CombatType));
    }
}

void UBaseAnimInstance::OnMovementStateChanged()
{
    /*
     * 상태 변경 시 호출되는 콜백 함수
     *
     * 상태가 변경되었을 때 필요한 추가 처리들을 수행합니다.
     * 기존 시스템과의 호환성을 위해 Stop Motion Type 업데이트 등을 처리합니다.
     */

     // Stop Motion Type 업데이트 (기존 로직 유지)
    if (MovementState == EMovementState::Idle && LastMovementState != EMovementState::Idle)
    {
        if (LastMovementState == EMovementState::Cycle_Walk || LastMovementState == EMovementState::Start)
        {
            UpdateStopMotionType(EStopMotionType::WalkStop);
        }
        else if (LastMovementState == EMovementState::Cycle_Sprint)
        {
            UpdateStopMotionType(EStopMotionType::SprintStop);
        }
    }

    // 이동 설정 업데이트 (기존 로직 유지)
    UpdateMovementSettings();

    // 상태 변경 로그 출력
    UE_LOGFMT(LogAnimInstance, Log, "{0} - MovementState 변경: {1} -> {2}",
        *OwnerCharacterBaseRef->GetName(),
        *UEnum::GetValueAsString(LastMovementState),
        *UEnum::GetValueAsString(MovementState));
}
#pragma endregion

// =====================================================
// 상태 전이 조건 함수들 영역
// =====================================================
#pragma region State Transition Conditions

bool UBaseAnimInstance::CanTransitionFromIdleToStart()
{
    /*
     * Idle에서 Start로 전이하는 기본 조건들을 체크하는 함수
     *
     * 이 함수는 일반적인 이동 시작 조건을 확인합니다.
     * Animation Blueprint에서 추가로 타이밍 조건을 체크할 수 있습니다.
     */

     // 함수 진입 로그 - 현재 상태와 기본 정보 출력
    UE_LOGFMT(LogAnimInstance, Warning, "=== CanTransitionFromIdleToStart 진입 ===");
    UE_LOGFMT(LogAnimInstance, Warning, "현재 MovementState: {0}, Speed: {1}, bShouldMove: {2}",
        *UEnum::GetValueAsString(MovementState), Speed, bShouldMove);

    // 조건 1: 이동 입력이 있어야 함
    bool bHasInput = HasMovementInput();
    UE_LOGFMT(LogAnimInstance, Warning, "조건 1 - HasMovementInput(): {0}", bHasInput);

    if (!bHasInput)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "❌ 조건 1 실패: 이동 입력이 없음");

        // HasMovementInput 내부 상태도 확인
        if (CharacterMovementCompRef.IsValid())
        {
            FVector Acceleration = CharacterMovementCompRef->GetCurrentAcceleration();
            UE_LOGFMT(LogAnimInstance, Warning, "세부정보 - Acceleration: {0}, IsNearlyZero: {1}",
                Acceleration.ToString(), Acceleration.IsNearlyZero());
        }
        else
        {
            UE_LOGFMT(LogAnimInstance, Warning, "세부정보 - CharacterMovementCompRef가 유효하지 않음");
        }

        return false;
    }

    // 조건 2: 현재 상태가 Idle이어야 함
    bool bIsIdle = (MovementState == EMovementState::Idle);
    UE_LOGFMT(LogAnimInstance, Warning, "조건 2 - MovementState == Idle: {0} (현재값: {1})",
        bIsIdle, *UEnum::GetValueAsString(MovementState));

    if (!bIsIdle)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "❌ 조건 2 실패: 현재 상태가 Idle이 아님");
        return false;
    }

    // 조건 3: Root Motion 재생 중이 아니어야 함
    UE_LOGFMT(LogAnimInstance, Warning, "조건 3 - Root Motion 재생 중: {0}",
        bIsPlayingRootMotionMontageWithFullBodySlot);

    if (bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "❌ 조건 3 실패: Root Motion 재생 중");
        return false;
    }

    // 조건 4: Combat State가 2가 아닐 때만 Start를 거쳐야 함
    bool bIsCombatState2 = IsInGuardCombatState();
    UE_LOGFMT(LogAnimInstance, Warning, "조건 4 - IsInGuardCombatState(): {0} (CombatType: {1})",
        bIsCombatState2, static_cast<int32>(CombatType));

    if (bIsCombatState2)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "❌ 조건 4 실패: Combat State가 2임 (바로 Cycle로 가야 함)");
        return false;
    }

    bCanTransitionFromIdleToStart = true;

    // 모든 조건 통과
    UE_LOGFMT(LogAnimInstance, Warning, "✅ 모든 조건 통과! Idle -> Start 전이 가능");
    return true;
}

bool UBaseAnimInstance::CanTransitionFromIdleToCycle()
{
    /*
     * Idle에서 바로 Cycle로 가는 특수한 경우를 체크하는 함수
     *
     * Combat State가 2일 때만 가능한 우회 경로입니다.
     * 전투 상황에서는 빠른 반응을 위해 Start 단계를 건너뛸 수 있습니다.
     */

     // 조건 1: Combat State가 3여야 함 (우회 조건)
    if (!IsInGuardCombatState())
    {
        return false;
    }

    // 조건 2-4: 기본 이동 조건들은 Start와 동일
    if (!HasMovementInput() || MovementState != EMovementState::Idle)
    {
        return false;
    }

    // 조건 5: Root Motion 재생 중이 아니어야 함
    if (bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        return false;
    }

    return true;
}

bool UBaseAnimInstance::CanTransitionFromStartToCycle()
{
    /*
     * Start에서 Cycle로 전이하는 조건을 체크하는 함수
     *
     * 실제 타이밍(GetRelevantTimeRemaining <= 0.7)은 Animation Blueprint에서 체크합니다.
     * 여기서는 기본적인 상황 조건만 확인합니다.
     */

     // 조건 1: 현재 상태가 Start여야 함
    if (MovementState != EMovementState::Start)
    {
        return false;
    }

    // 조건 2: 여전히 이동 입력이 있어야 함
    if (!HasMovementInput())
    {
        return false;
    }

    // 조건 3: Root Motion이 방해하지 않아야 함
    if (bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        return false;
    }

    return true;
}

bool UBaseAnimInstance::CanTransitionBetweenCycles()
{
    /*
     * Cycle_Walk <-> Cycle_Sprint 간 전환 가능성을 체크하는 함수
     *
     * 속도 변화에 따른 즉시 전환이 가능합니다.
     * 별도의 타이밍 조건 없이 바로 전환할 수 있습니다.
     */

     // 조건 1: 현재 Cycle 상태 중 하나여야 함
    if (MovementState != EMovementState::Cycle_Walk &&
        MovementState != EMovementState::Cycle_Sprint)
    {
        return false;
    }

    // 조건 2: 이동 입력이 계속 있어야 함
    if (!HasMovementInput())
    {
        return false;
    }

    // 조건 3: 현재 상태와 목표 상태가 달라야 함
    EMovementState TargetState = DetermineTargetCycleState();
    if (MovementState == TargetState)
    {
        return false;
    }

    return true;
}

bool UBaseAnimInstance::CanTransitionToStop()
{
    /*
     * 모든 활성 상태에서 Stop으로 전이하는 조건을 체크하는 함수 (개선됨)
     *
     * 핵심 개선사항: 방향 전환 시 불필요한 Stop을 방지하기 위해
     * 더 엄격한 정지 조건을 적용합니다.
     */

     // 조건 1: 정지 가능한 상태여야 함 (Start, Cycle_Walk, Cycle_Sprint)
    if (MovementState != EMovementState::Start &&
        MovementState != EMovementState::Cycle_Walk &&
        MovementState != EMovementState::Cycle_Sprint)
    {
        return false;
    }

    // 조건 2: Combat State 제한 체크
    bool bCanStopInThisCombatState = (CombatType == ECombatType::NormalCombat
        || CombatType == ECombatType::GuardCombat);

    if (!bCanStopInThisCombatState)
    {
        UE_LOGFMT(LogAnimInstance, VeryVerbose, "정지 조건 차단: Combat State = {0} ({1})",
            static_cast<int32>(CombatType),
            *UEnum::GetValueAsString(CombatType));
        return false;
    }

    // 조건 3: 실제 정지 조건 체크 (개선된 로직)
    bool bShouldStop = false;

    // 3-1: Root Motion 재생이 시작될 때 (기존 유지)
    if (bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "정지 조건 만족: Root Motion 재생 중");
        bShouldStop = true;
    }
    // 3-2: 이동 입력 체크 (개선된 로직)
    else
    {
        // 기존: !bShouldMove (너무 즉각적)
        // 개선: 더 엄격한 조건으로 불필요한 Stop 방지

        bool bHasStrongMovementInput = false;

        if (CharacterMovementCompRef.IsValid())
        {
            FVector CurrentAcceleration = CharacterMovementCompRef->GetCurrentAcceleration();

            // 더 엄격한 임계값 적용 - 정말로 멈추려는 의도일 때만 Stop
            bHasStrongMovementInput = CurrentAcceleration.SizeSquared() > (StopConditionThreshold * StopConditionThreshold);

            UE_LOGFMT(LogAnimInstance, VeryVerbose, "Stop 조건 체크 - Acceleration: {0}, SizeSquared: {1}, Threshold: {2}, HasStrongInput: {3}, bShouldMove: {4}",
                CurrentAcceleration.ToString(),
                CurrentAcceleration.SizeSquared(),
                StopConditionThreshold * StopConditionThreshold,
                bHasStrongMovementInput,
                bShouldMove);
        }

        // 추가 조건: 속도도 충분히 낮아야 Stop 허용
        // 빠르게 움직이는 중에는 순간적인 입력 부재로 인한 Stop을 방지
        bool bIsSlowEnoughToStop = Speed < 100.0f;

        // 최종 정지 조건: 입력이 없고 + 속도가 충분히 낮을 때
        if (!bHasStrongMovementInput && !bShouldMove && bIsSlowEnoughToStop)
        {
            UE_LOGFMT(LogAnimInstance, Warning, "정지 조건 만족: 입력 없음 + 저속 (Speed: {0})", Speed);
            bShouldStop = true;
        }
    }

    return bShouldStop;
}

bool UBaseAnimInstance::CanTransitionFromStopToIdle()
{
    /*
     * Stop에서 Idle로 전이하는 조건을 체크하는 함수 (개선됨)
     *
     * 이 함수는 완전히 멈췄을 때만 Idle로 전환하도록
     * 가장 엄격한 임계값을 사용합니다.
     */

     // 조건 1: 현재 상태가 Stop이어야 함
    if (MovementState != EMovementState::Stop)
    {
        return false;
    }

    // 조건 2: Character Movement Component가 유효해야 함
    if (!CharacterMovementCompRef.IsValid())
    {
        return false;
    }

    // 조건 3: 가속도가 거의 0에 가까워야 함 (가장 엄격한 조건)
    FVector CurrentAcceleration = CharacterMovementCompRef->GetCurrentAcceleration();

    // Stop -> Idle은 정말로 완전히 멈췄을 때만 허용
    // 여기서는 가장 엄격한 임계값 사용
    bool bIsReallyIdle = CurrentAcceleration.SizeSquared() < (IdleTransitionThreshold * IdleTransitionThreshold);

    UE_LOGFMT(LogAnimInstance, VeryVerbose, "Stop->Idle 체크 - Acceleration: {0}, SizeSquared: {1}, Threshold: {2}, IsReallyIdle: {3}",
        CurrentAcceleration.ToString(),
        CurrentAcceleration.SizeSquared(),
        IdleTransitionThreshold * IdleTransitionThreshold,
        bIsReallyIdle);

    return bIsReallyIdle;
}
#pragma endregion

// =====================================================
// 기존 기능 유지 영역 (레거시 호환성)
// =====================================================
#pragma region Legacy Functions

void UBaseAnimInstance::FindMovementDirectionAngle()
{
    /*
     * 이동 방향 각도를 계산하는 함수 (기존 로직 유지)
     *
     * 방향성 이동 시스템을 위해 캐릭터의 이동 방향을 계산합니다.
     * 상태 기계와는 독립적으로 작동합니다.
     */
    if (!bEnableDirectionalMovement 
        || MovementState == EMovementState::Cycle_Sprint
        || bIsRootMotionCooldown)
    {
        DirectionAngle = 0.f;
        MovementDirection = EMovementDirection::Fwd;
        return;
    }

    EMovementDirection PrevDirection = MovementDirection;
    FRotator Rotation = OwnerCharacterBaseRef->GetControlRotation();
    DirectionAngle = CalculateDirection(Velocity, Rotation);

    // 속도가 너무 낮을 때는 별도 처리
    const float MinVelocitySquared = 25.0f;
    if (Velocity.SizeSquared() < MinVelocitySquared)
    {
        // 현재 방향을 마지막 방향으로 저장
        LastMovementDirection = PrevDirection;
        UE_LOGFMT(LogAnimInstance, Log, "마지막 방향 저장: {0}",
            *UEnum::GetValueAsString(LastMovementDirection));

        // 정지 모션 실행 중일 때는 마지막 방향 사용
        if (StopMotionType != EStopMotionType::None)
        {
            MovementDirection = LastMovementDirection;
            UE_LOGFMT(LogAnimInstance, Log, "정지 모션의 방향: {0}",
                *UEnum::GetValueAsString(MovementDirection));
        }

        return;
    }

    // 이동 중일 때의 방향 계산 로직
    if (DirectionAngle >= -45.f && DirectionAngle < 45.f)
    {
        MovementDirection = EMovementDirection::Fwd;
    }
    else if (DirectionAngle >= 45.f && DirectionAngle < 135.f)
    {
        MovementDirection = EMovementDirection::Right;
    }
    else if (DirectionAngle >= -135.f && DirectionAngle < -45.f)
    {
        MovementDirection = EMovementDirection::Left;
    }
    else
    {
        MovementDirection = EMovementDirection::Bwd;
    }

    // 방향이 변경되었을 때 로그 출력
    if (PrevDirection != MovementDirection)
    {
        UE_LOGFMT(LogAnimInstance, Log, "이동 방향 변경: {0} -> {1}",
            *UEnum::GetValueAsString(PrevDirection),
            *UEnum::GetValueAsString(MovementDirection));

        LastMovementDirection = MovementDirection;
    }
}

void UBaseAnimInstance::UpdateMovementSettings()
{
    /*
     * 이동 설정을 업데이트하는 함수 (Root Motion Cooldown 고려)
     *
     * 상태에 따라 캐릭터의 회전 방식을 조정합니다.
     * Root Motion Cooldown 중에는 Controller 회전을 사용하지 않습니다.
     */
    if (!CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogAnimInstance, Error, "Movement Component가 유효하지 않습니다.");
        return;
    }

    //@Root Motion Cooldown 중일 때 특별 처리
    if (bIsRootMotionCooldown)
    {
        CharacterMovementCompRef->bUseControllerDesiredRotation = false;
        CharacterMovementCompRef->bOrientRotationToMovement = true;

        UE_LOGFMT(LogAnimInstance, Log, "Root Motion Cooldown 중 - 강제 OrientRotationToMovement 설정");
        return;
    }

    bool bShouldUseDirectionalMovement = bEnableDirectionalMovement
        && MovementState != EMovementState::Cycle_Sprint;

    UE_LOGFMT(LogAnimInstance, Log, "이동 방향 설정 값: DirectionalMovement({0}), MovementState({1}))",
        bEnableDirectionalMovement,
        *UEnum::GetValueAsString(MovementState));

    if (bShouldUseDirectionalMovement)
    {
        // 방향성 이동 설정
        CharacterMovementCompRef->bUseControllerDesiredRotation = true;
        CharacterMovementCompRef->bOrientRotationToMovement = false;

        UE_LOGFMT(LogAnimInstance, Log, "방향 이동 설정: ControllerDesiredRotation(true), OrientRotationToMovement(false)");
    }
    else
    {
        // 일반 이동 설정
        CharacterMovementCompRef->bUseControllerDesiredRotation = false;
        CharacterMovementCompRef->bOrientRotationToMovement = true;

        // 방향을 앞쪽으로 초기화
        MovementDirection = EMovementDirection::Fwd;
        LastMovementDirection = EMovementDirection::Fwd;

        UE_LOGFMT(LogAnimInstance, Log, "일반 이동 설정: ControllerDesiredRotation(false), OrientRotationToMovement(true)");
    }
}

void UBaseAnimInstance::UpdateStopMotionType(EStopMotionType Type)
{
    /*
     * Stop Motion Type을 업데이트하는 함수 (기존 로직 유지)
     */
    if (StopMotionType == Type) return;

    StopMotionType = Type;

    UE_LOGFMT(LogAnimInstance, Log, "정지 모션 변경: {0}, 방향: {1}",
        *UEnum::GetValueAsString(StopMotionType),
        *UEnum::GetValueAsString(MovementDirection));
}

void UBaseAnimInstance::HandleStartRootMotion()
{
    /*
     * Root Motion 시작 처리 함수 (기존 로직 유지)
     */
    bIsPlayingRootMotionMontageWithFullBodySlot = true;
    bCanSkipStopState = true;

    UE_LOGFMT(LogAnimInstance, Log, "Root Motion 시작");
}

void UBaseAnimInstance::HandleEndRootMotion()
{
    /*
     * Root Motion 종료 처리 함수 (기존 로직 유지)
     */
    bIsPlayingRootMotionMontageWithFullBodySlot = false;
    bool bShouldSkipBasedOnCombat = IsInGuardCombatState();
    bCanSkipStopState = bShouldSkipBasedOnCombat;

    UE_LOGFMT(LogAnimInstance, Log, "Root Motion 종료");
}

void UBaseAnimInstance::ListenToCombatStateAttributeChange()
{
    /*
     * Combat State 속성 변화를 관찰하는 함수 (기존 로직 유지)
     */
    if (!OwnerCharacterBaseRef.IsValid())
    {
        return;
    }

    const auto ASC = OwnerCharacterBaseRef->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogAnimInstance, Error, "어빌리티 시스템 컴포넌트가 유효하지 않습니다.");
        return;
    }

    const UAttributeSet* AttributeSet = ASC->GetAttributeSet(UBaseAttributeSet::StaticClass());
    if (!AttributeSet)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "어트리뷰트 셋이 유효하지 않습니다.");
        return;
    }

    const auto BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
    if (!BaseAttributeSet)
    {
        return;
    }

    FGameplayAttribute CombatStateAttribute = BaseAttributeSet->GetCombatStateAttribute();
    if (CombatStateAttribute.IsValid())
    {
        CombatStateAttributeListenerRef = UAsyncTaskAttributeChanged::ListenToAttributeValueChange(
            ASC,
            CombatStateAttribute);

        if (CombatStateAttributeListenerRef)
        {
            CombatStateAttributeListenerRef->OnAttributeValueChanged.AddDynamic(
                this,
                &UBaseAnimInstance::OnCombatStateAttributeValueChanged);
            UE_LOGFMT(LogAnimInstance, Log, "전투 상태 어트리뷰트 리스너가 생성되었습니다.");
        }
    }
}
#pragma endregion
#pragma endregion

// =====================================================
// 콜백 함수들 영역
// =====================================================
#pragma region Callbacks

void UBaseAnimInstance::OnLockOnStateChanged(bool bIsLockOn, AActor* LockOnTargetActor)
{
    /*
     * Lock On 상태 변화 콜백 함수 (기존 로직 유지)
     */
    if (!OwnerCharacterBaseRef.IsValid() || !CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogAnimInstance, Warning, "OnLockOnStateChanged 실패 - 참조 객체가 유효하지 않음");
        return;
    }

    bEnableDirectionalMovement = bIsLockOn;

    UE_LOGFMT(LogAnimInstance, Log, "{0} - Lock On 상태 변경: {1} -> DirectionalMovement: {2}",
        *OwnerCharacterBaseRef->GetName(),
        bIsLockOn,
        bEnableDirectionalMovement);

    UpdateMovementSettings();
}

void UBaseAnimInstance::OnCombatStateAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
    // 이전 Combat Type 저장
    const ECombatType OldCombatType = CombatType;

    // 새로운 Combat Type 계산 및 설정
    CombatType = static_cast<ECombatType>(FMath::RoundToInt(FMath::Clamp(NewValue, 0.f,
        static_cast<float>(ECombatType::MAX) - 1)));

    UE_LOGFMT(LogAnimInstance, Log, "전투 상태 변경: {0}({1}) -> {2}({3}) (값: {4})",
        *UEnum::GetValueAsString(OldCombatType), static_cast<uint8>(OldCombatType),
        *UEnum::GetValueAsString(CombatType), static_cast<uint8>(CombatType),
        NewValue);

    // === 새로 추가: 발도술 자세 이동 블록 처리 ===
    HandleBattoujutsuMovementBlock(OldCombatType, CombatType);

    // === 기존 로직: NormalCombat에서 GuardCombat으로 변경 시 강제 Idle 전환 처리 ===
    if (OldCombatType == ECombatType::NormalCombat && CombatType == ECombatType::GuardCombat)
    {
        if (MovementState == EMovementState::Start || MovementState == EMovementState::Stop)
        {
            UE_LOGFMT(LogAnimInstance, Warning, "Combat State Normal->Guard 변경으로 인한 강제 Idle 전환: {0} -> Idle",
                *UEnum::GetValueAsString(MovementState));

            LastMovementState = MovementState;
            MovementState = EMovementState::Idle;
            OnMovementStateChanged();
            return;
        }

        if (MovementState == EMovementState::Cycle_Walk || MovementState == EMovementState::Cycle_Sprint)
        {
            UE_LOGFMT(LogAnimInstance, Log, "Combat State Normal->Guard 변경 - Cycle 상태 유지: {0}",
                *UEnum::GetValueAsString(MovementState));
        }
    }

    if (OldCombatType == ECombatType::GuardCombat && CombatType == ECombatType::NormalCombat)
    {
        UE_LOGFMT(LogAnimInstance, Log, "Combat State Guard->Normal 변경, 현재 Movement State: {0}",
            *UEnum::GetValueAsString(MovementState));
    }
}

void UBaseAnimInstance::MontageStarted(UAnimMontage* Montage)
{
    /*
     * 몽타주 시작 콜백 함수 (기존 로직 유지)
     */
    if (!Montage)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "몽타주 시작 처리 실패: 몽타주가 유효하지 않음");
        return;
    }

    bool bIsFullBody = IsFullBodySlotMontage(Montage);

    if (bIsFullBody)
    {
        UE_LOGFMT(LogAnimInstance, Log, "전체 바디 몽타주 시작: {0}", *Montage->GetName());

        //@Sprint Stop 몽타주 감지 및 처리
        FString MontageName = Montage->GetName();
        if (MontageName.Contains("SprintStop"))
        {
            bIsRootMotionCooldown = true;
            CurrentRootMotionCooldownTime = 0.0f;

            UE_LOGFMT(LogAnimInstance, Log, "Sprint Stop 몽타주 감지: {0} - Root Motion Cooldown 활성화", *MontageName);

            //@이동 설정 즉시 업데이트
            UpdateMovementSettings();
        }

        HandleStartRootMotion();
    }
    else
    {
        UE_LOGFMT(LogAnimInstance, Log, "상체 몽타주 시작: {0} - Movement State 유지", *Montage->GetName());
    }
}

void UBaseAnimInstance::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    /*
     * 몽타주 종료 콜백 함수 (스마트 Root Motion 정리 + Sprint Stop 감지)
     */
    if (!Montage)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "몽타주 종료 처리 실패: 몽타주가 유효하지 않음");
        return;
    }

    bool bIsFullBody = IsFullBodySlotMontage(Montage);



    // === bInterrupted 상황 처리 (개선됨) ===
    if (bInterrupted)
    {
        if (bIsFullBody)
        {
            UE_LOGFMT(LogAnimInstance, Warning, "⚠️ 전체 바디 몽타주 중단됨: {0}", *Montage->GetName());
            UE_LOGFMT(LogAnimInstance, Warning, "   └─ 중단 시점 상태: Movement={0}, RootMotion플래그={1}",
                *UEnum::GetValueAsString(MovementState),
                bIsPlayingRootMotionMontageWithFullBodySlot);

            // === 스마트 Root Motion 정리 로직 ===
            // 현재 활성화된 몽타주 중에 다른 FullBody가 있는지 확인
            bool bHasOtherActiveFullBody = false;

            if (FAnimMontageInstance* ActiveMontage = GetActiveMontageInstance())
            {
                // 현재 활성 몽타주가 있고, 종료되는 몽타주와 다르며, FullBody인지 확인
                if (ActiveMontage->Montage &&
                    ActiveMontage->Montage != Montage &&
                    IsFullBodySlotMontage(ActiveMontage->Montage))
                {
                    bHasOtherActiveFullBody = true;
                    UE_LOGFMT(LogAnimInstance, Log, "   └─ 다른 활성 FullBody 몽타주 발견: {0}",
                        *ActiveMontage->Montage->GetName());
                }
            }

            if (bHasOtherActiveFullBody)
            {
                UE_LOGFMT(LogAnimInstance, Log, "   └─ 다른 FullBody 몽타주가 활성화되어 있어 Root Motion 정리 생략");
            }
            else
            {
                UE_LOGFMT(LogAnimInstance, Warning, "   └─ 활성 FullBody 몽타주가 없음 - Root Motion 정리 실행");
                if (bIsPlayingRootMotionMontageWithFullBodySlot)
                {
                    HandleEndRootMotion();
                }
                else
                {
                    UE_LOGFMT(LogAnimInstance, Log, "   └─ Root Motion 플래그가 이미 비활성화됨");
                }
            }
        }
        else
        {
            UE_LOGFMT(LogAnimInstance, Log, "상체 몽타주 중단됨: {0} (Movement State 영향 없음)", *Montage->GetName());
        }

        return; // 중단된 경우 추가 처리 없이 종료
    }

    // === 정상 종료 처리 (기존 로직 유지) ===
    if (bIsPlayingRootMotionMontageWithFullBodySlot && bIsFullBody)
    {
        UE_LOGFMT(LogAnimInstance, Log, "✅ 전체 바디 몽타주 정상 종료: {0}", *Montage->GetName());
        HandleEndRootMotion();
    }
    else if (bIsFullBody && !bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        // FullBody 몽타주인데 Root Motion 플래그가 이미 false인 경우
        UE_LOGFMT(LogAnimInstance, Warning, "⚠️ 전체 바디 몽타주 종료되었으나 Root Motion 플래그가 이미 비활성화 상태: {0}", *Montage->GetName());
        UE_LOGFMT(LogAnimInstance, Warning, "   └─ 이미 다른 곳에서 정리되었거나 상태 불일치 발생");
    }

    UE_LOGFMT(LogAnimInstance, Log, "몽타주 정상 종료: {0}, 타입: {1}",
        *Montage->GetName(),
        bIsFullBody ? TEXT("FullBody") : TEXT("UpperBody"));
}

#pragma endregion

// =====================================================
// 유틸리티 함수들 영역
// =====================================================
#pragma region Utility Functions
bool UBaseAnimInstance::IsInGuardCombatState() const
{
    /*
     * Combat State가 2인지 확인하는 헬퍼 함수
     *
     * BattoujutsuCombat 상태에서는 특수한 전이 규칙이 적용됩니다.
     * 빠른 반응을 위해 Start/Stop 단계를 건너뛸 수 있습니다.
     */
    return CombatType == ECombatType::GuardCombat;
}

bool UBaseAnimInstance::HasMovementInput() const
{
    /*
     * 현재 이동 입력이 있는지 확인하는 헬퍼 함수 (개선됨)
     *
     * 기존: Acceleration.IsNearlyZero() - 너무 엄격 (거의 완벽한 0)
     * 개선: 더 관대한 임계값 사용 - 방향 전환 시에도 자연스럽게 반응
     */
    if (!CharacterMovementCompRef.IsValid())
    {
        return false;
    }

    FVector Acceleration = CharacterMovementCompRef->GetCurrentAcceleration();

    // 기존: !Acceleration.IsNearlyZero() && bShouldMove
    // 개선: 더 관대한 임계값 사용
    bool bHasAcceleration = Acceleration.SizeSquared() > (MovementInputThreshold * MovementInputThreshold);

    UE_LOGFMT(LogAnimInstance, VeryVerbose, "HasMovementInput - Acceleration: {0}, SizeSquared: {1}, Threshold: {2}, HasAccel: {3}, ShouldMove: {4}",
        Acceleration.ToString(),
        Acceleration.SizeSquared(),
        MovementInputThreshold * MovementInputThreshold,
        bHasAcceleration,
        bShouldMove);

    return bHasAcceleration && bShouldMove;
}

EMovementState UBaseAnimInstance::DetermineTargetCycleState() const
{
    /*
     * 현재 속도에 따라 목표 Cycle 상태를 결정하는 함수
     *
     * 캐릭터의 최대 속도 설정을 기반으로
     * Walk 또는 Sprint 중 어느 상태로 가야 하는지 판단합니다.
     */
    if (!CharacterMovementCompRef.IsValid())
    {
        return EMovementState::Cycle_Walk;
    }

    float MaxWalkSpeed = CharacterMovementCompRef->MaxWalkSpeed;
    bool bIsSprinting = MaxWalkSpeed >= 650.f; // 기존 로직과 동일

    return bIsSprinting ? EMovementState::Cycle_Sprint : EMovementState::Cycle_Walk;
}

bool UBaseAnimInstance::IsFullBodySlotMontage(const UAnimMontage* Montage) const
{
    /*
     * 몽타주가 전체 바디 슬롯을 사용하는지 확인하는 함수 (기존 로직 유지)
     */
    if (!Montage || Montage->SlotAnimTracks.Num() == 0)
    {
        return false;
    }

    for (const FSlotAnimationTrack& Track : Montage->SlotAnimTracks)
    {
        FString SlotNameStr = Track.SlotName.ToString();

        // 1. 전체 이름이 정확히 일치하는 경우
        if (Track.SlotName.IsEqual(FName("DefaultGroup.FullBodySlot"), ENameCase::IgnoreCase))
        {
            return true;
        }

        // 2. 슬롯 이름이 "FullBodySlot"으로 끝나는 경우
        if (SlotNameStr.EndsWith("FullBodySlot", ESearchCase::IgnoreCase))
        {
            return true;
        }
    }

    return false;
}

FString UBaseAnimInstance::GetCurrentStateDebugInfo() const
{
    /*
     * 현재 상태 기계의 모든 핵심 정보를 문자열로 반환하는 디버깅 함수
     *
     * 개발 중에 상태 기계가 올바르게 작동하는지 확인하거나
     * 문제 상황을 분석할 때 사용합니다.
     */
    FString DebugInfo = FString::Printf(TEXT("Current: %s | Last: %s | Combat: %d | CanSkipStart: %s | CanSkipStop: %s | Speed: %.1f | ShouldMove: %s"),
        *UEnum::GetValueAsString(MovementState),
        *UEnum::GetValueAsString(LastMovementState),
        static_cast<int32>(CombatType),
        bCanSkipStartState ? TEXT("true") : TEXT("false"),
        bCanSkipStopState ? TEXT("true") : TEXT("false"),
        Speed,
        bShouldMove ? TEXT("true") : TEXT("false")
    );

    return DebugInfo;
}

#pragma endregion