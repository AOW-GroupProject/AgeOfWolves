// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "03_Player/PlayerStateBase.h"

UBaseCharacterMovementComponent::UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UBaseCharacterMovementComponent::OnRegister()
{
	Super::OnRegister();

}

void UBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const auto Owner = CastChecked<ACharacter>(GetOwner()))
	{
		if (auto PS = Owner->GetPlayerState<APlayerStateBase>())
		{
			PS->OnAnyAttributeValueChanged.AddDynamic(this, &UBaseCharacterMovementComponent::MoveSpeedChanged);
		}
	}
}

void UBaseCharacterMovementComponent::MoveSpeedChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
	if (Attribute.IsValid() && Attribute.AttributeName == "MoveSpeed")
	{
		ChangeMoveSpeed(OldValue, NewValue);
	}
}

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
