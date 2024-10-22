#pragma once

#include "CoreMinimal.h"
#include "08_UI/HorizontalToolBar.h"
#include "MenuUI.h"

#include "MenuUIToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuToolBar, Log, All)

//@이벤트/델리게이트
#pragma region Delegates
//@Menu Category 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FMenuCategoryButtonClicked, EMenuCategory)
#pragma endregion

/**
 * UMenuUIToolBar
 *
 * Menu UI 최상단에 위치하는 카테고리 선택 툴바입니다.
 */
    UCLASS()
    class AGEOFWOLVES_API UMenuUIToolBar : public UHorizontalToolBar
{

//@친추 클래스
#pragma region Friend Class
    friend class UMenuUI;
#pragma endregion

    GENERATED_BODY()

        //@Default Setting
#pragma region Default Setting
public:
    UMenuUIToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interface
    virtual void NativeOnInitialized() override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩
    void InternalBindToButton(UCustomButton* Button, EMenuCategory Category);

public:
    //@초기화
    virtual void InitializeToolBar() override;
#pragma endregion

    //@Property/Info...etc
#pragma region SubWidgets
protected:
    virtual void ResetToolBar() override;

protected:
    //@버튼 생성
    virtual void CreateButtons() override;
    void CreateAndAddButton(EMenuCategory Category);

protected:
    virtual void MoveSelection(int32 Direction) override;

protected:
    const EMenuCategory DefaultCategory = EMenuCategory::Inventory;
    EMenuCategory CurrentCategory = EMenuCategory::MAX;
    TMap<EMenuCategory, UCustomButton*> MMenuCategoryButtons;

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
    //@Menu Category Button의 선택 이벤트
    FMenuCategoryButtonClicked MenuCategoryButtonClicked;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    //@버튼 이벤트 override
    virtual void OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex) override;
    virtual void OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex) override;
    virtual void OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex) override;
    virtual void CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex) override;

protected:
    UFUNCTION()
        void MenuUIVisibilityChangedNotified(bool bIsVisible);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@인덱스의 유효성 검사 override
    virtual bool IsValidButtonIndex(uint8 Index) const override;

private:
    //@uint8 <-> EMenuCategory 변환 유틸리티
    FORCEINLINE EMenuCategory IndexToMenuCategory(uint8 Index) const { return static_cast<EMenuCategory>(Index); }
    FORCEINLINE uint8 MenuCategoryToIndex(EMenuCategory Category) const { return static_cast<uint8>(Category); }
#pragma endregion
};