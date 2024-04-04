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
	// @���� : Gameplay Ability�� �ߵ� �����Դϴ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability|Ability Activation")
		EAbilityActivationPolicy ActivationPolicy;
	// @���� : �ش� Gameplay Ability�� Ȱ��ȭ �������� Target(GA�� ���� ���)���� �����ϴ� Gameplay Effect�Դϴ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability|Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplyGameplayEffectClass;

public:
	FORCEINLINE EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FORCEINLINE UGameplayEffect* GetApplyGameplayEffect() { return ApplyGameplayEffectClass->IsValidLowLevel() ? ApplyGameplayEffectClass->GetDefaultObject<UGameplayEffect>() : nullptr; }

};
