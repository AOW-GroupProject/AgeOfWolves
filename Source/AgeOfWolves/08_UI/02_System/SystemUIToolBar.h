// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SystemUIToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogToolBar, Log, All)

//#�ʱ�ȭ �Ϸ� �̺�Ʈ
DECLARE_DELEGATE(FToolBarInitFinished);

/**
 * USystemUIToolBar
 * 
 * System UI �ֻ�ܿ� ��ġ�ϴ� �� �ٸ� �����մϴ�.
 */
UCLASS()
class AGEOFWOLVES_API USystemUIToolBar : public UUserWidget
{
	GENERATED_BODY()

#pragma region Default Setting
public:
    USystemUIToolBar(const FObjectInitializer& ObjectInitializer);

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
