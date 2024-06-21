// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class UBaseAbilitySystemComponent;

/**
 * @���� : Player Character�� ������ ���õ� ���� ����� ����ϴ� Component�Դϴ�.
 * @���� : 1. Combo �ý���
		   2. LockOn �ý���
		   3. Input Queue �ý���
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
	// Sets default values for this component's properties
	UCombatComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


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

#pragma region Input Queue Management

public:

	TSubclassOf<UGameplayAbility> QueuedInput;


	UFUNCTION(BlueprintCallable)
	void InputQueueMasterEvent();



	TArray<FGameplayTag> AllowedInputTags;

	UFUNCTION(BlueprintCallable)
	void UpdateAllowedInputTags(TArray<FGameplayTag> InputTags);

	UFUNCTION(BlueprintCallable)
	void ResetAllowedInputTags();


	



#pragma endregion

#pragma region LockOn System

#pragma endregion
public :
	

};
