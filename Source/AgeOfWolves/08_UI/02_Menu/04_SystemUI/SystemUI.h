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
 * System 관련 정보를 보여주는 UI, Menu UI의 자식 위젯입니다.
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
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
public:
	//@초기화
	UFUNCTION()
		void InitializeSystemUI();
#pragma endregion

#pragma region SubWidgets
protected:
	//@System UI Overlay
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* SystemUIOverlay;

	//@Title Image를 담을 Scale Box
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
