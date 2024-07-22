// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"

#include "CombatComponent.generated.h"


class UBaseAbilitySystemComponent;
class UGameplayAbility;
struct FGameplayTagContainer;

/**
 * @���� : Player Character�� ������ ���õ� ���� ����� ����ϴ� Component�Դϴ�.
 * @���� : 1. Combo �ý���
		   2. Ability Queue �ý���
 * @���� : 
 */


UCLASS(Blueprintable)
class AGEOFWOLVES_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UBaseAbilitySystemComponent* BaseAbilitySystemComponent;

#pragma region Combo Management

public:	

	UCombatComponent();


	UFUNCTION(BlueprintCallable)
	int GetInputCount();

	UFUNCTION(BlueprintCallable)
	void IncrementCombo();



	UFUNCTION(BlueprintCallable)
	void ResetCombo();


	UFUNCTION(BlueprintCallable)
	void ActivateComboAbility(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable)
	bool CanComboAttack();

	UFUNCTION(BlueprintCallable)
	bool GetbComboWindowOpen();


	UFUNCTION(BlueprintCallable)
	UBaseAbilitySystemComponent* GetAbilitysystemComponent();

	UFUNCTION(BlueprintCallable)
	void OpenComboWindow();
	UFUNCTION(BlueprintCallable)
	void CloseComboWindow();



protected:
	// Called when the game starts
	virtual void BeginPlay() override;



	
private:

	int32 InputCount = 0;
	int32 ComboIndex = 0;
	bool bComboWindowOpen;


#pragma endregion

#pragma region Ability Queue Management

public:

	// DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityFailedDelegate, const UGameplayAbility*, const FGameplayTagContainer&);
	void QueueBlockedAbility(const UGameplayAbility* BlockedAbility, const FGameplayTagContainer& TagContainer);

	
	UFUNCTION(BlueprintCallable)
	void CheckQueuedAbility(UGameplayAbility* CurrentActivatedAbility);

	UPROPERTY(BlueprintReadOnly)
	const UGameplayAbility* QueuedAbility;

	FGameplayAbilitySpecHandle QueuedSpecHandles;



	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InputQueueMasterEvent();

	UFUNCTION(BlueprintCallable)
	void TryActivateQueuedAbility();

	TArray<FGameplayTag> AllowedInputTags;

	UFUNCTION(BlueprintCallable)
	void UpdateAllowedInputTags(TArray<FGameplayTag> InputTags);

	UFUNCTION(BlueprintCallable)
	void ResetAllowedInputTags();

#pragma endregion

	

};
