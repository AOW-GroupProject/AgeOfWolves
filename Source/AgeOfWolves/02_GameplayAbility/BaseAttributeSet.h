// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "BaseAttributeSet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAttributeSet, Log, All);


/*
* @목적 : Gameplay Attribute 값들의 Getter&Setter를 위한 매크로 정의
* @설명 : Get(Attribute_Name)을 통해 간단히 Attirbute 수치를 가져올 수 있도록합니다.
* @주의 : 직접적으로 Getter와 Setter를 호출하기 보단, Gameplay Effect 활용을 지향합니다.
*/
// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 *
 */
UCLASS()
class AGEOFWOLVES_API UBaseAttributeSet: public UAttributeSet
{
	GENERATED_BODY()


public:
	// Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth.
	// Positive changes can directly use this.
	// Negative changes to Health should go through Damage meta attribute.
	UPROPERTY(BlueprintReadOnly, Category = "Attribute | Health")
		FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)

		// MaxHealth is its own attribute since GameplayEffects may modify it
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Health")
		FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

		// Health regen rate will passively increase Health every second
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Health")
		FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, HealthRegenRate)

		// Current Mana, used to execute special abilities. Capped by MaxMana.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Mana")
		FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Mana)

		// MaxMana is its own attribute since GameplayEffects may modify it
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Mana")
		FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxMana)

		// Mana regen rate will passively increase Mana every second
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Mana")
		FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, ManaRegenRate)

		// Current stamina, used to execute special abilities. Capped by MaxStamina.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Stamina")
		FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Stamina)

		// MaxStamina is its own attribute since GameplayEffects may modify it
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Stamina")
		FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxStamina)

		// Stamina regen rate will passively increase Stamina every second
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Stamina")
		FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, StaminaRegenRate)

		// Armor reduces the amount of damage done by attackers
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Armor")
		FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Armor)

		// MoveSpeed affects how fast characters can move.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | MoveSpeed")
		FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MoveSpeed)

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Character Level")
		FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CharacterLevel)

		// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
		// Temporary value that only exists on the Server. Not replicated.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Damage")
		FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)

		// Experience points gained from killing enemies. Used to level up (not implemented in this project).
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | XP")
		FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, XP)

		// 골드는 아직...
			UPROPERTY(BlueprintReadOnly, Category = "Attribute | Gold")
		FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Gold)

		// Bounty in Experience points gained from killing this character
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | XP Bounty")
		FGameplayAttributeData XPBounty;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, XPBounty)

		// Bounty in Gold when killing this character
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Gold Bounty")
		FGameplayAttributeData GoldBounty;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, GoldBounty)

	// @목적 : BaseAttributeSet의 모든 Attribute 항목을 반환하는 함수
	TArray<FGameplayAttribute> GetAllAttributes() const;

public:
	UBaseAttributeSet();

protected:
	/*
	* @목적 : Attribute 수치 변화 이벤트 발생 시 항상 호출되는 함수
	* @설명 : Health, Mana, Stamina 등 Max 값 변경이 가능한 Attribute 수치들에 대한 현재 값 조정에 사용
	*/
	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/*
	* @목적 : Attribute의 변화 이후에 호출되는 Handling Logic
	* @설명 : Current Health의 Clmap 작업 등 Attribute 값의 변화가 일어난 이후에 발생하는 핸들링 로직을 처리합니다.
	*/
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/*
	* @목적 : Attirbute 항목의 Max 값 변경에 따른 Current/Max 퍼센티지 값 변경
	* @설명 : Attribute 항목의 Max 값 변경은 Current 값과 Max 값의 비율 변경에 영향을 끼치므로, 이 내용을 별도의 함수로 정의합니다.
	*/
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);


};
