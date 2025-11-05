#pragma once

#include "CoreMinimal.h"
#include "08_UI/HorizontalToolBar.h"
#include "MenuUI.h"

#include "MenuUIToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuToolBar, Log, All)

//@이벤트/델리게이트
#pragma region Delegates
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
    friend class UMenuUI;

    GENERATED_BODY()

//@Default Setting
#pragma region Default Setting
public:
    UMenuUIToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

protected:
    //@내부 바인딩
    void InternalBindToButton(UCustomButton* Button, EMenuCategory Category);

public:
    virtual void InitializeToolBar() override;
#pragma endregion

 //@Property/Info...etc
#pragma region SubWidgets
protected:
    //@순수 가상 함수 구현
    virtual void ResetToolBar() override;
    virtual void CreateButtons() override;

protected:
    //@방향키 좌우 이동 편의 함수 (오버라이드)
    virtual void MoveLeft() override;
    virtual void MoveRight() override;

    virtual void MoveSelection(int32 Direction) override;

protected:
    //@버튼 생성 헬퍼
    void CreateAndAddButton(EMenuCategory Category);

protected:
    //@기본 선택 카테고리
    const EMenuCategory DefaultCategory = EMenuCategory::Inventory;

    //@현재 선택된 카테고리
    EMenuCategory CurrentCategory = EMenuCategory::MAX;

    //@버튼 맵 (EMenuCategory → Button)
    TMap<EMenuCategory, UCustomButton*> MMenuCategoryButtons;

    //@버튼 블루프린트 클래스들
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
    FMenuCategoryButtonClicked MenuCategoryButtonClicked;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    virtual void OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex) override;
    virtual void OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex) override;
    virtual void OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex) override;
    virtual void CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex) override;

protected:
    UFUNCTION()
    void MenuUIVisibilityChangedNotified(bool bIsVisible);
#pragma endregion

    //@Utility
#pragma region Utility
protected:
    //@순수 가상 함수 구현
    virtual bool IsValidButtonIndex(uint8 Index) const override;
    virtual UCustomButton* GetButtonByIndex(uint8 Index) const override;

private:
    //@타입 변환 유틸리티
    FORCEINLINE EMenuCategory IndexToMenuCategory(uint8 Index) const { return static_cast<EMenuCategory>(Index); }
    FORCEINLINE uint8 MenuCategoryToIndex(EMenuCategory Category) const { return static_cast<uint8>(Category); }
#pragma endregion
};