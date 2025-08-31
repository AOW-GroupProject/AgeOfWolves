// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"            // ECollisionEnabled
#include "Components/PrimitiveComponent.h" // UPrimitiveComponent
#include "GameplayTagContainer.h"          // FGameplayTag
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void PerformInteraction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void SetMeshCollision(UMeshComponent* MeshComp, ECollisionEnabled::Type NewCollision);

	// UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual FGameplayTag GetObjectTag() const = 0;
};

