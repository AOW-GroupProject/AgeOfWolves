// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "SpellGameplayAbility.generated.h"


#pragma region Forward Declaration
class UAnimMontage;
#pragma endregion


#pragma region Enums
#pragma endregion

#pragma region Structs
#pragma endregion

#pragma region Delegates
#pragma endregion


/**
 *  주술 기본 Ability
 */
UCLASS()
class AGEOFWOLVES_API USpellGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
#pragma region Friend Class
#pragma endregion

#pragma region Default Setting
public:
	USpellGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
#pragma endregion

#pragma region Property or Subwidgets or Infos...etc

protected:
	/** 스킬 사용 해당 몽타주 실행 */
	UPROPERTY(EditDefaultsOnly, Category = "Spell|Montage")
	TObjectPtr<UAnimMontage> ActivationSpellActionMontage;
	
	/** 몽타주 속도 조정 변수 */
	UPROPERTY(EditDefaultsOnly, Category = "Spell|Montage")
	float MontageRate;

	/** 이동을 제한하는 변수 */
	UPROPERTY(EditDefaultsOnly, Category = "Spell|Movement")
	bool bMoveNone;

	/** 애니메이션이 끝날 때 까지 진행하는 변수 */
	UPROPERTY(EditDefaultsOnly, Category = "Spell|Movement")
	bool bPlayMontageAndWait;

#pragma endregion
	
#pragma region Delegates
#pragma endregion


#pragma region Callbacks

protected:
	UFUNCTION()
	void OnCompleteCallback();

	UFUNCTION()
	void OnInterruptedCallback();

#pragma endregion


#pragma region Utility
private:
	void CreateTask_PlayMontageAndWait();
#pragma endregion

};
