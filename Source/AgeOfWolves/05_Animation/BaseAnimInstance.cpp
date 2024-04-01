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
		//떨어지는 중인가 저장합니다.
		bFalling = OwnerCharacterBase->GetMovementComponent()->IsFalling();
		//캐릭터의 움직이는 속도값을 가져옵니다.
		Speed = OwnerCharacterBase->GetVelocity().Length();

		Velocity = OwnerCharacterBase->GetVelocity();
		bShouldMove = Speed > 3.f && OwnerCharacterBase->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;


		const auto& characterRotation = OwnerCharacterBase->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, characterRotation);

	}
}
