// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "BaseGameplayAbility.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#pragma region Default Setting

protected:
	// @목적 : Gameplay Ability의 발동 조건입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability|Ability Activation")
		EAbilityActivationPolicy ActivationPolicy;
	// @설명 : 해당 Gameplay Ability의 활성화 과정에서 Target(GA의 적용 대상)에게 전달하는 Gameplay Effect입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability|Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplyGameplayEffectClass;

public:
	FORCEINLINE EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FORCEINLINE UGameplayEffect* GetApplyGameplayEffect() { return ApplyGameplayEffectClass->IsValidLowLevel() ? ApplyGameplayEffectClass->GetDefaultObject<UGameplayEffect>() : nullptr; }

};
