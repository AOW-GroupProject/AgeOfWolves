// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
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
class AGEOFWOLVES_API UBaseMonsterAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

	UBaseMonsterAttributeSet();

public:

	void InitializeAttributeValue(struct FSingleMonsterData& SingleMonsterData);

	
	// @목적 : BaseAttributeSet의 모든 Attribute 항목을 반환하는 함수
	TArray<FGameplayAttribute> GetAllAttributes() const;

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

	
};
