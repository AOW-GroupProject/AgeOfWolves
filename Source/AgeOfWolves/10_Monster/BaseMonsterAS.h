// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "BaseMonsterAS.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseMonsterAS : public UBaseAttributeSet
{
	GENERATED_BODY()

public:
	UBaseMonsterAS();

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
	
};
