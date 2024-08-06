// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MenuUIToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogToolBar, Log, All)

//#초기화 완료 이벤트
DECLARE_DELEGATE(FToolBarInitFinished);

/**
 * UMenuUIToolBar
 *
 * Menu UI 최상단에 위치하는 툴 바를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUIToolBar : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UMenuUIToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
public:
    UFUNCTION()
        void InitializeToolBar();
#pragma endregion

#pragma region Delegates
public:
    FToolBarInitFinished ToolBarInitFinished;
#pragma endregion

};
