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

	friend class UCombatComponent;

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
	* @목적: Unblock된 Passive GA의 재 활성화 동작 수행.
	* @참조: UBaseAbilitySystemComponent::OnAbilityEnded
	*/
	void ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags);
#pragma endregion

#pragma region GA Life Span
protected:
	/*
	* @목적: 현재 활성화 이벤트 처리 중인 GA들의 Gameplay Tag들을 담아둡니다.
	* @설명: OnAbilityActivated 콜백 함수에서 Gameplay Tag를 추가하고, OnAbilityEnded 혹은 OnAbilityCanceled에서 특정 Gameplay Tag를 삭제해줍니다.
	*/
	FGameplayTagContainer ActivatingAbilityTags;
public:
	FORCEINLINE void GetActivatingAbilityTags(OUT FGameplayTagContainer& OutGameplayTagContainer) const { OutGameplayTagContainer = ActivatingAbilityTags; }

protected:
	/*
	* @목적: Ability의 활성화 이벤트에 등록할 콜백 함수
	* @설명
	*	1. "AbilityTagsToBlock"과 "AbilityTagsToCancel"목록에 해당되는 GA의 "Block"과 "Cancel"동작 수행
	*	2. ActivatingAbilityTags 목록에 해당 GA를 추가합니다. 
	*/
	void OnAbilityActivated(UGameplayAbility* Ability);
	/*
	* @목적: Ability의 활성화 종료 이벤트에 등록할 콜백 함수
	* @설명
	*	1. ActivatingAbilityTags 목록에서 해당 GA를 제거합니다.
	*	2. Unblock 수행
	*	3. Unblock된 GA 중 선택적으로 Passive GA에 대하여 재 활성화 동작 수행
	*/
	void OnAbilityEnded(UGameplayAbility* Ability);

#pragma endregion


#pragma region Active GA
public:
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	void AbilityInputTagTriggered(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:
	TArray<FGameplayAbilitySpecHandle> InputTriggeredSpecHandles;

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

#pragma endregion

#pragma region Gameplay Tag Relationship Mapping
public:
	/*
	* @목적: 전달 받은 GA와 관련하여 "Ability Tags To Block"/"Ability Tags To Cancel" Ability Tag를 전달합니다.
	*/
	void GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel);
	/*
	* @목적: 전달 받은 GA와 관련하여 "Activation Required"/"Activation Blocked" Ability Tag를 전달합니다.
	*/
	void GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const;
	/*
	* @목적: 전달 받은 GA와 관련하여 "Blocekd"/"Canceled" Ability Tag에 대응되는 GA들에 대한 각각의 조치
	*/
	//~UAbilitySystemComponent Interface
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	//~End of Interface

protected:
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;
public:
	FORCEINLINE void SetAbilityTagRelationshipMapping(UAbilityTagRelationshipMapping* ATRM) { AbilityTagRelationshipMapping = ATRM;}
#pragma endregion

};