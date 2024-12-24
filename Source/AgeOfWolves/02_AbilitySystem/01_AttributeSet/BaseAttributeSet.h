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
 *	@UBaseAttributeSe
 * 
 *	ASC를 활용하는 캐릭터의 능력치 속성 목록 정의
 */
UCLASS()
class AGEOFWOLVES_API UBaseAttributeSet: public UAttributeSet
{
	GENERATED_BODY()


public:
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

		// @강인도: '피격' 이벤트 발생 시 피격 반응 여부를 결정합니다.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Poise")
		FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Poise)

		// @공격력: 전달하고자 하는 데미지를 일정 수치 증가시킵니다.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Offense")
		FGameplayAttributeData Offense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Offense)

		// @방어력: 전달 받은 데미지를 일정 수치 감소시킵니다.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Defense")
		FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Defense)

		// @Meta Attribute: 최종적으로 계산된 데미지 수치를 임시 저장하고, Health Attribute에 영향을 주는 용도입니다.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Damage")
		FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)

		// MoveSpeed affects how fast characters can move.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | MoveSpeed")
		FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MoveSpeed)

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Character Level")
		FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CharacterLevel)

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | XP")
		FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, XP)

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Gold")
		FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Gold)

		// @봉인 가능 수치(상대가), 몬스터 전용
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Seal Point")
		FGameplayAttributeData SealPoint;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, SealPoint)

		// @그로기 수치, 몬스터 전용: 전달 받은 그로기 수치가 쌓이고, 일정 이상 증가하면 그로기 반응 수행
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Groggy")
		FGameplayAttributeData Groggy;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Groggy)

		// @처치 시 얻는 경험치(상대가), 몬스터 전용
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | XP Bounty")
		FGameplayAttributeData XPBounty;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, XPBounty)

		// @처치 시 얻는 금전(상대가), 몬스터 전용
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Gold Bounty")
		FGameplayAttributeData GoldBounty;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, GoldBounty)

		// @전투/비전투 상태 정의
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Combat State")
		FGameplayAttributeData CombatState;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CombatState)

public:
	UBaseAttributeSet();


protected:
	/*
	* @목적 : Attribute 수치 변화 이벤트 발생 시 항상 호출되는 함수
	* @설명 : Attribute 수치 변화 발생 시 하한과 상한이 고정된 Attribute 들에 대하여 클램핑을 수행합니다.
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

	/*
	* @목적 : Attirbute 항목의 현재 값 변경에 따른 현재 값 조정
	* @설명 : Attribute 항목의 현재 값 변경은 Current 값과 Max 값의 비율 변경에 영향을 끼치므로, 이 내용을 별도의 함수로 정의합니다.
	*/
	void AdjustAttributeForCurrentChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

public:
	// @목적 : BaseAttributeSet의 모든 Attribute 항목을 반환하는 함수
	TArray<FGameplayAttribute> GetAllAttributes() const;

public:
	// 블루프린트에서 CombatState 값을 쉽게 가져올 수 있는 함수
	UFUNCTION(BlueprintPure, Category = "Attributes")
		float GetCurrentCombatState() const { return GetCombatState(); }

};
