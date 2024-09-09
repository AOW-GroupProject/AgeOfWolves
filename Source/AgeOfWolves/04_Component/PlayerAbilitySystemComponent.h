// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "PlayerAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UPlayerAbilitySystemComponent : public UBaseAbilitySystemComponent
{
	GENERATED_BODY()

public:

	// @목적 : 현재 실행중인 몽타주 및 어빌리티를 조기 종료하는 함수
	UFUNCTION(BlueprintCallable)
	void EarlyCancelAbility();

	// @목적 : bCanSaveBlockedAbility 멤버 변수 Setter
	UFUNCTION(BlueprintCallable)
	void SetbCanSaveBlockedAbility(bool bCanSave);
	// @목적 : bCanSaveBlockedAbility 멤버 변수 Getter
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbCanSaveBlockedAbility(){ return bCanSaveBlockedAbility; }

protected:

	virtual void InitializeComponent() override;
	virtual void OnAbilityEnded(UGameplayAbility* Ability) override;

private:

	bool bCanSaveBlockedAbility = false;

	// @목적 : 선입력 된 어빌리티를 저장하는 변수
	const UGameplayAbility* SavedAbility = nullptr;
	// @목적 : 선입력 된 어빌리티를 저장하는 함수
	void SaveBlockedAbility(const UGameplayAbility* BlockedAbility, const FGameplayTagContainer& TagContainer);
};
