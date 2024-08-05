#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SystemUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSystemUI, Log, All)

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitBySystemUI);
//@Inventory UI 초기화 완료 알림 이벤트
DECLARE_DELEGATE(FNotifyInventoryUIInitFinished);
//@Tool Bar 초기화 완료 알림 이벤트
DECLARE_DELEGATE(FNotifyToolBarInitFinished);

class UScaleBox;
class UImage;
class UTextBlock;
class UOverlay;

class UInventoryUI;
class USystemUIToolBar;

/**
 * USystemUI
 *
 * System 관련 UI들의 프레임을 정의합니다.
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
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface
protected:
    void InternalBindingToToolBar(USystemUIToolBar* ToolBar);
    void InternalBindingToInventoryUI(UInventoryUI* InventoryUI);
public:
    //@초기화
    UFUNCTION()
        void InitializeSystemUI();
#pragma endregion

#pragma region SubWidgets
protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* SystemUIOverlay;

    UPROPERTY(BlueprintReadWrite, Category = "System", meta = (BindWidget))
        UImage* SystemUI_Outer_BG;
    UPROPERTY(BlueprintReadWrite, Category = "System", meta = (BindWidget))
        UImage* SystemUI_Inner_BG;
    const float Inner_BG_Padding = 10.f;

    UPROPERTY(BlueprintReadWrite, Category = "System", meta = (BindWidget))
        UScaleBox* ToolBarBox;
    UPROPERTY(EditDefaultsOnly, category = "System")
        TSubclassOf<USystemUIToolBar> ToolBarClass;

    UPROPERTY(BlueprintReadWrite, Category = "System | Inventory UI", meta = (BindWidget))
        UScaleBox* InventoryUIBox;
    UPROPERTY(EditDefaultsOnly, category = "System | Inventory UI")
        TSubclassOf<UInventoryUI> InventoryUIClass;

protected:
    void CreateBG();
    //@Tool Bar 생성
    void CreateToolBar();
    //@Inventory UI를 생성합니다.
    void CreateInventoryUI();

public:
    UFUNCTION(BlueprintCallable, Category = "System | Inventory UI")
        UInventoryUI* GetInventoryUI() const;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitBySystemUI RequestStartInitBySystemUI;
public:
    //@초기화 완료
    FNotifyInventoryUIInitFinished NotifyInventoryUIInitFinished;
    FNotifyToolBarInitFinished NotifyToolBarInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION()
        void OnInventoryUIInitFinished();
    UFUNCTION()
        void OnToolBarInitFinished();
#pragma endregion
};