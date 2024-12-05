#include "BaseAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

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
    , bFalling(false)
    , bShouldMove(false)
    , Velocity(FVector::ZeroVector)
    , Speed(0.0f)
    , DirectionAngle(0.0f)
    , bEnableDirectionalMovement(false)
    , bModifyBoneTransform(false)
    , BoneTransformLerpSpeed(10.0f)
    , OwnerCharacterBase(nullptr)
    , CharacterMovementCompRef(nullptr)
    , bIsSprintingCooldown(false)
    , SprintingCooldownTime(0.0f)
    , SprintingCooldownDuration(1.5f)
    , CurrentCooldownTime(0.0f)
{
    OwnerCharacterBase.Reset();
    CharacterMovementCompRef.Reset();
}

void UBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

    const auto CharacterBase = Cast<ACharacterBase>(TryGetPawnOwner());
    if(!CharacterBase)
	{
        return;
	}

    //@캐싱
    OwnerCharacterBase = CharacterBase;

    UCharacterMovementComponent* CharacterMovementComp = OwnerCharacterBase->GetCharacterMovement();
    if (!CharacterMovementComp)
    {
        return;
    }

    //@캐싱
    CharacterMovementCompRef = CharacterMovementComp;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwnerCharacterBase.IsValid() || !OwnerCharacterBase->GetController())
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

	// 떨어지는 중인가 저장합니다.
	bFalling = OwnerCharacterBase->GetMovementComponent()->IsFalling();
	// 캐릭터의 가속도를 가져옵니다.
	Velocity = OwnerCharacterBase->GetVelocity();
	// 캐릭터으 현재 속도를 정의합니다.
	Speed = OwnerCharacterBase->GetVelocity().Length();
	// 캐릭터의 이동/비이동 상태를 확인합니다.
	bShouldMove = Speed > 3.f && OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;
	// 캐릭터의 이동 상태를 정의합니다. EMovementState(열거형) 유형의 변수로 나타냅니다.
	FindMovementState();
	// 캐릭터의 이동 방향을 정의합니다.
	FindMovementDirection();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseAnimInstance::FindMovementState()
{
    //@이전 이동 상태 값 기억
    LastMovementState = MovementState;

    //@현재 스피드
    float CurrentSpeed = Speed;
    //@최대 속도
    float MaxWalkSpeed = OwnerCharacterBase->GetCharacterMovement()->MaxWalkSpeed;

    //@달리기 여부
    bool bIsSprinting = MaxWalkSpeed >= 550.f;

    //@EMovementState::Idle
    if (CurrentSpeed < 0.05f)
    {
        MovementState = EMovementState::Idle;
    }
    //@EMovementState::Sprinting or Walking
    else
    {
        MovementState = bIsSprinting ? EMovementState::Sprinting : EMovementState::Walking;
    }

    // Sprinting에서 다른 상태로 전환되었을 때 쿨다운 시작
    if (LastMovementState == EMovementState::Sprinting && MovementState != EMovementState::Sprinting)
    {
        bIsSprintingCooldown = true;
        CurrentCooldownTime = 0.0f;
        UE_LOGFMT(LogAnimInstance, Log, "Sprinting 종료 - 쿨다운 시작");
    }

    // MovementState가 변경되었다면 이동 설정 업데이트
    if (LastMovementState != MovementState)
    {
        UE_LOGFMT(LogAnimInstance, Log, "이동 상태 변경: {0} -> {1}, 현재 속도: {2}, 최대 속도: {3}",
            *UEnum::GetValueAsString(LastMovementState),
            *UEnum::GetValueAsString(MovementState),
            CurrentSpeed,
            MaxWalkSpeed);

        UpdateMovementSettings();
    }
}

