// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI.h"

#include "MenuUIToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogToolBar, Log, All)

//@초기화 완료 이벤트
DECLARE_DELEGATE(FToolBarInitFinished);

//@Menu Category 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FMenuCategoryButtonClikced, EMenuCategory);
//@닫기 버튼 선택 이벤트
DECLARE_DELEGATE(FCloseButtonClicked);

class UHorizontalBox;
class UButton;
class UImage;

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

#pragma region Widgets
protected:
    void CreateButtons();

protected:
    //@Tool Bar의 Horizontal Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* CategoryButtonsBox;

protected:
    //@현재 선택된 카테고리
    EMenuCategory CurrentCategory = EMenuCategory::Inventory;
    //@Menu Category 버튼들을 담고 있는 배열
    TMap<UButton*, EMenuCategory> MCategoryButtons;
    //@Inventory UI 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* InventoryUIButton;
    //@Level UI 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* LevelUIButton;
    //@Map UI 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* MapUIButton;
    //@System UI 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* SystemUIButton;

private:
    //@현재 선택된 버튼의 스타일을 업데이트합니다.
    void UpdateButtonStyle(UButton* SelectedButton, UButton* PreviousButton = nullptr);

private:
    void HandleButtonClick(EMenuCategory Category);
    void HandleButtonHover(EMenuCategory Category);
    void HandleButtonUnhover(EMenuCategory Category);

#pragma endregion

#pragma region Delegates
public:
    //@Tool Bar의 초기화 완료 이벤트
    FToolBarInitFinished ToolBarInitFinished;
    //@Menu Category Button의 선택 이벤트
    FMenuCategoryButtonClikced MenuCategoryButtonClikced;
#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION()
        FORCEINLINE void OnInventoryButtonClicked() { HandleButtonClick(EMenuCategory::Inventory); }
    UFUNCTION()
        FORCEINLINE void OnLevelButtonClicked() { HandleButtonClick(EMenuCategory::Level); }
    UFUNCTION()
        FORCEINLINE void OnMapButtonClicked() { HandleButtonClick(EMenuCategory::Map); }
    UFUNCTION()
        FORCEINLINE void OnSystemButtonClicked() { HandleButtonClick(EMenuCategory::System); }

    UFUNCTION()
        FORCEINLINE void OnInventoryButtonHovered() { HandleButtonHover(EMenuCategory::Inventory); }
    UFUNCTION()
        FORCEINLINE void OnLevelButtonHovered() { HandleButtonHover(EMenuCategory::Level); }
    UFUNCTION()
        FORCEINLINE void OnMapButtonHovered() { HandleButtonHover(EMenuCategory::Map); }
    UFUNCTION()
        FORCEINLINE void OnSystemButtonHovered() { HandleButtonHover(EMenuCategory::System); }

    UFUNCTION()
        FORCEINLINE void OnInventoryButtonUnhovered() { HandleButtonUnhover(EMenuCategory::Inventory); }
    UFUNCTION()
        FORCEINLINE void OnLevelButtonUnhovered() { HandleButtonUnhover(EMenuCategory::Level); }
    UFUNCTION()
        FORCEINLINE void OnMapButtonUnhovered() { HandleButtonUnhover(EMenuCategory::Map); }
    UFUNCTION()
        FORCEINLINE void OnSystemButtonUnhovered() { HandleButtonUnhover(EMenuCategory::System); }
#pragma endregion

};
 