#pragma once

#include "BaseAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimInstance)
// UE_LOGFMT(LogAnimInstance, Log, "");

//@Defualt Setting
#pragma region Default Setting
UBaseAnimInstance::UBaseAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , LastMovementState(EMovementState::Idle)
    , MovementState(EMovementState::Idle)
    , MovementDirection(EMovementDirection::Fwd)
    , StopMotionType(EStopMotionType::None)
    , bFalling(false)
    , bShouldMove(false)
    , Velocity(FVector::ZeroVector)
    , Speed(0.0f)
    , DirectionAngle(0.0f)
    , bEnableDirectionalMovement(false)
    , bIsInDeceleration(false)
    , bModifyBoneTransform(false)
    , BoneTransformLerpSpeed(10.0f)
    , CharacterMovementCompRef(nullptr)
    , bIsSprintingCooldown(false)
    , SprintingCooldownTime(0.0f)
    , SprintingCooldownDuration(1.5f)
    , CurrentCooldownTime(0.0f)
    , bIsCombatState(false)
{
    OwnerCharacterBaseRef.Reset();
    CharacterMovementCompRef.Reset();
}

void UBaseAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

}

void UBaseAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    //@Character Base
    const auto CharacterBase = Cast<ACharacterBase>(TryGetPawnOwner());
    if (!CharacterBase)
    {
        return;
    }
    OwnerCharacterBaseRef = CharacterBase;

    //@Character Movement Component
    UCharacterMovementComponent* CharacterMovementComp = OwnerCharacterBaseRef->GetCharacterMovement();
    if (!CharacterMovementComp)
    {
        return;
    }
    CharacterMovementCompRef = CharacterMovementComp;

}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwnerCharacterBaseRef.IsValid() || !OwnerCharacterBaseRef->GetController())
    {
        return;
    }

    // 쿨다운 타이머 업데이트
    if (bIsSprintingCooldown)
    {
        CurrentCooldownTime += DeltaSeconds;
        if (CurrentCooldownTime >= SprintingCooldownDuration)
        {
            bIsSprintingCooldown = false;
            LastMovementState = MovementState;

            UE_LOGFMT(LogAnimInstance, Log, "Sprint Stop 종료 - LastState 업데이트: {0}",
                *UEnum::GetValueAsString(LastMovementState));

            UpdateMovementSettings();
        }
    }

    //@떨어지는 중인가 저장합니다.
    bFalling = OwnerCharacterBaseRef->GetMovementComponent()->IsFalling();
    //@캐릭터의 가속도를 가져옵니다.
    Velocity = OwnerCharacterBaseRef->GetVelocity();
    //@캐릭터으 현재 속도를 정의합니다.
    Speed = OwnerCharacterBaseRef->GetVelocity().Length();
    //@캐릭터의 이동/비이동 상태를 확인합니다.
    bShouldMove = Speed > 3.f && OwnerCharacterBaseRef->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;
    //@캐릭터의 이동 상태를 정의합니다. EMovementState(열거형) 유형의 변수로 나타냅니다.
    FindMovementState();
    //@캐릭터의 이동 방향을 정의합니다.
    FindMovementDirection();
    //@Upper Body Slot 업데이트 여부
    if (bIsCombatState) UpdateUpperBodyAnimation();
    
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseAnimInstance::FindMovementState()
{
    //@감속 중
    if (bIsInDeceleration)
    {
        MovementState = EMovementState::Idle;
        return;
    }

    //@이전 이동 상태 값 기억
    LastMovementState = MovementState;

    //@현재 스피드
    float CurrentSpeed = Speed;
    //@최대 속도
    float MaxWalkSpeed = OwnerCharacterBaseRef->GetCharacterMovement()->MaxWalkSpeed;
    //@달리기 여부
    bool bIsSprinting = MaxWalkSpeed >= 550.f;

    //@상태 결정
    if (CurrentSpeed < 0.05f)
    {
        MovementState = EMovementState::Idle;
    }
    else
    {
        MovementState = bIsSprinting ? EMovementState::Sprinting : EMovementState::Walking;
    }

    // 상태가 변경되었을 때만 로그 출력
    if (LastMovementState != MovementState)
    {
        UE_LOGFMT(LogAnimInstance, Log, "이동 상태 변경: {0} -> {1}",
            *UEnum::GetValueAsString(LastMovementState),
            *UEnum::GetValueAsString(MovementState));

        UpdateMovementSettings();
    }
}

