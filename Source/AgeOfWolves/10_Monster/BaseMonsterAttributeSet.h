// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseMonsterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseMonsterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UBaseMonsterAttributeSet();

public:

	void InitializeAttributeValue(struct FSingleMonsterData& SingleMonsterData);

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseMonsterAttributeSet, Health)

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseMonsterAttributeSet, MaxHealth)

		UPROPERTY(BlueprintReadOnly, Category = "Attribute | Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseMonsterAttributeSet, Damage)
	
	// @���� : BaseAttributeSet�� ��� Attribute �׸��� ��ȯ�ϴ� �Լ�
	TArray<FGameplayAttribute> GetAllAttributes() const;

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

	
};
