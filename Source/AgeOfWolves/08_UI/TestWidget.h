// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"

#include "TestWidget.generated.h"

class APlayerStateBase;

/**
 * @���� : ĳ������ Attribute ���� �������̽� ������ �׽�Ʈ�� ���� �ӽ÷� ������ UI Ŭ����
 * @���� : ĳ������ ASC�� Attribute �� ��ȭ �̺�Ʈ�� UI Ŭ������ �ݹ� �Լ��� ����Ͽ�, Ư�� Attribute ���� �ǽð����� �����ɴϴ�.
 * @���� : -
 */
UCLASS()
class AGEOFWOLVES_API UTestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTestWidget(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	//~ End UUserWidget Interface

#pragma region Attribute Value
protected:
	/*
	* @���� : OwningPawn�� PlayerState�� Attribute �� ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ��Դϴ�.
	* @���� : �ش� �ݹ� �Լ��� ���� HP, SP, MP ���� ĳ������ �ǽð� Attribute ������ ������ UI�� �ݿ��� �� �ֽ��ϴ�.
	*/
	UFUNCTION()
		void OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion 

};
