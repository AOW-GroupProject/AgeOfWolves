// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "BaseGameplayAbility.generated.h"

class UGameplayEffect;
class UAbilityTagRelationshipMapping;

DECLARE_LOG_CATEGORY_EXTERN(LogGA, Log, All)

UENUM(BlueprintType)	
enum class EAbilityActivationPolicy : uint8
{
	/*
	* @목적: Active GA 전용 활성화 정책
	* @설명: Active GA는 사용자 입력이 Trigger 매개이며, 사용자 입력과 관련하여 활성화 정책을 선택합니다.
	*/
	// Input이 Trigger 되면 활성화됩니다.
	OnInputTriggered,

	// Input이 Hold 상태 일때 지속적으로 활성화됩니다.
	WhileInputActive,

	/*
	* @목적: Passive GA 전용 활성화 정책
	* @설명: Passive GA라면, 해당 정책을 활용하여 등록 시 활성화 되도록 합니다.
	*/
	OnGranted,

	MAX
};



/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

#pragma region Friend Class
		friend class UBaseAbilitySystemComponent;
#pragma endregion

public:
	UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#pragma region Default Setting
protected:
	//~UGameplayAbility interface, ASC와 상호작용
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	//~End of Interface
#pragma endregion

#pragma region Gameplay Tag Relationship Mapping
protected:
	//~UGameplayAbility interface, Overloading
	bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, 
		OUT FGameplayTagContainer* ARTags= nullptr,
		OUT FGameplayTagContainer* ABTags = nullptr) const;
	bool DoesAbilitySatisfyTagRequirements(
		const UAbilitySystemComponent& AbilitySystemComponent, 
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
		FGameplayTagContainer* ARTags = nullptr, FGameplayTagContainer* ABTags = nullptr) const;
	//~End of Interface
#pragma endregion

#pragma region GA Info
protected:
	// @목적 : Gameplay Ability의 발동 조건입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability | Ability Activation")
		EAbilityActivationPolicy ActivationPolicy;
	// @설명 : 해당 Gameplay Ability의 활성화 과정에서 Target(GA의 적용 대상)에게 전달하는 Gameplay Effect입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability | Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplyGameplayEffectClass;

public:
	FORCEINLINE EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FORCEINLINE UGameplayEffect* GetApplyGameplayEffect() { return ApplyGameplayEffectClass->IsValidLowLevel() ? ApplyGameplayEffectClass->GetDefaultObject<UGameplayEffect>() : nullptr; }
#pragma endregion


};