void UBaseAnimInstance::FindMovementDirection()
{
    if (!bEnableDirectionalMovement || MovementState == EMovementState::Sprinting || bIsSprintingCooldown)
    {
        DirectionAngle = 0.f;
        MovementDirection = EMovementDirection::Fwd;
        return;
    }

    EMovementDirection PrevDirection = MovementDirection;
    FRotator Rotation = OwnerCharacterBaseRef->GetControlRotation();
    DirectionAngle = CalculateDirection(Velocity, Rotation);

    if (Velocity.SizeSquared() < 25.0f)
    {
        MovementDirection = EMovementDirection::Fwd;
        return;
    }


    if (PrevDirection != MovementDirection)
    {
        UE_LOGFMT(LogAnimInstance, Log, "이동 방향 변경: {0} -> {1}",
            *UEnum::GetValueAsString(PrevDirection),
            *UEnum::GetValueAsString(MovementDirection));
    }
}

void UBaseAnimInstance::UpdateMovementSettings()
{
    if (!CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogAnimInstance, Error, "Movement Component가 유효하지 않습니다.");
        return;
    }

    bool bShouldUseDirectionalMovement = bEnableDirectionalMovement &&
        MovementState != EMovementState::Sprinting &&
        !bIsSprintingCooldown;

    UE_LOGFMT(LogAnimInstance, Log, "이동 방향 설정 값: DirectionalMovement({0}), MovementState({1}), SprintingCooldown({2})",
        bEnableDirectionalMovement,
        *UEnum::GetValueAsString(MovementState),
        bIsSprintingCooldown);

    if (bShouldUseDirectionalMovement)
    {
        CharacterMovementCompRef->bUseControllerDesiredRotation = true;
        CharacterMovementCompRef->bOrientRotationToMovement = false;

        UE_LOGFMT(LogAnimInstance, Log, "방향 이동 설정: ControllerDesiredRotation(true), OrientRotationToMovement(false)");
    }
    else
    {
        CharacterMovementCompRef->bUseControllerDesiredRotation = false;
        CharacterMovementCompRef->bOrientRotationToMovement = true;

        UE_LOGFMT(LogAnimInstance, Log, "일반 이동 설정: ControllerDesiredRotation(false), OrientRotationToMovement(true)");
    }
}

void UBaseAnimInstance::UpdateStopMotionType(EStopMotionType Type)
{
    if (StopMotionType == Type) return;

    StopMotionType = Type;
    UE_LOGFMT(LogAnimInstance, Log, "정지 모션 변경: {0}", *UEnum::GetValueAsString(StopMotionType));
}

void UBaseAnimInstance::ChangeCombatState(bool bEnterCombat)
{
    // 상태 변경이 필요한지 체크
    if (bIsCombatState == bEnterCombat)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "전투 상태가 이미 {0} 상태입니다.",
            bEnterCombat ? TEXT("전투") : TEXT("비전투"));
        return;
    }

    // 상태 변경 및 로깅
    bIsCombatState = bEnterCombat;
    UE_LOGFMT(LogAnimInstance, Log, "전투 상태가 {0}(으)로 변경되었습니다.",
        bEnterCombat ? TEXT("전투") : TEXT("비전투"));
}

void UBaseAnimInstance::UpdateUpperBodyAnimation()
{
    // 전제 조건 체크
    if (!ensureMsgf(OwnerCharacterBaseRef.IsValid(),
        TEXT("상반신 애니메이션 업데이트 실패: 캐릭터가 유효하지 않습니다.")))
    {
        return;
    }

    // 애니메이션 업데이트 시작
    UE_LOGFMT(LogAnimInstance, Log, "상반신 애니메이션 업데이트 시작");

    //@TODO: Upper Body Slot의 상반신 애님 몽타주 재생

    UE_LOGFMT(LogAnimInstance, Log, "상반신 애니메이션 업데이트 완료");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UBaseAnimInstance::OnLockOnStateChanged(bool bIsLockOn)
{
    if (!OwnerCharacterBaseRef.IsValid() || !CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogAnimInstance, Log, "Owner Character가 유효하지 않습니다.");
        return;
    }

    //@Lock On 상태 저장
    bEnableDirectionalMovement = bIsLockOn;

    //@이동 설정 업데이트
    UpdateMovementSettings();
}

void UBaseAnimInstance::OnDecelerationStateChanged(bool bIsDecelerating)
{
    bIsInDeceleration = bIsDecelerating;
    UE_LOGFMT(LogAnimInstance, Log, "감속 상태 변경: {0}", bIsDecelerating);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion