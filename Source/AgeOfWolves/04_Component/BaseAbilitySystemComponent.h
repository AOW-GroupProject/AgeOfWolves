// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

#include "BaseAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASC, Log, All);

/**
 *
 */
UCLASS()
class AGEOFWOLVES_API UBaseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
#pragma endregion

#pragma region Gameplay Tag Relationship Mapping
public:
	/*
	* @����: ���� ���� GA�� �����Ͽ� "Activation Required"/"Activation Blocked" Ability Tag�� �����մϴ�.
	*/
	void GetRelationshipActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	/*
	* @����: ���� ���� GA�� �����Ͽ� "Blocekd"/"Canceled" Ability Tag�� �����Ǵ� GA�鿡 ���� ������ ��ġ
	*/
	//~UAbilitySystemComponent Interface
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	//~End of Interface

protected:
	UPROPERTY(EditAnywhere)
		UAbilityTagRelationshipMapping* AbilityTagRelationship;
#pragma endregion

#pragma region Active GA
public:
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
#pragma endregion

};