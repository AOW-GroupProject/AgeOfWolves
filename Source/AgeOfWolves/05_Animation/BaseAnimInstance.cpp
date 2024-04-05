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
		// �������� ���ΰ� �����մϴ�.
		bFalling = OwnerCharacterBase->GetMovementComponent()->IsFalling();
		// ĳ������ ���ӵ��� �����ɴϴ�.
		Velocity = OwnerCharacterBase->GetVelocity();
		// ĳ������ ���� �ӵ��� �����մϴ�.
		Speed = OwnerCharacterBase->GetVelocity().Length();
		// ĳ������ �̵�/���̵� ���¸� Ȯ���մϴ�.
		bShouldMove = Speed > 3.f && OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;
		// ĳ������ �̵� ������ �����մϴ�.
		FindMovementDirection();
		// ĳ������ �̵� ���¸� �����մϴ�. EMovementState(������) ������ ������ ��Ÿ���ϴ�.
		FindMovementState();
	}
}

void UBaseAnimInstance::FindMovementState()
{
	// ĳ������ �̵� ���¸� �����մϴ�.
	FVector Vector1 = OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration();
	FVector Vector2 = Velocity;

	float Length1 = OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration().Length();
	float Length2 = Speed;

	Vector1.Normalize();
	Vector2.Normalize();

	// #1. ���� ��ȯ
	if (FVector::DotProduct(Vector1, Vector2) < -0.75f)
	{
		MovementState = EMovementState::Pivoting;
	}
	// #2. �ٱ�
	else if (OwnerCharacterBase->GetCharacterMovement()->MaxWalkSpeed > 300.f && Length1 > 0.5f && Length2 > 1.0)
	{
		MovementState = EMovementState::Run;
	}
	// #3. �ȱ�
	else if (OwnerCharacterBase->GetCharacterMovement()->MaxWalkSpeed > 1.f && Length1 > 0.01f && Length2 > 0.f)
	{
		MovementState = EMovementState::Walk;
	}
	// #4. ����
	else
	{
		MovementState = EMovementState::Idle;
	}
}

void UBaseAnimInstance::FindMovementDirection()
{
	// ĳ������ ���� ���ӵ� ���͸� ������� �� �����̼��� ���
	FRotator Rotation1 = UKismetMathLibrary::MakeRotFromX(OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration());

	// ĳ���Ͱ� �ٶ󺸴� ������ ���͸� ������� �����̼��� ���
	FRotator Rotation2 = OwnerCharacterBase->GetControlRotation();

	// �� ���� ������ �ּ� ���� ���̸� ���
	float Angle = FMath::FindDeltaAngleDegrees(Rotation1.Yaw, Rotation2.Yaw);

	if (Angle >= -70.f && Angle <= 70.f)
	{
		MovementDirection = EMovementDirection::Fwd;
		UE_LOG(LogAnimInstance, Log, TEXT("Fwd : %f"), Angle);
	}
	else if (Angle > 70.f && Angle < 110.f)
	{
		MovementDirection = EMovementDirection::Left;
		UE_LOG(LogAnimInstance, Log, TEXT("Left : %f"), Angle);
	}
	else if (Angle > -110.f && Angle < -70.f)
	{
		MovementDirection = EMovementDirection::Right;
		UE_LOG(LogAnimInstance, Log, TEXT("Right : %f"), Angle);
	}
	else
	{
		MovementDirection = EMovementDirection::Bwd;
		UE_LOG(LogAnimInstance, Log, TEXT("Bwd : %f"), Angle);
	}

}