void UBaseAnimInstance::FindMovementDirection()
{
    EMovementDirection PrevDirection = MovementDirection;

    //@방향 별 이동 가능 여부 체크 - Lock On 상태이며 Sprinting이 아닐 때만 방향별 이동 가능
    if (!bEnableDirectionalMovement || MovementState == EMovementState::Sprinting || bIsSprintingCooldown)
    {
        DirectionAngle = 0.f;
        MovementDirection = EMovementDirection::Fwd;
        return;
    }

    FRotator Rotation = OwnerCharacterBase->GetControlRotation();
    DirectionAngle = CalculateDirection(Velocity, Rotation);

    // 속도가 너무 낮으면 전방으로 설정
    if (Velocity.SizeSquared() < 25.0f)
    {
        MovementDirection = EMovementDirection::Fwd;
        return;
    }

    UE_LOGFMT(LogAnimInstance, Log, "Angle Update - DirectionAngle: {0}, Movement: {1}",
        DirectionAngle,
        *UEnum::GetValueAsString(MovementDirection));

    const float HYSTERESIS = 15.f;

    // 현재 상태에 따른 방향 전환 결정
    if (MovementDirection == EMovementDirection::Bwd)
    {
        // Backward 상태에서는 더 큰 각도 변화가 있어야 상태 전환
        if (FMath::Abs(DirectionAngle) < (135.f - HYSTERESIS))
        {
            MovementDirection = (DirectionAngle > 0.f) ? EMovementDirection::Right : EMovementDirection::Left;
        }
    }
    else if (MovementDirection == EMovementDirection::Right)
    {
        // Right 상태에서는 각도가 더 커야 Backward로 전환
        if (DirectionAngle > (135.f + HYSTERESIS))
        {
            MovementDirection = EMovementDirection::Bwd;
        }
        else if (DirectionAngle <= 45.f)
        {
            MovementDirection = EMovementDirection::Fwd;
        }
    }
    else if (MovementDirection == EMovementDirection::Left)
    {
        // Left 상태에서는 각도가 더 작아야 Backward로 전환
        if (DirectionAngle < -(135.f + HYSTERESIS))
        {
            MovementDirection = EMovementDirection::Bwd;
        }
        else if (DirectionAngle >= -45.f)
        {
            MovementDirection = EMovementDirection::Fwd;
        }
    }
    else  // Forward 상태
    {
        if (DirectionAngle > 45.f && DirectionAngle <= 135.f)
        {
            MovementDirection = EMovementDirection::Right;
        }
        else if (DirectionAngle < -45.f && DirectionAngle >= -135.f)
        {
            MovementDirection = EMovementDirection::Left;
        }
        else if (FMath::Abs(DirectionAngle) > 135.f)
        {
            MovementDirection = EMovementDirection::Bwd;
        }
    }

    if (PrevDirection != MovementDirection)
    {
        UE_LOGFMT(LogAnimInstance, Log, "Direction Changed - Angle: {0}, From: {1} To: {2}",
            DirectionAngle,
            *UEnum::GetValueAsString(PrevDirection),
            *UEnum::GetValueAsString(MovementDirection));
    }
}

void UBaseAnimInstance::UpdateMovementSettings()
{
    if (!CharacterMovementCompRef.IsValid()) return;

    // Sprinting 중이거나 쿨다운 중일 때는 항상 정면 이동
    bool bShouldUseDirectionalMovement = bEnableDirectionalMovement &&
        MovementState != EMovementState::Sprinting &&
        !bIsSprintingCooldown;

    if (bShouldUseDirectionalMovement)
    {
        //@방향별 이동 설정
        CharacterMovementCompRef->bUseControllerDesiredRotation = true;
        CharacterMovementCompRef->bOrientRotationToMovement = false;

        UE_LOGFMT(LogAnimInstance, Log, "방향별 이동 설정 적용");
    }
    else
    {
        //@정면 이동 설정
        CharacterMovementCompRef->bUseControllerDesiredRotation = false;
        CharacterMovementCompRef->bOrientRotationToMovement = true;

        const FString Reason = MovementState == EMovementState::Sprinting ?
            "Sprinting" : (bIsSprintingCooldown ? "Sprint Cooldown" : "Not Lock On");
        UE_LOGFMT(LogAnimInstance, Log, "정면 이동 설정 적용 - 사유: {0}", Reason);
    }
}

void UBaseAnimInstance::UpdateStopMotionType(EStopMotionType Type)
{
    // 사이클의 마지막 프레임에서 현재 상태를 LastMovementState에 저장
    StopMotionType = Type;

    UE_LOGFMT(LogAnimInstance, Log, "Last Movement State 업데이트: {0}",
        *UEnum::GetValueAsString(LastMovementState));
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UBaseAnimInstance::OnLockOnStateChanged(bool bIsLockOn)
{
    if (!OwnerCharacterBase.IsValid() || !CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogAnimInstance, Log, "Owner Character가 유효하지 않습니다.");
        return;
    }

    //@Lock On 상태 저장
    bEnableDirectionalMovement = bIsLockOn;

    //@이동 설정 업데이트
    UpdateMovementSettings();
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion