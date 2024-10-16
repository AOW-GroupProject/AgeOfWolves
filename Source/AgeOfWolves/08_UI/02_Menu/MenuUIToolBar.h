// MenuUIToolBar.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI.h"

#include "MenuUIToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogToolBar, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UHorizontalBox;
class UCustomButton;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FToolBarInitFinished)

//@Menu Category 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FMenuCategoryButtonClicked, EMenuCategory)
#pragma endregion

/**
 * UMenuUIToolBar
 *
 * Menu UI 최상단에 위치하는 툴 바를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUIToolBar : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UMenuUIToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    void InternalBindToButton(UCustomButton* Button, EMenuCategory Category);

public:
    UFUNCTION()
        void InitializeToolBar();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@강제로 Default Setting으로 리셋합니다.
    UFUNCTION(BlueprintCallable, Category = "Menu Tool Bar")
        void ResetToolBar();

protected:
    //@버튼 생성
    void CreateButtons();
    //@카테고리별 버튼 생성 및 추가
    void CreateAndAddButton(EMenuCategory Category, TSubclassOf<UCustomButton> ButtonClass);

public:
    //@메뉴 카테고리를 왼쪽으로 이동
    void MoveCategoryLeft();
    //@메뉴 카테고리를 오른쪽으로 이동
    void MoveCategoryRight();

protected:
    //@Tool Bar의 Horizontal Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* CategoryButtonsBox;

protected:
    //@Reset 시 설정할 Default 메뉴 카테고리
    const EMenuCategory DefaultCategory = EMenuCategory::Inventory;
    //@현재 선택된 카테고리
    EMenuCategory CurrentCategory = EMenuCategory::MAX;
    //@MenuCategory별 버튼들을 담고 있는 맵
    TMap<EMenuCategory, UCustomButton*> MMenuCategoryButtons;
    //@Menu Category 별 버튼들의 블루프린트 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Menu Tool Bar | Buttons")
        TSubclassOf<UCustomButton> InventoryButtonClass;
    UPROPERTY(EditDefaultsOnly, Category = "Menu Tool Bar | Buttons")
        TSubclassOf<UCustomButton> LevelButtonClass;
    UPROPERTY(EditDefaultsOnly, Category = "Menu Tool Bar | Buttons")
        TSubclassOf<UCustomButton> MapButtonClass;
    UPROPERTY(EditDefaultsOnly, Category = "Menu Tool Bar | Buttons")
        TSubclassOf<UCustomButton> SystemButtonClass;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@Tool Bar의 초기화 완료 이벤트
    FToolBarInitFinished ToolBarInitFinished;

public:
    //@Menu Category Button의 선택 이벤트
    FMenuCategoryButtonClicked MenuCategoryButtonClicked;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    UFUNCTION()
        void MenuUIVisibilityChangedNotified(bool bIsVisible);

protected:
    //@Menu UI Tool Bar 버튼 클릭 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnMenuUIToolBarButtonClicked(EMenuCategory Category);
    virtual void OnMenuUIToolBarButtonClicked_Implementation(EMenuCategory Category);
    //@Menu UI Tool Bar 버튼 Hover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnMenuUIToolBarButtonHovered(EMenuCategory Category);
    virtual void OnMenuUIToolBarButtonHovered_Implementation(EMenuCategory Category);
    //@Menu UI Tool Bar 버튼 Unhover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnMenuUIToolBarButtonUnhovered(EMenuCategory Category);
    virtual void OnMenuUIToolBarButtonUnhovered_Implementation(EMenuCategory Category);

protected:
    //@Menu UI Tool Bar 버튼 선택 취소 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void CancelMenuUIToolBarButtonSelected(EMenuCategory PreviousCategory);
    virtual void CancelMenuUIToolBarButtonSelected_Implementation(EMenuCategory PreviousCategory);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@현재 선택된 카테고리의 인덱스를 반환
    int32 GetCurrentCategoryIndex() const;
    //@인덱스에 해당하는 메뉴 카테고리를 반환
    EMenuCategory GetMenuCategoryFromIndex(int32 Index) const;
#pragma endregion

};