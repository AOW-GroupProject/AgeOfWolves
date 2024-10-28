#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "ItemSlots.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlots, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UVerticalBox;
class UHorizontalBox;
class UInteractableItemSlot;
class UItemSlot_DropDownMenu;
class UConfirmationMenu;
class UCustomButton;
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
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByItemSlots)
//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FItemSlotsInitFinished);

//@Focus 취소 이벤트
DECLARE_DELEGATE(FRequestCancelItemSlotsFocus);

//@이전 선택된 아이템 슬롯 취소 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FCancelItemSlotButton, const FGuid&)

//@아이템 사용
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemUsed, const FGuid&, int32)
//@아이템 드롭
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemLeft, const FGuid&, int32)
//@아이템 버리기
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemDiscarded, const FGuid&, int32)
#pragma endregion

/**
 * @UItemSlots
 *
 * Item Slot 목록을 표시하는 UI입니다.
 */
    UCLASS()
    class AGEOFWOLVES_API UItemSlots : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
    friend class UInventoryUIContent;
    friend class UItemSlot_DropDownMenu;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UItemSlots(const FObjectInitializer& ObjectInitializer);

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
    void ExternalBindToInputComp();
    void ExternalBindToInventoryComp();

protected:
    //@내부 바인딩
    void InternalBindingToItemSlot(UInteractableItemSlot* ItemSlot, bool bLastItemSlot = false);
    void InternalBindToItemSlotDropDownMenu(UItemSlot_DropDownMenu* DropDownMenu);
    void InternalBindToConfirmationMenu(UConfirmationMenu* Menu);

public:
    //@초기화
    UFUNCTION()
        void InitializeItemSlots();

protected:
    //@초기화 완료 체크
    bool bItemSlotReady = false;
    bool bItemSlotDropDownMenuInitFinished = false;
    bool bConfirmationMenuInitFinished = false;
    void CheckItemSlotInitFinished();
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
public:
    //@주의: 아무 곳에서 호출하면 안됩니다.
    //@Item Slots의 상태를 초기 상태로 리셋합니다.
    UFUNCTION(BlueprintCallable, Category = "Item Slots")
        void ResetItemSlots();

protected:
    //@아이템 슬롯 목록 생성
    void CreateItemSlots();
    //@Drop Down Menu 생성
    void CreateItemSlotDropDownMenu();
    //@Confirmation Menu 생성
    void CreateConfirmationMenu();

protected:
    //@현재 선택된 아이템을 사용합니다.
    UFUNCTION(BlueprintNativeEvent, Category = "Item Slots")
        bool UseItem(const int32 ItemCount);
    virtual bool UseItem_Implementation(const int32 ItemCount);

    //@현재 선택된 아이템을 드롭합니다.
    UFUNCTION(BlueprintNativeEvent, Category = "Item Slots")
        bool LeaveItem(const int32 ItemCount);
    virtual bool LeaveItem_Implementation(const int32 ItemCount);

    //@현재 선택된 아이템을 버립니다.
    UFUNCTION(BlueprintNativeEvent, Category = "Item Slots")
        bool DiscardItem(const int32 ItemCount);
    virtual bool DiscardItem_Implementation(const int32 ItemCount);

protected:
    //@Drop Down Menu 열기 함수
    void OpenDropDownMenu(const FVector2D& Position, EInteractionMethod InteractionMethod);

protected:
        //@방향키 입력 처리
    UFUNCTION(BlueprintNativeEvent, Category = "Item Slots")
        void HandleDirectionalInput(EUINavigation NavigationInput);
    virtual void HandleDirectionalInput_Implementation(EUINavigation NavigationInput);

protected:
    //@Item Slot 목록이 나타낼 아이템 유형
    EItemType ItemType = EItemType::MAX;

protected:
    //@현재 호버된 Item Slot에 대한 Weak Object Ptr
    TWeakObjectPtr<UInteractableItemSlot> CurrentHoveredItemSlot;
    //@현재 선택된 Item Slot에 대한 Weak Object Ptr
    TWeakObjectPtr<UInteractableItemSlot> CurrentSelectedItemSlot;
    //@Item Slot
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UVerticalBox* ItemSlotBox;
    //@Item Slot Blueprint Class
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slot")
        TSubclassOf<UInteractableItemSlot> InteractableItemSlotClass;

protected:
    //@Item Slot 목록
    TArray<UInteractableItemSlot*> ItemSlots;

    //@최대 생성 가능한 Item Slot의 행 개수
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slots")
        int32 DefaultRows;
    //@최초 제공하는 Item Slot 행 개수
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slots")
        int32 MaxRows;
    //@행 별 최대 생성 가능한 Item Slot 개수
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slots")
        int32 MaxItemSlotsPerRow;
    //@행 간 간격 설정
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slots")
        FMargin PaddingBetweenRows = FMargin(0, 10.f);
    //@행 내 Item Slote 들간의 간격 설정
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Item Slots")
        FMargin PaddingBetweenItemSlots = FMargin(10.f, 10.f);

protected:
    //@현재 선택한 Drop Down Menu Option 명
    FName CurrentSelectedDropDownMenuOptionName;
    //@아이템 슬롯 메뉴
    TObjectPtr<UItemSlot_DropDownMenu> ItemSlotDropDownMenu;
    //@아이템 슬롯 메뉴 클래스
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Drop Down Menu")
        TSubclassOf<UItemSlot_DropDownMenu> ItemSlotDropDownMenuClass;

protected:
    //@확정 메뉴
    TObjectPtr<UConfirmationMenu> ConfirmationMenu;
    //@확정 메뉴 클래스
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Confirmation Menu")
        TSubclassOf<UConfirmationMenu> ConfirmationMenuClass;
#pragma endregion

//@Delegates
#pragma region Delegate
public:
    //@초기화 요청 이벤트
    FRequestStartInitByItemSlots RequestStartInitByItemSlots;
    //@초기화 완료 이벤트
    FItemSlotsInitFinished ItemSlotsInitFinished;

public:
    FRequestCancelItemSlotsFocus RequestCancelItemSlotsFocus;

public:
    //@마지막 선택된 아이템 슬롯의 선택 취소 이벤트
    FCancelItemSlotButton CancelItemSlotButton;

public:
    //@아이템 사용 이벤트
    FItemUsed ItemUsed;
    //@아이템 드롭 이벤트
    FItemLeft ItemLeft;
    //@아이테 버리기 이벤트
    FItemDiscarded ItemDiscarded;
#pragma endregion

//@Callbacks
#pragma region Callback
protected:
    //@가시성 변화 이벤트 구독
    UFUNCTION()
        void OnUIVisibilityChanged(ESlateVisibility VisibilityType);

protected:
    //@마지막 Item Slot 초기화 완료 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnItemSlotInitFinished();
    //@아이템 슬롯 메뉴의 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnItemSlotDropDownMenuInitFinished();
    //@확정 메뉴의 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnConfirmationMenuInitFinished();

protected:
    //@Item Slot Button 호버 이벤트 구독
    UFUNCTION()
        void OnItemSlotButtonHovered(const FGuid& UniqueItemID, EInteractionMethod InteractionMethodType);
    UFUNCTION()
        void OnItemSlotButtonUnhovered(const FGuid& UniqueItemID);
    //@Item Slot Button 클릭 이벤트 구독
    UFUNCTION()
        void OnItemSlotButtonClicked(const FGuid& UniqueItemID, EInteractionMethod InteractionMethodType);

protected:
    UFUNCTION()
        void OnItemSlotDropDownMenuClosed(ESlateVisibility VisibilityType);

    //@아이템 슬롯 메뉴의 옵션 선택 이벤트 구독
    UFUNCTION()
        void OnItemSlotDropDownMenuOptionSelected(FName ItemSlotDropDownMenuOptionName);

protected:
    //@확정 메뉴의 옵션 선택 이벤트 구독
    UFUNCTION()
        void OnConfirmationMenuOptionSelected(FName OkOrCancel);

public:
    //@Item Slots의 첫 번째 아이템 슬롯의 강제 Hover 상태 전환 이벤트 구독 
    UFUNCTION()
        void OnRequestFirstItemSlotHover(EItemType RequestedItemType);
    //@Item Slots의 현재 Hovered 아이템 슬롯의 취소 이벤트 구독
    UFUNCTION()
        void OnRequestCancelCurrentHoveredItemSlot(EItemType RequestedItemType);

protected:
    //@Inventory Comp의 아이템 할당 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag);
    //@Inventory Comp의 아이템 제거 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemRemovedFromInventory(const FGuid& UniqueItemID);
    //@Inventory Comp의 아이테 업데이트 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag, int32 UpdatedItemCount);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility Functions
public:
    // 모든 Item Slot을 반환하는 함수
    TArray<UInteractableItemSlot*> GetAllItemSlots() const;

public:
    FORCEINLINE EItemType GetItemType() const { return ItemType; }
    FORCEINLINE void SetItemType(const EItemType& Type) { ItemType = Type; }

public:
    FORCEINLINE UInteractableItemSlot* GetCurrentHoveredItemSlot() const { return CurrentHoveredItemSlot.Get(); }

protected:
    // 새로운 유틸리티 함수들을 추가합니다.
    int32 GetSlotIndex(int32 Row, int32 Column) const;
    void GetSlotRowAndColumn(int32 Index, int32& OutRow, int32& OutColumn) const;
    UInteractableItemSlot* GetSlotAtPosition(int32 Row, int32 Column) const;

protected:
    //@Item Slot 목록 중 좌측 최 상단에 위치한 첫 번째 Item Slot을 찾습니다.
    UInteractableItemSlot* FindFirstItemSlot();
    //@빈 슬롯 찾기
    UInteractableItemSlot* FindEmptySlot();
    //@아이템 ID로 슬롯 찾기
    UInteractableItemSlot* FindSlotByItemID(const FGuid& ItemID);
#pragma endregion

};