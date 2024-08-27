#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "InventoryToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryToolBar, Log, All)

//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryToolBarInitFinished);

//@Item Type 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FInventoryToolBarButtonClikced, EItemType);

class UHorizontalBox;
class UButton;

/**
 * @UInventoryToolBar
 *
 * Inventory UI의 Item Slot에 나타낼 아이템 타입을 선택할 수 있는 툴 바
 */
UCLASS()
class AGEOFWOLVES_API UInventoryToolBar : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UInventoryToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
public:
    UFUNCTION()
        void InitializeInventoryToolBar();
#pragma endregion

#pragma region Widgets
protected:
    void CreateButtons();

protected:
    //@Tool Bar의 Horizontal Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* ItemTypeButtonBox;

protected:
    //@현재 선택된 아이템 타입
    EItemType CurrentItemType = EItemType::Tool;
    //@아이템 타입 버튼들을 담고 있는 맵
    TMap<UButton*, EItemType> MItemTypeButtons;
    //@Tool 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* ToolTypeButton;
    //@Material 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* MaterialTypeButton;
    //@Memory(Info) 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* MemoryTypeButton;
    //@Equipment 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UButton* EquipmentTypeButton;

private:
    //@현재 선택된 버튼의 스타일을 업데이트합니다.
    void UpdateButtonStyle(UButton* SelectedButton, UButton* PreviousButton = nullptr);

private:
    void HandleButtonClick(EItemType ItemType);
    void HandleButtonHover(EItemType ItemType);
    void HandleButtonUnhover(EItemType ItemType);
#pragma endregion

#pragma region Delegates
public:
    //@Inventory Tool Bar 초기화 완료 이벤트
    FInventoryToolBarInitFinished InventoryToolBarInitFinished;
    //@Inventory Tool Bar 버튼 클릭 이벤트
    FInventoryToolBarButtonClikced InventoryToolBarButtonClikced;
#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION()
        FORCEINLINE void OnToolButtonClicked() { HandleButtonClick(EItemType::Tool); }
    UFUNCTION()
        FORCEINLINE void OnMaterialButtonClicked() { HandleButtonClick(EItemType::Material); }
    UFUNCTION()
        FORCEINLINE void OnMemoryButtonClicked() { HandleButtonClick(EItemType::Memory); }
    UFUNCTION()
        FORCEINLINE void OnEquipmentButtonClicked() { HandleButtonClick(EItemType::Equipment); }

    UFUNCTION()
        FORCEINLINE void OnToolButtonHovered() { HandleButtonHover(EItemType::Tool); }
    UFUNCTION()
        FORCEINLINE void OnMaterialButtonHovered() { HandleButtonHover(EItemType::Material); }
    UFUNCTION()
        FORCEINLINE void OnMemoryButtonHovered() { HandleButtonHover(EItemType::Memory); }
    UFUNCTION()
        FORCEINLINE void OnEquipmentButtonHovered() { HandleButtonHover(EItemType::Equipment); }

    UFUNCTION()
        FORCEINLINE void OnToolButtonUnhovered() { HandleButtonUnhover(EItemType::Tool); }
    UFUNCTION()
        FORCEINLINE void OnMaterialButtonUnhovered() { HandleButtonUnhover(EItemType::Material); }
    UFUNCTION()
        FORCEINLINE void OnMemoryButtonUnhovered() { HandleButtonUnhover(EItemType::Memory); }
    UFUNCTION()
        FORCEINLINE void OnEquipmentButtonUnhovered() { HandleButtonUnhover(EItemType::Equipment); }
#pragma endregion
};