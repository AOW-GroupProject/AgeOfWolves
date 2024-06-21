// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

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

		// @���ε�: '�ǰ�' �̺�Ʈ �߻� �� �ǰ� ���� ���θ� �����մϴ�.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Poise")
		FGameplayAttributeData Poise;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Poise)

		// @���ݷ�: �����ϰ��� �ϴ� �������� ���� ��ġ ������ŵ�ϴ�.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Offense")
		FGameplayAttributeData Offense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Offense)

		// @����: ���� ���� �������� ���� ��ġ ���ҽ�ŵ�ϴ�.
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Defense")
		FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Defense)

		// @Meta Attribute: ���������� ���� ������ ��ġ�� �ӽ� �����ϰ�, Health Attribute�� ������ �ִ� �뵵�Դϴ�.
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

		// @���� ���� ��ġ(��밡), ���� ����
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Seal Point")
		FGameplayAttributeData SealPoint;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, SealPoint)

		// @�׷α� ��ġ, ���� ����: ���� ���� �׷α� ��ġ�� ���̰�, ���� �̻� �����ϸ� �׷α� ���� ����
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Groggy")
		FGameplayAttributeData Groggy;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Groggy)

		// @óġ �� ��� ����ġ(��밡), ���� ����
		UPROPERTY(BlueprintReadOnly, Category = "Attribute | XP Bounty")
		FGameplayAttributeData XPBounty;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, XPBounty)

		// @óġ �� ��� ����(��밡), ���� ����
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
	* @���� : Attribute ��ġ ��ȭ �߻� �� ���Ѱ� ������ ������ Attribute �鿡 ���Ͽ� Ŭ������ �����մϴ�.
	*/
	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/*
	* @���� : Attribute�� ��ȭ ���Ŀ� ȣ��Ǵ� Handling Logic
	* @���� : Current Health�� Clmap �۾� �� Attribute ���� ��ȭ�� �Ͼ ���Ŀ� �߻��ϴ� �ڵ鸵 ������ ó���մϴ�.
	*/
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/*
	* @���� : Attirbute �׸��� Max �� ���濡 ���� Current/Max �ۼ�Ƽ�� �� ����
	* @���� : Attribute �׸��� Max �� ������ Current ���� Max ���� ���� ���濡 ������ ��ġ�Ƿ�, �� ������ ������ �Լ��� �����մϴ�.
	*/
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	/*
	* @���� : Attirbute �׸��� ���� �� ���濡 ���� ���� �� ����
	* @���� : Attribute �׸��� ���� �� ������ Current ���� Max ���� ���� ���濡 ������ ��ġ�Ƿ�, �� ������ ������ �Լ��� �����մϴ�.
	*/
	void AdjustAttributeForCurrentChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);



};
