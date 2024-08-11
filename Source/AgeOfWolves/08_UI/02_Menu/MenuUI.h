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
    //@외부 바인딩
    void ExternalBindingToUIComponent();

protected:
    //@내부 바인딩
    void InternalBindingToToolBar(UMenuUIToolBar* ToolBar);
    void InternalBindingToInventoryUI(UInventoryUI* InventoryUI);
    void InternalBindingToLevelUI(ULevelUI* LevelUI);
    void InternalBindingToMapUI(UMapUI* MapUI);
    void InternalBindingToSystemUI(USystemUI* SystemUI);
public:
    //@초기화
    UFUNCTION()
        void InitializeMenuUI();
#pragma endregion

#pragma region SubWidgets
protected:
    //@현재 보여지고 있는 Menu UI의 자식 UI
    EMenuCategory CurrentCategory = EMenuCategory::Inventory;

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
    TMap<EMenuCategory, UUserWidget*> ChildUIs;

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

protected:
    //@BG 생성
    void CreateBG();
    //@Tool Bar 생성
    void CreateToolBar();

protected:
    void CreateAllCategoryUIs();
    //@inventory UI
    void CreateInventoryUI();
    //@Level UI
    void CreateLevelUI();
    //@Map UI
    void CreateMapUI();
    //@System UI
    void CreateSystemUI();

public:
    //@가시성 관리
    void SetCategoryVisibility(EMenuCategory Category, bool bVisible);
    //@카테고리별 UI 가져오기
    UFUNCTION(BlueprintCallable, Category = "Menu")
        UUserWidget* GetCategoryUI(EMenuCategory Category) const;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByMenuUI RequestStartInitByMenuUI;

public:
    //@Inventory UI의 초기화 완료 이벤트
    FNotifyInventoryUIInitFinished NotifyInventoryUIInitFinished;
    //@Tool Bar UI의 초기화 완료 이벤트
    FNotifyToolBarInitFinished NotifyToolBarInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    //@UI의 가시성 변화 이벤트에 바인딩 되는 콜백
    UFUNCTION()
        void OnUIVisibilityChanged(UUserWidget* Widget, bool bVisible);
protected:
    UFUNCTION()
        void OnToolBarInitFinished();
    UFUNCTION()
        void OnInventoryUIInitFinished();
    UFUNCTION()
        void OnLevelUIInitFinished();
    UFUNCTION()
        void OnMapUIInitFinished();
    UFUNCTION()
        void OnSystemUIInitFinished();
protected:
    //@Menu Tool Bar의 Menu Category 선택 이벤트에 바인딩 되는 콜백
    UFUNCTION()
        void OnMenuCategoryButtonClikced(EMenuCategory MenuCategory);
#pragma endregion
};