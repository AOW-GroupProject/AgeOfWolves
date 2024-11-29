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

	// Max Walk Speed 로그 출력
	UE_LOGFMT(LogAnimInstance, Log, "Current Max Walk Speed: {0}", MaxWalkSpeed);

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
	//캐릭터가 LockOn을 취소하거나, Sprint 중일 때는 정면으로 설정
	if (bLockOn == false || MovementState == EMovementState::Sprinting || LastMovementState == EMovementState::Sprinting)
	{
		DirectionAngle = 0.f;
		MovementDirection = EMovementDirection::Fwd;
		return;
	}

	// 캐릭터의 현재 가속도 벡터를 기반으로 한 로테이션을 계산
	FRotator Rotation1 = UKismetMathLibrary::MakeRotFromX(OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration());

	// 캐릭터가 바라보는 방향의 벡터를 기반으로 로테이션을 계산
	FRotator Rotation2 = OwnerCharacterBase->GetControlRotation();

	// 두 방향 사이의 각도 차이를 계산 (-180 ~ 180 범위)
	DirectionAngle = FMath::FindDeltaAngleDegrees(Rotation1.Yaw, Rotation2.Yaw);

	// Movement Direction 업데이트
	if (DirectionAngle >= -70.f && DirectionAngle <= 70.f)
	{
		MovementDirection = EMovementDirection::Fwd;
	}
	else if (DirectionAngle > 70.f && DirectionAngle < 110.f)
	{
		MovementDirection = EMovementDirection::Left;
	}
	else if (DirectionAngle > -110.f && DirectionAngle < -70.f)
	{
		MovementDirection = EMovementDirection::Right;
	}
	else
	{
		MovementDirection = EMovementDirection::Bwd;
	}

	// 디버그 로그
	UE_LOGFMT(LogAnimInstance, Verbose, "Movement Direction Angle: {0}, Direction: {1}",
		DirectionAngle,
		*UEnum::GetValueAsString(MovementDirection));
}

#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion