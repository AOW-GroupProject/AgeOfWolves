// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"

UBaseCharacterMovementComponent::UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UBaseCharacterMovementComponent::ChangeMoveSpeed(float InOldVal, float InNewVal)
{
	MaxWalkSpeed = InNewVal;

	if (InOldVal > 0.f && InOldVal <= 500.f)
	{
		if (InNewVal > InOldVal)
		{
			IsSprinting = true;
			MaxWalkSpeed = InNewVal;
		}
		else
		{
			IsSprinting = false;
			MaxWalkSpeed = InNewVal;
		}
	}
}
