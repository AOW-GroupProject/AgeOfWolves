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

#pragma region Friend Class

#pragma endregion

#pragma region Default Setting

public:
	UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;

protected:
	//~UAbilitySystemComponent Interfaces
	bool TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey = FPredictionKey(), UGameplayAbility** OutInstancedAbility = nullptr, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate = nullptr, const FGameplayEventData* TriggerEventData = nullptr);
	virtual void CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore) override;
	//~End Of Interface
	/*
	* @����: Ability�� Ȱ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ�
	* @����
	*	1. Abilit Tag Realtionship Mapping�� ���� B, C, AR, AB ���� ���� ����
	*/
	void OnAbilityActivated(UGameplayAbility* Ability);
	/*
	* @����: Ability�� Ȱ��ȭ ���� �̺�Ʈ�� ����� �ݹ� �Լ�
	* @����
	*	1. Ability Tag Relationship Mapping�� ���� B, C, AR, AB ���� ����� �ʱ�ȭ
	*/
	void OnAbilityEnded(UGameplayAbility* Ability);

#pragma endregion

#pragma region Gameplay Tag Relationship Mapping
public:
	/*
	* @����: ���� ���� GA�� �����Ͽ� "Ability Tags To Block"/"Ability Tags To Cancel" Ability Tag�� �����մϴ�.
	*/
	void GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel);
	/*
	* @����: ���� ���� GA�� �����Ͽ� "Activation Required"/"Activation Blocked" Ability Tag�� �����մϴ�.
	*/
	void GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutActivationRequired, OUT FGameplayTagContainer& OutActivationBlocked) const;

	/*
	* @����: ���� ���� GA�� �����Ͽ� "Blocekd"/"Canceled" Ability Tag�� �����Ǵ� GA�鿡 ���� ������ ��ġ
	*/
	//~UAbilitySystemComponent Interface
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	//~End of Interface

protected:
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;

	/*
	* @����: ���� Ȱ��ȭ �̺�Ʈ ó�� ���� GA���� Gameplay Tag���� ��ƵӴϴ�.
	* @����: OnAbilityActivated �ݹ� �Լ����� Gameplay Tag�� �߰��ϰ�, OnAbilityEnded Ȥ�� OnAbilityCanceled���� Ư�� Gameplay Tag�� �������ݴϴ�.
	*/
	FGameplayTagContainer ActivatingAbilityTags;

public:
	FORCEINLINE void SetAbilityTagRelationshipMapping(UAbilityTagRelationshipMapping* ATRM) { AbilityTagRelationshipMapping = ATRM;}
	FORCEINLINE void GetActivatingAbilityTags(OUT FGameplayTagContainer& OutGameplayTagContainer) const { OutGameplayTagContainer = ActivatingAbilityTags; }
#pragma endregion

#pragma region Active GA
public:
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
#pragma endregion

};