// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"


#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "BaseMonsterASC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMonsterASC, Log, All);
/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseMonsterASC : public UAbilitySystemComponent
{
	GENERATED_BODY()
#pragma region Default Setting

public:
	UBaseMonsterASC(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;

protected:
	//~UAbilitySystemComponent Interfaces
	bool TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey = FPredictionKey(), UGameplayAbility** OutInstancedAbility = nullptr, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate = nullptr, const FGameplayEventData* TriggerEventData = nullptr);
	virtual void CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore) override;
	//~End Of Interface
	/*
	* @����: Unblock�� Passive GA�� �� Ȱ��ȭ ���� ����.
	* @����: UBaseAbilitySystemComponent::OnAbilityEnded
	*/
	void ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags);
#pragma endregion

#pragma region GA Life Span
protected:
	/*
	* @����: ���� Ȱ��ȭ �̺�Ʈ ó�� ���� GA���� Gameplay Tag���� ��ƵӴϴ�.
	* @����: OnAbilityActivated �ݹ� �Լ����� Gameplay Tag�� �߰��ϰ�, OnAbilityEnded Ȥ�� OnAbilityCanceled���� Ư�� Gameplay Tag�� �������ݴϴ�.
	*/
	FGameplayTagContainer ActivatingAbilityTags;
public:
	FORCEINLINE void GetActivatingAbilityTags(OUT FGameplayTagContainer& OutGameplayTagContainer) const { OutGameplayTagContainer = ActivatingAbilityTags; }

protected:
	/*
	* @����: Ability�� Ȱ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ�
	* @����
	*	1. "AbilityTagsToBlock"�� "AbilityTagsToCancel"��Ͽ� �ش�Ǵ� GA�� "Block"�� "Cancel"���� ����
	*	2. ActivatingAbilityTags ��Ͽ� �ش� GA�� �߰��մϴ�.
	*/
	void OnAbilityActivated(UGameplayAbility* Ability);
	/*
	* @����: Ability�� Ȱ��ȭ ���� �̺�Ʈ�� ����� �ݹ� �Լ�
	* @����
	*	1. ActivatingAbilityTags ��Ͽ��� �ش� GA�� �����մϴ�.
	*	2. Unblock ����
	*	3. Unblock�� GA �� ���������� Passive GA�� ���Ͽ� �� Ȱ��ȭ ���� ����
	*/
	void OnAbilityEnded(UGameplayAbility* Ability);

#pragma endregion


#pragma region Active GA
public:
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

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
	void GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const;
	/*
	* @����: ���� ���� GA�� �����Ͽ� "Blocekd"/"Canceled" Ability Tag�� �����Ǵ� GA�鿡 ���� ������ ��ġ
	*/
	//~UAbilitySystemComponent Interface
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	//~End of Interface

protected:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;

#pragma endregion

#pragma region Gameplay Ability System
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilityTagRelationshipMapping> TagRelationship;

	

public:
	UAbilityTagRelationshipMapping* GetTagRelationship();

#pragma endregion

public:
	FORCEINLINE void SetAbilityTagRelationshipMapping(UAbilityTagRelationshipMapping* ATRM) { AbilityTagRelationshipMapping = ATRM; }
#pragma endregion

	
};