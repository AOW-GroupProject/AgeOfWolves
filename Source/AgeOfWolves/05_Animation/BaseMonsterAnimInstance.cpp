// Fill out your copyright notice in the Description page of Project Settings.


#include "05_Animation/BaseMonsterAnimInstance.h"
#include "01_Character/BaseMonster.h"
#include "KismetAnimationLibrary.h"
#include "AIController.h"

UBaseMonsterAnimInstance::UBaseMonsterAnimInstance()
{
}

void UBaseMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MyPlayer = Cast<ABaseMonster>(TryGetPawnOwner());
}

void UBaseMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (MyPlayer)
	{
		Speed = MyPlayer->GetVelocity().Size() / 100;
		(Speed > 0.1) ? isMoving = true : isMoving = false;
		Dir = UKismetAnimationLibrary::CalculateDirection(MyPlayer->GetVelocity(), MyPlayer->GetActorRotation());
		
		AAIController* controller = Cast<AAIController>(GetOwningActor()->GetInstigatorController());
		if (controller)
		{
			if (controller->GetFocusActor())
			{
				IsFocusingPlayer = true;
			}
			else IsFocusingPlayer = false;
		}
	}
}
