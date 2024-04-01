#include "BaseAnimInstance.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "KismetAnimationLibrary.h"


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

	if (OwnerCharacterBase.IsValid())
	{
		//�������� ���ΰ� �����մϴ�.
		bFalling = OwnerCharacterBase->GetMovementComponent()->IsFalling();
		//ĳ������ �����̴� �ӵ����� �����ɴϴ�.
		Speed = OwnerCharacterBase->GetVelocity().Length();

		Velocity = OwnerCharacterBase->GetVelocity();
		bShouldMove = Speed > 3.f && OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;


		const auto& characterRotation = OwnerCharacterBase->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, characterRotation);

	}
}
