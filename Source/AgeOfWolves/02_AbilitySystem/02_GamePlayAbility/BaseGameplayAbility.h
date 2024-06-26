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
	* @설명: Active GA는 사용자 입력이 Trigger 매개이며, 사용자 입력이 Pressed 상태일 때 활성화 됩니다.
	*/
	OnInputTriggered,

	/*
	* @목적: Acitve GA 전용 활성화 정책
	* @설명: Active GA는 사용자 입력이 Trigger 매개이며, 사용자 입력이 Hold 상태일 때 활성화 됩니다.
	*/
	WhileInputActive,
	/*
	* @목적: Passive GA 전용 활성화 정책
	* @설명: Passive GA 중 특정 조건 없이 등록 시 한 번 활성화 될 경우 해당 활성화 정책을 선택하세요.
	*/
	OnGranted_Instant,

	/*
	* @목적: Passive GA 전용 활성화 정책
	* @설명: Passive GA 중 특정 조건이 만족되면 주기적으로 "GE"적용 수행할 경우 해당 활성화 정책을 선택하세요.
	*/
	OnGranted_ConditionalPeriodic,

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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of Interface
#pragma endregion

#pragma region Gameplay Tag Relationship Mapping
protected:
	//~UGameplayAbility interface, Overloading
	/*
	* @목적:해당 GA의 활성화 조건(Cost, CoolDown, ATRM)을 확인하고, 해당 GA의 활성화 가능 여부를 반환합니다.
	* @설명
	*	1. ActorInfo 체크
	*	2. Input Inhibition 체크(추후에 결정)
	*	3. CoolDown, Cost 체크
	*	4. AR(Activation Required), AB(Activation Blocked) 태그 관계성 만족 여부 확인
	*	5. 그 외 Null 체크
	* @참조:UBaseAbilitySystemComponent::TryActivateAbility
	*/
	bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, 
		OUT FGameplayTagContainer* ARTags= nullptr,
		OUT FGameplayTagContainer* ABTags = nullptr) const;
	/*
	* @목적: 해당 GA의 관계성을 통해 활성화 조건 만족 여부 확인
	* @설명
	*	1. BlockedAbilityTags에 해당 GA가 있다면, 활성화 할 수 없습니다.
	*	2. AR(Activation Required), AB(Activation Blocked) 관계성 확인
	* @참조: UBaseGameplayAbility::CanActivateAbility
	*/	
	bool DoesAbilitySatisfyTagRequirements(
		const UAbilitySystemComponent& AbilitySystemComponent, 
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
		FGameplayTagContainer* ARTags = nullptr, FGameplayTagContainer* ABTags = nullptr) const;
	//~End of Interface
#pragma endregion

#pragma region GA Info
protected:
	// @목적: Gameplay Ability의 발동 조건입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability | Ability Activation")
		EAbilityActivationPolicy ActivationPolicy;
	// @설명: 해당 Gameplay Ability의 활성화 과정에서 Target(GA의 적용 대상)에게 전달하는 Gameplay Effect입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability | Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplyGameplayEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay Ability | Gameplay Effect")
		FActiveGameplayEffectHandle ActiveApplyGameplayEffectHandle;
public:
	FORCEINLINE EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetApplyGameplayEffectClass() { return ApplyGameplayEffectClass; }

#pragma endregion

};
