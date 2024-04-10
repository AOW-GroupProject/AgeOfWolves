// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"

UBaseCharacterMovementComponent::UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UBaseCharacterMovementComponent::ChangeMoveSpeed(float InOldVal, float InNewVal)
{
	if (InOldVal >= 0.f && InOldVal <= 550.f)
	{
		if (InNewVal > InOldVal)
		{
			IsSprinting = true;
			MaxAcceleration = 300.f;
			MaxWalkSpeed = InNewVal;
		}
		else
		{
			IsSprinting = false;
			MaxAcceleration = 150.f;
			MaxWalkSpeed = InNewVal;
		}
	}
}
