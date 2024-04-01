// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BaseCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
		void ChangeMoveSpeed(float InOldVal, float InNewVal);

private:
	bool IsSprinting;

};
