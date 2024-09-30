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
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
    void InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget);
    void InternalBindingToItemDescription(UItemDescriptionSlot* ItemDescription);

public:
    //@초기화
    UFUNCTION()
        void InitializeInventoryUIContent();

protected:
    //@초기화 완료 체크
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
    //@생성
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
    //@초기화 요청 이벤트(비동기 초기화, 지연 초기화)
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    //@Inventory UI Content의 초기화 완료 이벤트
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;

public:
    //@Item Slots의 초기화 완료 및 바인딩을 위한 준비 완료 알림 이벤트
    FItemSlotsReadyForBinding ItemSlotsReadyForBinding;
#pragma endregion

#pragma region Callbacks
protected:
    //@Inventory Tool Bar의 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnInventoryToolBarInitFinished();
    //@Item Slots의 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnInventoryItemSlotsInitFinished();
    //@Item Description 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnInventoryItemDescriptionInitFinished();

protected:
    //@Inventory Tool Bar의 버튼 클릭 이벤트 구독
    UFUNCTION()
        void OnInventoryToolBarButtonClicked(EItemType ItemType);
#pragma endregion

#pragma region Utility
public:
    UUserWidget* GetItemSlotsUI(EItemType ItemType) const;
#pragma endregion
};