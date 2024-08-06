// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SystemUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSystemUI, Log, All)

DECLARE_DELEGATE(FSystemUIInitFinished);

/**
 * @USystemUI
 * 
 * System ���� ������ �����ִ� UI, Menu UI�� �ڽ� �����Դϴ�.
 */
UCLASS()
class AGEOFWOLVES_API USystemUI : public UUserWidget
{
	GENERATED_BODY()
#pragma region Default Setting
public:
	USystemUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // �޸� �Ҵ� �Ϸ�, ȭ�鿡 �����Ǳ� ���� ȣ���
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // ȭ�鿡 �����Ǳ� ������ ȣ���
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
protected:
	//@�ܺ� ���ε�
	void ExternalBindingToInventoryComp();
	void ExternalBindingToInputComp();
public:
	//@�ʱ�ȭ
	UFUNCTION()
		void InitializeSystemUI();
#pragma endregion

#pragma region Delegates
public:
	FSystemUIInitFinished SystemUIInitFinished;
#pragma endregion
};
