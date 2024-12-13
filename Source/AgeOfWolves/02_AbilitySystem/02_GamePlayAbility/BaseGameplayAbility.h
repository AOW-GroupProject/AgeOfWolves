// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "BaseGameplayAbility.generated.h"

class UGameplayEffect;
class UAbilityTagRelationshipMapping;

DECLARE_LOG_CATEGORY_EXTERN(LogGA, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EAbilityActivationPolicy
* 
*	활성화 정책
*/
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
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseGameplayAbility : public UGameplayAbility
{

//@친추 클래스
#pragma region Friend Class
	friend class UBaseAbilitySystemComponent;
	friend class UPlayerAbilitySystemComponent;
	friend class UBaseMonsterASC;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~UGameplayAbility interface, ASC와 상호작용
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of Interface
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//~UGameplayAbility interface, Overloading
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//~End of Interface

protected:
	bool CheckGameplayEffectApplicationRequirements(const UGameplayEffect* GameplayEffect,
		const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const;

protected:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// Input Pressed 이벤트를 Blueprint에서 구현할 수 있도록 함
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
		void K2_InputPressed();
	virtual void K2_InputPressed_Implementation() { }

	// Input Released 이벤트를 Blueprint에서 구현할 수 있도록 함
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
		void K2_InputReleased();
	virtual void K2_InputReleased_Implementation() { }

protected:
	// @목적: Gameplay Ability의 발동 조건입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability | Ability Activation")
		EAbilityActivationPolicy ActivationPolicy;
	// @설명: 해당 Gameplay Ability의 활성화 과정에서 Target(GA의 적용 대상)에게 전달하는 Gameplay Effect입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability | Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplyGameplayEffectClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay Ability | Gameplay Effect")
		FActiveGameplayEffectHandle ActiveApplyGameplayEffectHandle;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	FORCEINLINE EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetApplyGameplayEffectClass() { return ApplyGameplayEffectClass; }
	FORCEINLINE FGameplayTagContainer GetRequiredTags() const { return ActivationRequiredTags; }
#pragma endregion

};
