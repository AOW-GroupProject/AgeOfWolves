#include "BaseAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimInstance)
// UE_LOGFMT(LogAnimInstance, Log, "");

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

void UBaseAnimInstance::FindMovementState()
{


	// @FIX: 240 -> 350 상향
	// #1. 뛰기
	if (OwnerCharacterBase->GetCharacterMovement()->MaxWalkSpeed > 400.f && Speed > 0.05f)
	{
		MovementState = EMovementState::Run;
		if(LastMovementState == EMovementState::Idle) LastMovementState = MovementState;
	}
	// #2. 걷기
	else if (OwnerCharacterBase->GetCharacterMovement()->MaxWalkSpeed > 0.f && Speed > 10.f)
	{
		MovementState = EMovementState::Walk;
	}
	// #3. 정지
	else
	{
		MovementState = EMovementState::Idle;
	}
}

void UBaseAnimInstance::FindMovementDirection()
{
	//케릭터가 LockOn을 취소하거나, Run동안은 이동 방향을 Fwd로 설정.
	if (bLockOn == false || MovementState == EMovementState::Run)
	{
		MovementDirection = EMovementDirection::Fwd;
		return;
	}
		
	// 캐릭터의 현재 가속도 벡터를 기반으로 한 로테이션을 계산
	FRotator Rotation1 = UKismetMathLibrary::MakeRotFromX(OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration());

	// 캐릭터가 바라보는 방향의 벡터를 기반으로 로테이션을 계산
	FRotator Rotation2 = OwnerCharacterBase->GetControlRotation();

	// 두 방향 사이의 최소 각도 차이를 계산, 오른손 좌표계 기준 캐릭터의 Yaw 회전 값 기준 시계 방향은 음수, 반 시계 방향은 양수
	float Angle = FMath::FindDeltaAngleDegrees(Rotation1.Yaw, Rotation2.Yaw);

	// #1. Fwd
	if (Angle >= -70.f && Angle <= 70.f)
	{
		MovementDirection = EMovementDirection::Fwd;
	}
	// #2. Left
	else if (Angle > 70.f && Angle < 110.f)
	{
		MovementDirection = EMovementDirection::Left;
	}
	// #3. Right
	else if (Angle > -110.f && Angle < -70.f)
	{
		MovementDirection = EMovementDirection::Right;
	}
	// #4. Back
	else
	{
		MovementDirection = EMovementDirection::Bwd;
	}

}
