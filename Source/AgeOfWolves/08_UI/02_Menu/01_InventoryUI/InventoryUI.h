#pragma once

#include "CoreMinimal.h"
#include "08_UI/02_Menu/MenuUIContent.h"
#include "GameplayTagContainer.h"
#include "09_Item/Item.h"

#include "InventoryUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UImage;
class UInventoryToolBar;
class UItemSlots;
class UItemDescriptionSlot;
class UInteractableItemSlot;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
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
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUI)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIInitFinished)

//@Item Slots 바인딩 준비 완료 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotsReadyForBinding, const UInventoryUI*)

//@Item Slots의 첫 번째 아이템 슬롯의 Hover 상태 설정 요청
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestFirstItemSlotHover, EItemType)
//@Item Slots의 마지막 Hovered Item Slot의 Cancel 요청
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestCancelCurrentHoveredItemSlot, EItemType)
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UInventoryUI : public UMenuUIContent
{
    //@친추 클래스
#pragma region Friend Class
    friend class UInventoryToolBar;
    friend class UItemSlots;
    friend class UItemDescriptionSlot;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting        
#pragma region Default Setting
public:
    UInventoryUI(const FObjectInitializer& ObjectInitializer);

protected:
    //Interface of UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    //~End UUserWidget

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
    void InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget);
    void InternalBindingToItemDescription(UItemDescriptionSlot* ItemDescription);

public:
    //@초기화
    virtual void InitializeMenuUIContent() override;

protected:
    //@초기화 완료 체크
    bool bInventoryItemSlotsReady = false;
    bool bInventoryToolBarReady = false;
    bool bInventoryItemDescriptionReady = false;
    void CheckInventoryUIInitialization();

protected:
    //@Invenotry UI의 내부 컨텐츠 초기화 완료 체크
    bool bInventoryUIContentReady = false;
    virtual void CheckMenuUIContentInitFinished() override;
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
private:
    //@Inventory UI의 상태를 초기 상태로 되돌립니다.
    virtual void ResetMenuUIContent() override;

protected:
    //@생성
    void CreateToolBar();
    void CreateAllItemSlots();
    void CreateItemDescription();

protected:
    //@Reset
    void ResetInventoryUI();

protected:
    void UpdateAllItemSlotsVisibility();
    void SetItemTypeVisibility(EItemType ItemType, bool bVisible);

protected:
    //@Tool Bar
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UOverlay* ToolBarOverlay;
    UInventoryToolBar* InventoryToolBar;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Tool Bar")
    TSubclassOf<UInventoryToolBar> InventoryToolBarClass;

protected:
    //@Item Slots
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UOverlay* ItemSlotsOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Item Slots")
    TArray<FItemSlotsInfo> ItemSlotInformations;
    UPROPERTY()
    TMap<EItemType, UUserWidget*> MItemSlots;

    EItemType DefaultItemType = EItemType::Tool;
    EItemType CurrentItemType = EItemType::MAX;

protected:
    //@Item Description
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UOverlay* ItemDescriptionOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Item Description")
    TSubclassOf<UItemDescriptionSlot> ItemDescriptionSlotClass;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByInventoryUI RequestStartInitByInventoryUI;
    //@초기화 완료 이벤트
    FInventoryUIInitFinished InventoryUIInitFinished;

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

protected:
    //@UI의 가시성 변화 이벤트
    virtual void OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType) override;
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    UUserWidget* GetItemSlotsUI(EItemType ItemType) const;
    TArray<UItemSlots*> GetAllItemTypesItemSlots() const;
    UItemSlots* GetItemSlotsByType(EItemType ItemType) const;
#pragma endregion

};