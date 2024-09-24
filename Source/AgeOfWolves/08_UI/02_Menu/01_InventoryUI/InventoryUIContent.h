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

//@�ʱ�ȭ ��û �̺�Ʈ
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUIContent)
//@�ʱ�ȭ �Ϸ� �̺�Ʈ
DECLARE_DELEGATE(FInventoryUIContentInitFinished)
//@Item Slots ���ε� �غ� �Ϸ� �̺�Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotsReadyForBinding, const UInventoryUIContent*)

//@Item Slots ���� ����ü
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
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@�ܺ� ���ε�

protected:
    //@���� ���ε�
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
    void InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget);
    void InternalBindingToItemDescription(UItemDescriptionSlot* ItemDescription);

public:
    //@�ʱ�ȭ
    UFUNCTION()
        void InitializeInventoryUIContent();

protected:
    //@�ʱ�ȭ �Ϸ� üũ
    bool bInventoryItemSlotsReady = false;
    bool bInventoryToolBarReady = false;
    bool bInventoryItemDescriptionReady = false;
    void CheckInventoryUIContentInitialization();
#pragma endregion

#pragma region SubWidgets
protected:
    //@Reset
    void ResetInventoryUIContent();

protected:
    //@����
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
    //@�ʱ�ȭ ��û �̺�Ʈ(�񵿱� �ʱ�ȭ, ���� �ʱ�ȭ)
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    //@Inventory UI Content�� �ʱ�ȭ �Ϸ� �̺�Ʈ
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;

public:
    //@Item Slots�� �ʱ�ȭ �Ϸ� �� ���ε��� ���� �غ� �Ϸ� �˸� �̺�Ʈ
    FItemSlotsReadyForBinding ItemSlotsReadyForBinding;
#pragma endregion

#pragma region Callbacks
protected:
    //@Inventory Tool Bar�� �ʱ�ȭ �Ϸ� �̺�Ʈ ����
    UFUNCTION()
        void OnInventoryToolBarInitFinished();
    //@Item Slots�� �ʱ�ȭ �Ϸ� �̺�Ʈ ����
    UFUNCTION()
        void OnInventoryItemSlotsInitFinished();
    //@Item Description �ʱ�ȭ �Ϸ� �̺�Ʈ ����
    UFUNCTION()
        void OnInventoryItemDescriptionInitFinished();

protected:
    //@Inventory Tool Bar�� ��ư Ŭ�� �̺�Ʈ ����
    UFUNCTION()
        void OnInventoryToolBarButtonClicked(EItemType ItemType);
#pragma endregion

#pragma region Utility
public:
    UUserWidget* GetItemSlotsUI(EItemType ItemType) const;
#pragma endregion
};