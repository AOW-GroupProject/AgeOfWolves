// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SystemUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSystemUI, Log, All)

DECLARE_DELEGATE(FSystemUIInitFinished);

class UOverlay;
class UScaleBox;
class UImage;

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
public:
	//@�ʱ�ȭ
	UFUNCTION()
		void InitializeSystemUI();
#pragma endregion

#pragma region SubWidgets
protected:
	//@System UI Overlay
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* SystemUIOverlay;

	//@Title Image�� ���� Scale Box
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UScaleBox* TitleImageBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* TitleImage;
#pragma endregion

#pragma region Delegates
public:
	FSystemUIInitFinished SystemUIInitFinished;
#pragma endregion
};
