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
 * @UInputConfig
 * 
 * Enhanced Input 활용을 위해 필요한 정보들을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UInputConfig(const FObjectInitializer& ObjectInitializer);

public:
	

public:
	UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
		const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
		const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
		const UInputAction* FindUIInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
		FGameplayTag IMCTag;
	//@IMC
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
		TObjectPtr<UInputMappingContext> InputMappingContext;
	//@Pirority
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
		int32 MappingPriority;
	//@Native IA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Native Input Action")
		TArray<FInputActionInfo> NativeInputActions;
	//@Ability IA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Ability Input Action")
		TArray<FInputActionInfo> AbilityInputActions;
	//@UI IA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | UI Input Action")
		TArray<FInputActionInfo> UIInputActions;

};
