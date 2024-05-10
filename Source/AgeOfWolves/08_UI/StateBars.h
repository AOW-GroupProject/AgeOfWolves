// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"

#include "StateBars.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStateBars, Log, All)

class UProgressBar;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UStateBars : public UUserWidget
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	UStateBars(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // �޸� �Ҵ� �Ϸ�, ȭ�鿡 �����Ǳ� ���� ȣ���
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // ȭ�鿡 �����Ǳ� ������ ȣ���
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
#pragma endregion 

#pragma region SubWidgets
protected:
	TMap<FString, UProgressBar*> MStateBars;

	UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
		UVerticalBox* StateBarList;

	UPROPERTY(BlueprintReadWrite, Category="State Bar", meta=(BindWidget))
		UProgressBar* HP;
	UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
		UProgressBar* MP;
	UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
		UProgressBar* SP;

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
