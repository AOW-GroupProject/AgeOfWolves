// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "InputConfig.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInputConfig, Log, All)

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputActionInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
		FGameplayTag InputTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<UInputAction> InputAction = nullptr;

};

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
		const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
		const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// IMC
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
		TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
		int32 MappingPriority;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Native Input Action")
		TArray<FInputActionInfo> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Ability Input Action")
		TArray<FInputActionInfo> AbilityInputActions;

};
