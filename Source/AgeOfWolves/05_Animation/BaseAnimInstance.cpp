#pragma once

#include "BaseAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"

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
    , bModifyBoneTransform(false)
    , BoneTransformLerpSpeed(10.0f)
    , CharacterMovementCompRef(nullptr)
    , bIsSprintingCooldown(false)
    , SprintingCooldownTime(0.0f)
    , SprintingCooldownDuration(1.5f)
    , CurrentCooldownTime(0.0f)
    , CombatType(ECombatType::NonCombat)
    , bIsPlayingRootMotionMontage(false)
    , bIsRootMotionCooldown(false)
    , RootMotionCooldownTime(0.0f)
    , RootMotionCooldownDuration(1.5f)
    , CurrentRootMotionCooldownTime(0.0f)
{
    OwnerCharacterBaseRef.Reset();
    CharacterMovementCompRef.Reset();
    CombatStateAttributeListenerRef = nullptr;
}

void UBaseAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    //@Combat State 속성 수치 변화 관찰
    ListenToCombatStateAttributeChange();

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

    //@Character Base, Controller
    if (!OwnerCharacterBaseRef.IsValid() || !OwnerCharacterBaseRef->GetController())
    {
        return;
    }

    // Root Motion Cooldown 처리
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

    //@Sprinting 쿨 다운 타이머
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
    //@캐릭터의 이동 방향 각도를 정의합니다.
    FindMovementDirectionAngle();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseAnimInstance::FindMovementState()
{
    if (bIsPlayingRootMotionMontage)
    {
        MovementState = EMovementState::Idle;
        return;
    }

    LastMovementState = MovementState;

    float CurrentSpeed = Speed;
    float MaxWalkSpeed = OwnerCharacterBaseRef->GetCharacterMovement()->MaxWalkSpeed;
    bool bIsSprinting = MaxWalkSpeed >= 550.f;

    if (CurrentSpeed < 0.05f)
    {
        MovementState = EMovementState::Idle;
    }
    else
    {
        MovementState = bIsSprinting ? EMovementState::Sprinting : EMovementState::Walking;
    }

    if (LastMovementState != MovementState)
    {
        // Owner 이름과 함께 이동 상태 변경 로그 출력
        UE_LOGFMT(LogAnimInstance, Log, "{0} - 이동 상태 변경: {1} -> {2}",
            *OwnerCharacterBaseRef->GetName(),
            *UEnum::GetValueAsString(LastMovementState),
            *UEnum::GetValueAsString(MovementState));

        //@Idle로 전환 시 이전 상태에 따라 StopMotion 결정
        if (LastMovementState == EMovementState::Walking || LastMovementState == EMovementState::Sprinting)
        {
            LastMovementState == EMovementState::Walking ?
                UpdateStopMotionType(EStopMotionType::WalkStop)
                : UpdateStopMotionType(EStopMotionType::SprintStop);
        }

        UpdateMovementSettings();
    }
}

void UBaseAnimInstance::FindMovementDirectionAngle()
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

    // 방향 각도에 따른 이동 방향 설정
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

    bool bShouldUseDirectionalMovement = bEnableDirectionalMovement 
        && MovementState != EMovementState::Sprinting 
        && !bIsSprintingCooldown;

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

    //@Stop Motion Type 업데이트
    StopMotionType = Type;

    //@Sprint Stop일 경우,
    StopMotionType == EStopMotionType::SprintStop ? bIsSprintingCooldown = true : nullptr;

    UE_LOGFMT(LogAnimInstance, Log, "정지 모션 변경: {0}", *UEnum::GetValueAsString(StopMotionType));
}

void UBaseAnimInstance::HandleStartRootMotion()
{
    bIsPlayingRootMotionMontage = true;
    UE_LOGFMT(LogAnimInstance, Log, "Root Motion 시작");
}

void UBaseAnimInstance::HandleEndRootMotion()
{
    bIsPlayingRootMotionMontage = false;
    bIsRootMotionCooldown = true;
    CurrentRootMotionCooldownTime = 0.0f;

    UE_LOGFMT(LogAnimInstance, Log, "Root Motion 종료 - Cooldown 시작");

    UpdateMovementSettings();
}

void UBaseAnimInstance::ListenToCombatStateAttributeChange()
{
    //@Owner Character Ref
    if (!OwnerCharacterBaseRef.IsValid())
    {
        return;
    }

    //@ASC
    const auto ASC = OwnerCharacterBaseRef->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogAnimInstance, Error, "어빌리티 시스템 컴포넌트가 유효하지 않습니다.");
        return;
    }

    //@AttributeSet
    const UAttributeSet* AttributeSet = ASC->GetAttributeSet(UBaseAttributeSet::StaticClass());
    if (!AttributeSet)
    {
        UE_LOGFMT(LogAnimInstance, Warning, "어트리뷰트 셋이 유효하지 않습니다.");
        return;
    }

    //@Base Attribute Set
    const auto BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
    if (!BaseAttributeSet)
    {
        return;
    }

    //@Combat State 속성 관찰
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

void UBaseAnimInstance::OnCombatStateAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
    //@Old Combat Type
    const ECombatType OldCombatType = OldValue > 0.f ? ECombatType::NormalCombat : ECombatType::NonCombat;

    //@Combat Type
    CombatType = NewValue > 0.f ? ECombatType::NormalCombat : ECombatType::NonCombat;

    UE_LOGFMT(LogAnimInstance, Log, "전투 상태 변경: {0} -> {1} (값: {2})",
        static_cast<uint8>(OldCombatType),
        static_cast<uint8>(CombatType),
        NewValue);


    //@TODO: 임시 무기 착용 시에만 활용하는 코드
    auto OwnerPlayer = Cast<APlayerCharacter>(OwnerCharacterBaseRef.Get());
    if (!OwnerPlayer)
    {
        return;
    }

    //@임시 코드
    UStaticMeshComponent* WeaponMesh = OwnerPlayer->GetWeaponMesh();
    UStaticMeshComponent* ShealthMesh = OwnerPlayer->GetShealthMesh();
    UStaticMeshComponent* FullMesh = OwnerPlayer->GetFullWeaponMesh();

    if (!WeaponMesh || !ShealthMesh || !FullMesh)
    {
        return;
    }

    //@TODO: 지울 예정
    const float VisibilityDelay = 0.5f;

    if (NewValue > 0.f)
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [=]()
            {
                WeaponMesh->SetVisibility(true);
                ShealthMesh->SetVisibility(true);
                FullMesh->SetVisibility(false);
            },
            VisibilityDelay,
                false
                );
    }
    else
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [=]()
            {
                WeaponMesh->SetVisibility(false);
                ShealthMesh->SetVisibility(false);
                FullMesh->SetVisibility(true);
            },
            VisibilityDelay + 1.f,
                false
                );
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion