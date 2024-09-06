#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "InventoryUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUIContent, Log, All)

class UOverlay;
class UInventoryToolBar;
class UItemSlots;
class UItemDescriptionSlot;

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUIContent)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIContentInitFinished)
//@Item Slots 바인딩 준비 완료 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotsReadyForBinding, const UInventoryUIContent*)

//@Item Slots 정보 구조체
USTRUCT(BlueprintType)
struct FItemSlotsInfo
{
    GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        EItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<UItemSlots> ItemSlotsClass;
};

UCLASS()
class AGEOFWOLVES_API UInventoryUIContent : public UUserWidget
{
    //@Friend Class
    friend class UInventoryUI;

    GENERATED_BODY()

#pragma region Default Setting
public:
    UInventoryUIContent(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
    void InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget);
    void InternalBindingToItemDescription(UItemDescriptionSlot* ItemDescription);

public:
    UFUNCTION()
        void InitializeInventoryUIContent();

protected:
    bool bInventoryItemSlotsReady = false;
    bool bInventoryToolBarReady = false;
    bool bInventoryItemDescriptionReady = false;
    void CheckInventoryUIContentInitialization();
#pragma endregion

#pragma region SubWidgets
protected:
    void ResetInventoryUIContent();

protected:
    void CreateToolBar();
    void CreateAllItemSlots();
    void CreateItemDescription();

protected:
    void UpdateAllItemSlotsVisibility();
    void SetItemTypeVisibility(EItemType ItemType, bool bVisible);

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ToolBarOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Tool Bar")
        TSubclassOf<UInventoryToolBar> InventoryToolBarClass;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemSlotsOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Item Slots")
        TArray<FItemSlotsInfo> ItemSlots;
    UPROPERTY()
        TMap<EItemType, UUserWidget*> MItemSlots;

    EItemType DefaultItemType = EItemType::Tool;
    EItemType CurrentItemType = EItemType::MAX;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemDescriptionOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Item Description")
        TSubclassOf<UItemDescriptionSlot> ItemDescriptionSlotClass;

#pragma endregion

#pragma region Delegates
public:
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;
    FItemSlotsReadyForBinding ItemSlotsReadyForBinding;
#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION()
        void OnInventoryToolBarInitFinished();

    UFUNCTION()
        void OnInventoryItemSlotsInitFinished();

    UFUNCTION()
        void OnInventoryItemDescriptionInitFinished();


protected:
    UFUNCTION()
        void OnInventoryToolBarButtonClicked(EItemType ItemType);
#pragma endregion

#pragma region Utility

public:
    UUserWidget* GetItemSlotsUI(EItemType ItemType) const;
#pragma endregion
};