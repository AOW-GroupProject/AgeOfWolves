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
	
};
