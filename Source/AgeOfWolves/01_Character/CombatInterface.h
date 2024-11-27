// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGEOFWOLVES_API ICombatInterface
{
	GENERATED_BODY()

public:

	virtual void Die() = 0;

	virtual void HitReact(FGameplayTag HitDirectionTag) = 0;
};
