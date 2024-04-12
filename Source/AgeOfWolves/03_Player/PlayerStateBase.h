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

#pragma region Default Setting
public:
	APlayerStateBase();

protected:
	//~APlayerState Interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	//~End Of APlayerState Interface

public:
	//~IAbilitySystemInterface Interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End Of IAbilitySystemInterface Interface
#pragma endregion

#pragma region Gameplay Ability System
protected:
	/*
	* @����: PawnData�� ���� ĳ������ ����(���� ���� ����) �⺻ GA, GE, AttributeSet�� ASC�� ����ϴ� �۾��� �����մϴ�.
	* @����: ��� ������ AttributeBase -> GE -> GA ������ �����մϴ�. AttributeBase�� �� Attribute�� �ʱ� ���� GE�� ���� �ʱ�ȭ�ϱ� �����Դϴ�!
	* @����: 
	*/
	void InitializeGameplayAbilitySystem();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability System|Pawn Data")
		TObjectPtr<UPawnData> PawnData;
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

	UPROPERTY()
		UBaseAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

public:
	UPawnData* GetPawnData() const;
	TSoftObjectPtr<UBaseAttributeSet> GetAttributeSet() const;
#pragma endregion

#pragma region Attribute Callbacks
protected:
	/*
	* @���� : ĳ������ Attribute ��ġ ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ�
	* @���� : Ability System Component���� �����ϴ� Attribute �׸��� ��ġ ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ��Դϴ�.
	*		  HUD ������ ���� PS���� �����ϴ� AttributeBase ���� �������̽��� Ȱ�� �����մϴ�(C++ȯ��).
	* @���� : APlayerStateBase::InitializeGameplayAbilitySystem()
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
