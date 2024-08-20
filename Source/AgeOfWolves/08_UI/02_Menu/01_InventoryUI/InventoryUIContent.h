#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InventoryUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUIContent, Log, All)

//@�ʱ�ȭ ��û �̺�Ʈ
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUIContent)
//@�ʱ�ȭ �Ϸ� �̺�Ʈ
DECLARE_DELEGATE(FInventoryUIContentInitFinished);

class UOverlay;
class UHorizontalBox;

class UInventoryToolBar;
class UItemSlot;

/**
 * @UInventoryUIContent
 *
 * Inventory UI ���� Content�� �����ִ� UI
 *
 * 1. Inventory Tool Bar: Item Slots�� ��Ÿ�� ������ ī�װ��� ������ �� �ִ� Tool Bar
 * 2. Item Slots: ���� Inventory�� ����� Item ����� ��Ÿ���� UI
 * 2. Item Description: ���� ���õ� Item ������ ��Ÿ���� UI
 */
UCLASS()
class AGEOFWOLVES_API UInventoryUIContent : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UInventoryUIContent(const FObjectInitializer& ObjectInitializer);

protected:
    //Interface of UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~End Interface
protected:
    //@���ι��ε�
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
public:
    //@�ʱ�ȭ
    UFUNCTION()
        void InitializeInventoryUIContent();
protected:
    //@Invntory 
    void CheckInventoryUIContentInitialization();
#pragma endregion

#pragma region SubWidgets
protected:
    bool bInventoryToolBarReady = false;
    void CreateToolBar();

    bool bInventoryItemSlotsReady = false;
    void CreateItemSlots();

    bool bInventoryItemDescriptionReady = false;
    void CreateItemDescription();

protected:
    //@Tool Bar Overlay
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Content UI | Tool Bar", meta = (BindWidget))
        UOverlay* ToolBarOverlay;
    //@Tool Bar Blueprint Class
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Tool Bar")
        TSubclassOf<UInventoryToolBar> InventoryToolBarClass;
protected:
    //@Item Slot ����� ���� Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemSlotsOverlay;
    //@Item Slot Blueprint Class
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slot")
        TSubclassOf<UItemSlot> ItemSlotClass;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemDescriptionOverlay;
#pragma endregion

#pragma region Delegate
public:
    //@�ʱ�ȭ ��û �̺�Ʈ
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    //@�ʱ�ȭ �Ϸ� �̺�Ʈ
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    //@Inventory Tool Bar �ʱ�ȭ �Ϸ� �̺�Ʈ�� ����ϴ� �ݹ�
    UFUNCTION()
        void OnInventoryToolBarInitFinished();
#pragma endregion
};
