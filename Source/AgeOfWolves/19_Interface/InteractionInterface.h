// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType,NotBlueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGEOFWOLVES_API IInteractionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void PerformInteraction() = 0;
	
	virtual FGameplayTag GetObjectTag() = 0;
};

