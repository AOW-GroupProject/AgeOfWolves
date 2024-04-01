// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "GameplayEffectTypes.h"
#include "02_GameplayAbility/BaseAbilitySet.h"
#include "02_GameplayAbility/BaseAttributeSet.h"

#include "PlayerStateBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All)

class UPawnData;
class UBaseAbilitySystemComponent;

/**
 * Player State contaions pawn's info interacting with others
 */
UCLASS()
class AGEOFWOLVES_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region Default Setup

public:
	APlayerStateBase();

protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// @설명 : 게임 시작 시점에 주어진 Pawn Data(Data Asset)을 통해 Avatar 캐릭터의 Gameplay Ability System에 Startup AttributeSet, GA, 그리고 GE를 등록합니다.
	void InitializeGameplayAbilitySystem();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability System|Pawn Data")
		TObjectPtr<UPawnData> PawnData;

public:
	UPawnData* GetPawnData() const;

#pragma endregion

#pragma region Gameplay Ability System

protected:
	UPROPERTY()
		UBaseAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
		TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

public:
	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TSoftObjectPtr<UBaseAttributeSet> GetAttributeSet() const;


#pragma endregion

#pragma region Attribute Delegates

protected:
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

	/*
	* [목적] : 캐릭터의 Attribute 수치 변화 이벤트에 등록할 콜백 함수
	* [설명] : Ability System Component에서 관리하는 Attribute 항목의 수치 변화 이벤트에 등록할 콜백 함수입니다.
	*/
	void OnAnyAttributeChanged(const FOnAttributeChangeData& Data);


#pragma endregion

#pragma region Getter&Setter

public:

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetHealth() const { return AttributeSet.Get()->GetHealth(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetMaxHealth() const { return AttributeSet.Get()->GetMaxHealth(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetHealthRegenRate() const { return AttributeSet.Get()->GetHealthRegenRate(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE bool IsAlive() const { return GetHealth() > 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetMana() const { return AttributeSet.Get()->GetMana(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetMaxMana() const { return AttributeSet.Get()->GetMana(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetManaRegenRate() const { return AttributeSet.Get()->GetManaRegenRate(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetStamina() const { return AttributeSet.Get()->GetStamina(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetMaxStamina() const { return AttributeSet.Get()->GetMaxStamina(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetStaminaRegenRate() const { return AttributeSet.Get()->GetStaminaRegenRate(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetArmor() const { return AttributeSet.Get()->GetArmor(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE float GetMoveSpeed() const { return AttributeSet.Get()->GetMoveSpeed(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE int32 GetCharacterLevel() const { return AttributeSet.Get()->GetCharacterLevel(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE int32 GetXP() const { return AttributeSet.Get()->GetXP(); }

	UFUNCTION(BlueprintCallable, Category = "Player State | Attributes")
		FORCEINLINE int32 GetGold() const { return AttributeSet.Get()->GetGold(); }

#pragma endregion
	
};
