#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "InventoryUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUIContent, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UInventoryToolBar;
class UItemSlots;
class UItemDescriptionSlot;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUIContent)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIContentInitFinished)

//@Item Slots 바인딩 준비 완료 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotsReadyForBinding, const UInventoryUIContent*)

//@Item Slots의 첫 번째 아이템 슬롯의 Hover 상태 설정 요청
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestFirstItemSlotHover, EItemType)
//@Item Slots의 마지막 Hovered Item Slot의 Cancel 요청
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestCancelCurrentHoveredItemSlot, EItemType)
#pragma endregion

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

//@친추 클래스
#pragma region Friend Class
    friend class UInventoryUI;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UInventoryUIContent(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
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

//@Property/Info...etc
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
    UInventoryToolBar* InventoryToolBar;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Tool Bar")
        TSubclassOf<UInventoryToolBar> InventoryToolBarClass;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemSlotsOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Item Slots")
        TArray<FItemSlotsInfo> ItemSlotInformations;
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

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트(비동기 초기화, 지연 초기화)
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    //@Inventory UI Content의 초기화 완료 이벤트
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;

public:
    //@Item Slots의 초기화 완료 및 바인딩을 위한 준비 완료 알림 이벤트
    FItemSlotsReadyForBinding ItemSlotsReadyForBinding;

public:
    //@첫 번째 아이템 슬롯의 강제 Hover 상태 전환 요청 이벤트
    FRequestFirstItemSlotHover RequestFirstItemSlotHover;
    //@마지막 Hovered 된 아이템 슬롯의 Cancel 요청 이벤트
    FRequestCancelCurrentHoveredItemSlot RequestCancelCurrentHoveredItemSlot;
#pragma endregion

//@Callbacks
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
    UFUNCTION()
        void OnRequestCancelItemSlotsFocus();

protected:
    //@Inventory Tool Bar의 버튼 클릭 이벤트 구독
    UFUNCTION()
        void OnInventoryToolBarButtonClicked(EItemType ItemType);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    UUserWidget* GetItemSlotsUI(EItemType ItemType) const;
    TArray<UItemSlots*> GetAllItemTypesItemSlots() const;
#pragma endregion

};