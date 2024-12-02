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
	:Super(ObjectInitializer)
{}
void UBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const auto character = Cast<ACharacterBase>(TryGetPawnOwner()))
	{
		OwnerCharacterBase = character;
	}

    bLockOn = false;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacterBase.IsValid() && OwnerCharacterBase->GetController())
	{
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
}

void UBaseAnimInstance::FindMovementDirection()
{
    EMovementDirection PrevDirection = MovementDirection;

    if (!bLockOn || MovementState == EMovementState::Sprinting || LastMovementState == EMovementState::Sprinting)
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
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion