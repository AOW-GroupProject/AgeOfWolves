// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "04_Component/BaseAbilitySystemComponent.h"

#include "BaseAttributeSet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAttributeSet, Log, All);


/*
* @���� : Gameplay Attribute ������ Getter&Setter�� ���� ��ũ�� ����
* @���� : Get(Attribute_Name)�� ���� ������ Attirbute ��ġ�� ������ �� �ֵ����մϴ�.
* @���� : ���������� Getter�� Setter�� ȣ���ϱ� ����, Gameplay Effect Ȱ���� �����մϴ�.
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

		// ���� ����...
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

	// @���� : BaseAttributeSet�� ��� Attribute �׸��� ��ȯ�ϴ� �Լ�
	TArray<FGameplayAttribute> GetAllAttributes() const;

public:
	UBaseAttributeSet();

protected:
	/*
	* @���� : Attribute ��ġ ��ȭ �̺�Ʈ �߻� �� �׻� ȣ��Ǵ� �Լ�
	* @���� : Health, Mana, Stamina �� Max �� ������ ������ Attribute ��ġ�鿡 ���� ���� �� ������ ���
	*/
	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/*
	* @���� : Attribute ��ġ ��ȭ �̺�Ʈ �߻� �Ŀ� �׻� ȣ��Ǵ� �Լ�
	* @���� : Attribute ��ġ�� ��ȭ�Ŀ� ȣ��ȴ�. 
	*/
	// AttributeSet Overrides
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


	/*
	* @���� : Attribute�� ��ȭ ���Ŀ� ȣ��Ǵ� Handling Logic
	* @���� : Current Health�� Clmap �۾� �� Attribute ���� ��ȭ�� �Ͼ ���Ŀ� �߻��ϴ� �ڵ鸵 ������ ó���մϴ�.
	*/
	// AttributeSet Overrides
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/*
	* @���� : Attirbute �׸��� Max �� ���濡 ���� Current/Max �ۼ�Ƽ�� �� ����
	* @���� : Attribute �׸��� Max �� ������ Current ���� Max ���� ���� ���濡 ������ ��ġ�Ƿ�, �� ������ ������ �Լ��� �����մϴ�.
	*/
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);


	/*
	* @���� : Attribute�� Clamp�ϴ� �Լ�
	* @���� : PreAttributeChange �Լ����� Clamp �۾��� ����Ѵ�.
	*/
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;




	/*
	* @���� : AttributeSet Ŭ���� ������ Base Ability System Component�� ��� Getter�Լ�
	* @���� : GetOwningAbilitySystemComponent �Լ��� ��ȯ���� UBaseAbilitysystemComponent�� ĳ���� �Ŀ� ��ȯ�մϴ�.
	*/
	UBaseAbilitySystemComponent* GetBaseAbilitySystemComponent() const;

};
