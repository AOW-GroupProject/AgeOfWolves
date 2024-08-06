#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MenuUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuUI, Log, All)

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByMenuUI);
//@Inventory UI 초기화 완료 알림 이벤트
DECLARE_DELEGATE(FNotifyInventoryUIInitFinished);
//@Tool Bar 초기화 완료 알림 이벤트
DECLARE_DELEGATE(FNotifyToolBarInitFinished);

class UScaleBox;
class UImage;
class UTextBlock;
class UOverlay;

class UMenuUIToolBar;
class UInventoryUI;
class ULevelUI;
class UMapUI;
class USystemUI;


/*
* @EMenuCategory
* 
* Menu에서 제공하는 총 4개의 카테고리를 정의하는 열거형
*/
UENUM(BlueprintType)
enum class EMenuCategory : uint8
{
    Inventory = 0	UMETA(DisplayName = "Inventory"),
    Level		    UMETA(DisplayName = "Level"),
    Map             UMETA(DisplayName = "Map"),
    System          UMETA(DisplayName = "System"),
    MAX,
};

/**
 * UMenuUI
 *
 * Menu 관련 UI들의 프레임을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUI : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UMenuUI(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface
protected:
    void InternalBindingToToolBar(UMenuUIToolBar* ToolBar);
    void InternalBindingToInventoryUI(UInventoryUI* InventoryUI);
public:
    //@초기화
    UFUNCTION()
        void InitializeMenuUI();
#pragma endregion

#pragma region SubWidgets
protected:


protected:
    //@Menu UI를 감싸는 Overlay 위젯
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* MenuUIOverlay;

    //@BG 이미지
    UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
        UImage* MenuUI_Outer_BG;
    UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
        UImage* MenuUI_Inner_BG;
    const float Inner_BG_Padding = 10.f;

    //@Tool Bar UI
    UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
        UScaleBox* ToolBarBox;
    UPROPERTY(EditDefaultsOnly, category = "Menu")
        TSubclassOf<UMenuUIToolBar> ToolBarClass;

    //@카테고리와 위젯을 매핑하는 맵
    TMap<EMenuCategory, UUserWidget*> CategoryWidgets;

    //@Inventory UI
    UPROPERTY(BlueprintReadWrite, Category = "Menu | Inventory UI", meta = (BindWidget))
        UScaleBox* InventoryUIBox;
    UPROPERTY(EditDefaultsOnly, category = "Menu | Inventory UI")
        TSubclassOf<UInventoryUI> InventoryUIClass;

    //@Level UI
    UPROPERTY(BlueprintReadWrite, Category = "Menu | Level UI", meta = (BindWidget))
        UScaleBox* LevelUIBox;
    UPROPERTY(EditDefaultsOnly, category = "Menu | Level UI")
        TSubclassOf<ULevelUI> LevelUIClass;
    
    //@Map UI
    UPROPERTY(BlueprintReadWrite, Category = "Menu | Map UI", meta = (BindWidget))
        UScaleBox* MapUIBox;
    UPROPERTY(EditDefaultsOnly, category = "Menu | Map UI")
        TSubclassOf<UMapUI> MapUIClass;
    
    //@System UI
    UPROPERTY(BlueprintReadWrite, Category = "Menu | System UI", meta = (BindWidget))
        UScaleBox* SystemUIBox;
    UPROPERTY(EditDefaultsOnly, category = "Menu | System UI")
        TSubclassOf<USystemUI> SystemUIClass;

public:
    //@카테고리별 UI 가져오기
    UFUNCTION(BlueprintCallable, Category = "Menu")
        UUserWidget* GetCategoryUI(EMenuCategory Category) const;

protected:
    //@BG 생성
    void CreateBG();
    //@Tool Bar 생성
    void CreateToolBar();

protected:
    //@Menu UI 내 모든 카테고리의 UI 생성
    void CreateAllCategoryUIs();

    //Inventory UI 생성
    void CreateInventoryUI();
    //@Level UI 생성
    void CreateLevelUI();
    //@Map UI 생성
    void CreateMapUI();
    //@System UI 생성
    void CreateSystemUI();
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByMenuUI RequestStartInitByMenuUI;
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