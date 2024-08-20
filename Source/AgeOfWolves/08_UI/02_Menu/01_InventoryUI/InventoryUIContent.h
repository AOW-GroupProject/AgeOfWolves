#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InventoryUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUIContent, Log, All)

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUIContent)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIContentInitFinished);

class UOverlay;
class UHorizontalBox;

class UInventoryToolBar;
class UItemSlot;

/**
 * @UInventoryUIContent
 *
 * Inventory UI 내부 Content를 보여주는 UI
 *
 * 1. Inventory Tool Bar: Item Slots에 나타낼 아이템 카테고리를 선택할 수 있는 Tool Bar
 * 2. Item Slots: 현재 Inventory에 저장된 Item 목록을 나타내는 UI
 * 2. Item Description: 현재 선택된 Item 정보를 나타내는 UI
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
    //@내부바인딩
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
public:
    //@초기화
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
    //@Item Slot 목록을 담을 Overlay
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
    //@초기화 요청 이벤트
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    //@초기화 완료 이벤트
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    //@Inventory Tool Bar 초기화 완료 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnInventoryToolBarInitFinished();
#pragma endregion
};
