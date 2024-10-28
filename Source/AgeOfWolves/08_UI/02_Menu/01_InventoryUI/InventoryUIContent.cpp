#include "InventoryUIContent.h"
#include "Logging/StructuredLog.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryToolBar.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemDescriptionSlot.h"
#include "08_UI/InteractableItemSlot.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogInventoryUIContent)

//@Defualt Setting
#pragma region Default Setting
UInventoryUIContent::UInventoryUIContent(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    InventoryToolBar = nullptr;
}

void UInventoryUIContent::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UInventoryUIContent::NativePreConstruct()
{
    Super::NativePreConstruct();

}

void UInventoryUIContent::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);

}

void UInventoryUIContent::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UInventoryUIContent::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);
}

FReply UInventoryUIContent::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    
    //@Set Directly(SetFocus())를 통한 포커스 시도 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return FReply::Handled().ClearUserFocus();
    }

    UE_LOGFMT(LogInventoryUIContent, Log, "포커스 : 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

    return FReply::Handled();
}

void UInventoryUIContent::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    //@SetDirectly(SetFocus())를 통한 포커스 소실 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        SetFocus();

        return;
    }

    Super::NativeOnFocusLost(InFocusEvent);

    UE_LOGFMT(LogInventoryUIContent, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
}

FReply UInventoryUIContent::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogInventoryUIContent, Log, "키 입력 감지됨: {0}", *Key.ToString());

    //@Inventory Tool Bar
    if (!InventoryToolBar)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "InventoryToolBar를 찾을 수 없습니다.");
        return FReply::Unhandled();
    }

    //@좌, 우 방향키 조작
    if (Key == EKeys::Left)
    {
        InventoryToolBar->MoveLeft();
        return FReply::Handled();
    }
    else if (Key == EKeys::Right)
    {
        InventoryToolBar->MoveRight();
        return FReply::Handled();
    }
    else if (Key == EKeys::Enter || Key == EKeys::Down)
    {
        //@Current Item Slots
        UItemSlots* CurrentItemSlots = Cast<UItemSlots>(GetItemSlotsUI(CurrentItemType));
        if (!CurrentItemSlots)
        {
            UE_LOGFMT(LogInventoryUIContent, Error, "{0} 타입의 ItemSlots를 찾을 수 없습니다.", *UEnum::GetValueAsString(CurrentItemType));
            return FReply::Handled();
        }

        //@첫 번째 아이템 슬롯 유효한가?
        UInteractableItemSlot* FirstItemSlot = CurrentItemSlots->FindFirstItemSlot();
        if (!FirstItemSlot || !FirstItemSlot->GetUniqueItemID().IsValid())
        {
            UE_LOGFMT(LogInventoryUIContent, Warning, "{0} 타입의 첫 번째 Item Slot이 유효하지 않습니다.", *UEnum::GetValueAsString(CurrentItemType));
            return FReply::Handled();
        }

        //@첫 번째 아이템 슬롯의 강제 호버 상태 전환 요청 이벤트
        RequestFirstItemSlotHover.Broadcast(CurrentItemType);

        //@SetFocus
        CurrentItemSlots->SetFocus();

        UE_LOGFMT(LogInventoryUIContent, Log, "포커스가 {0} 타입의 ItemSlots로 이동했습니다.", *UEnum::GetValueAsString(CurrentItemType));

        return FReply::Handled();
    }
    else if (Key == EKeys::Escape || Key == EKeys::Up)
    {
        //@Current Item Slots
        UItemSlots* CurrentItemSlots = Cast<UItemSlots>(GetItemSlotsUI(CurrentItemType));
        if (CurrentItemSlots && CurrentItemSlots->HasKeyboardFocus())
        {
            //@Set Focus
            SetFocus();

            //@Request Cancel Current Hovered Item Slot
            RequestCancelCurrentHoveredItemSlot.Broadcast(CurrentItemType);

            return FReply::Handled();
        }
        else
        {
            //@ItemSlots에 포커스가 없으면 상위 위젯에서 처리하도록 Unhandled 반환 및 Focus 해제
            return FReply::Unhandled().ClearUserFocus();
        }
    }

    UE_LOGFMT(LogInventoryUIContent, Log, "Inventory UI에서 처리하지 않는 키 입력: {0}", *Key.ToString());
    return FReply::Unhandled();
}

void UInventoryUIContent::InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar)
{
    //@Tool Bar
    if (!ToolBar)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }

    //@내부 바인딩
    ToolBar->ToolBarInitFinished.BindUFunction(this, "OnInventoryToolBarInitFinished");
    ToolBar->InventoryToolBarButtonClicked.BindUFunction(this, "OnInventoryToolBarButtonClicked");

}

void UInventoryUIContent::InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget)
{
    //@Item Slots
    if (!ItemSlotsWidget)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ItemSlots UI가 유효하지 않습니다.");
        return;
    }
    //@초기화 완료 이벤트
    ItemSlotsWidget->ItemSlotsInitFinished.BindUFunction(this, "OnInventoryItemSlotsInitFinished");
    ItemSlotsWidget->RequestCancelItemSlotsFocus.BindUFunction(this, "OnRequestCancelItemSlotsFocus");
}

void UInventoryUIContent::InternalBindingToItemDescription(UItemDescriptionSlot* ItemDescription)
{
    //@Item Slots
    if (!ItemDescription)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "Item Description UI가 유효하지 않습니다.");
        return;
    }
    //@초기화 완료 이벤트
    ItemDescription->ItemDescriptionSlotInitFinished.BindUFunction(this, "OnInventoryItemDescriptionInitFinished");
}

void UInventoryUIContent::InitializeInventoryUIContent()
{
    //@Inventory Tool Bar
    CreateToolBar();
    //@Item Slots 
    CreateAllItemSlots();
    //@Item Description
    CreateItemDescription();
    //@초기화 요청 이벤트 호출
    RequestStartInitByInventoryUIContent.Broadcast();
}

void UInventoryUIContent::CheckInventoryUIContentInitialization()
{
    //@초기화 완료 이벤트 호출
    if (bInventoryItemSlotsReady && bInventoryToolBarReady && bInventoryItemDescriptionReady)
    {
        bInventoryItemSlotsReady = false;
        bInventoryToolBarReady = false;
        bInventoryItemDescriptionReady = false;

        for (auto& Pair : MItemSlots)
        {
            if (UItemSlots* ItemSlotsWidget = Cast<UItemSlots>(Pair.Value))
            {
                RequestFirstItemSlotHover.AddUObject(ItemSlotsWidget, &UItemSlots::OnRequestFirstItemSlotHover);
                RequestCancelCurrentHoveredItemSlot.AddUObject(ItemSlotsWidget, &UItemSlots::OnRequestCancelCurrentHoveredItemSlot);
            }
        }

        //@Reset
        ResetInventoryUIContent();

        //@Inventory UI Content 초기화 완료 이벤트 호출
        InventoryUIContentInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
void UInventoryUIContent::ResetInventoryUIContent()
{

    UE_LOGFMT(LogInventoryUIContent, Log, "Inventory UI Content 구성 위젯들을 초기 상태로 리셋합니다.");

    //@Current Item Type
    if (CurrentItemType != EItemType::MAX)
    {
        //@Item Slots
        UUserWidget* ItemSlotsUI = GetItemSlotsUI(CurrentItemType);
        if (UItemSlots* ItemSlotsWidget = Cast<UItemSlots>(ItemSlotsUI))
        {
            ItemSlotsWidget->ResetItemSlots();
            UE_LOGFMT(LogInventoryUIContent, Log, "현재 타입({0})의 Item Slots가 초기화되었습니다.", *UEnum::GetValueAsString(CurrentItemType));
        }
        else
        {
            UE_LOGFMT(LogInventoryUIContent, Warning, "현재 타입({0})의 Item Slots를 찾을 수 없거나 초기화하지 못했습니다.", *UEnum::GetValueAsString(CurrentItemType));
        }

        //@Item Description
        bool bItemDescriptionReset = false;
        for (auto OverlaySlot : ItemDescriptionOverlay->GetAllChildren())
        {
            if (UItemDescriptionSlot* ItemSlotWidget = Cast<UItemDescriptionSlot>(OverlaySlot))
            {
                ItemSlotWidget->ResetItemDescriptionSlot();
                bItemDescriptionReset = true;
                UE_LOGFMT(LogInventoryUIContent, Log, "Item Description Slot이 초기화되었습니다.");
                break;
            }
        }
        if (!bItemDescriptionReset)
        {
            UE_LOGFMT(LogInventoryUIContent, Warning, "Item Description Slot을 찾을 수 없거나 초기화하지 못했습니다.");
        }
    }

    //@Inventory Toolbar
    bool bToolbarReset = false;
    for (auto OverlaySlot : ToolBarOverlay->GetAllChildren())
    {
        UE_LOGFMT(LogInventoryUIContent, Warning, "{0}", *OverlaySlot->GetName());

        if (UInventoryToolBar* ToolBar = Cast<UInventoryToolBar>(OverlaySlot))
        {
            ToolBar->ResetToolBar();

            bToolbarReset = true;

            UE_LOGFMT(LogInventoryUIContent, Log, "Inventory Toolbar가 초기화되었습니다.");
            break;
        }
    }
    if (!bToolbarReset)
    {
        UE_LOGFMT(LogInventoryUIContent, Warning, "Inventory Toolbar를 찾을 수 없거나 초기화하지 못했습니다.");
    }

    UE_LOGFMT(LogInventoryUIContent, Log, "Inventory UI Content 구성 위젯들의 초기화가 완료되었습니다.");
}

void UInventoryUIContent::CreateToolBar()
{
    if (!ensureMsgf(InventoryToolBarClass && ToolBarOverlay, TEXT("InventoryToolBarClass 또는 ToolBarOverlay가 유효하지 않습니다.")))
    {
        return;
    }
    //@Tool Bar
    InventoryToolBar = CreateWidget<UInventoryToolBar>(this, InventoryToolBarClass);
    if (!IsValid(InventoryToolBar))
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "InventoryToolBar 위젯 생성에 실패했습니다.");
        return;
    }
    //@비동기 초기화 이벤트
    RequestStartInitByInventoryUIContent.AddUFunction(InventoryToolBar, "InitializeToolBar");
    //@내부 바인딩
    InternalBindingToInventoryToolBar(InventoryToolBar);
    //@Tool Bar Overlay
    if (UOverlaySlot* OverlaySlot = ToolBarOverlay->AddChildToOverlay(InventoryToolBar))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    UE_LOGFMT(LogInventoryUIContent, Log, "InventoryToolBar가 성공적으로 생성되고 ToolBarOverlay에 추가되었습니다.");
}

void UInventoryUIContent::CreateAllItemSlots()
{
    //@Item Slots Overlay
    if (!IsValid(ItemSlotsOverlay))
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ItemSlotsOverlay가 유효하지 않습니다.");
        return;
    }
    //@TSet, Item Type 별 하나의 Item Slots만 생성되도록 제한
    TSet<EItemType> CreatedItemTypes;
    //@FItemSlotsInfo
    for (const FItemSlotsInfo& SlotInfo : ItemSlotInformations)
    {
        //@Contains
        if (CreatedItemTypes.Contains(SlotInfo.ItemType))
        {
            UE_LOGFMT(LogInventoryUIContent, Warning, "{0} 타입의 ItemSlots가 이미 생성되었습니다. 중복 생성을 건너뜁니다.", *UEnum::GetValueAsString(SlotInfo.ItemType));
            continue;
        }
        //@Item Slots 블루프린트 클래스
        if (!SlotInfo.ItemSlotsClass)
        {
            UE_LOGFMT(LogInventoryUIContent, Warning, "{0} 타입의 ItemSlotsClass가 설정되지 않았습니다.", *UEnum::GetValueAsString(SlotInfo.ItemType));
            continue;
        }
        //@Create Widget
        UItemSlots* NewItemSlots = CreateWidget<UItemSlots>(this, SlotInfo.ItemSlotsClass);
        if (!NewItemSlots)
        {
            UE_LOGFMT(LogInventoryUIContent, Error, "{0} 타입의 ItemSlots 생성에 실패했습니다.", *UEnum::GetValueAsString(SlotInfo.ItemType));
            continue;
        }
        //@Set Item Type
        NewItemSlots->SetItemType(SlotInfo.ItemType);
        //@TMap
        MItemSlots.Add(SlotInfo.ItemType, NewItemSlots);
        //@비동기 초기화 이벤트
        RequestStartInitByInventoryUIContent.AddUFunction(NewItemSlots, "InitializeItemSlots");
        //@내부 바인딩
        InternalBindingToItemSlots(NewItemSlots);
        //@Item Slots Overlay
        if (UOverlaySlot* OverlaySlot = ItemSlotsOverlay->AddChildToOverlay(NewItemSlots))
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
            OverlaySlot->SetVerticalAlignment(VAlign_Fill);
        }

        CreatedItemTypes.Add(SlotInfo.ItemType);

        UE_LOGFMT(LogInventoryUIContent, Log, "{0} 타입의 ItemSlots가 성공적으로 생성되고 ItemSlotsOverlay에 추가되었습니다.",
            *UEnum::GetValueAsString(SlotInfo.ItemType));
    }

    //@Debugging
    for (uint8 i = 0; i < static_cast<uint8>(EItemType::MAX); ++i)
    {
        EItemType ItemType = static_cast<EItemType>(i);
        if (!CreatedItemTypes.Contains(ItemType))
        {
            UE_LOGFMT(LogInventoryUIContent, Warning, "{0} 타입의 ItemSlots가 생성되지 않았습니다.", *UEnum::GetValueAsString(ItemType));
        }
    }
}

void UInventoryUIContent::CreateItemDescription()
{
    //@ItemDescriptionOverlay, ItemDescriptionSlot 블루프린트 클래스
    if (!ensureMsgf(ItemDescriptionSlotClass && ItemDescriptionOverlay,
        TEXT("ItemDescriptionSlotClass 또는 ItemDescriptionOverlay가 유효하지 않습니다.")))
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "아이템 설명 UI 생성에 필요한 클래스 또는 오버레이가 설정되지 않았습니다.");
        return;
    }

    //@Create Widget
    UItemDescriptionSlot* ItemDescription = CreateWidget<UItemDescriptionSlot>(this, ItemDescriptionSlotClass);
    if (!IsValid(ItemDescription))
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ItemDescriptionSlot 위젯 생성에 실패했습니다.");
        return;
    }

    //@비동기 초기화 이벤트에 바인딩
    RequestStartInitByInventoryUIContent.AddUFunction(ItemDescription, "InitializeItemDescriptionSlot");

    //@Item Slots의 바인딩 준비 완료 이벤트에 바인딩(Item Description에서 직접적으로 가져오기가 빡셈)
    ItemSlotsReadyForBinding.AddUFunction(ItemDescription, "OnItemSlotsReadyForBinding");

    //@내부 바인딩
    InternalBindingToItemDescription(ItemDescription);

    //@Add Child To Overlay
    UOverlaySlot* OverlaySlot = ItemDescriptionOverlay->AddChildToOverlay(ItemDescription);
    if (!OverlaySlot)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ItemDescriptionSlot을 ItemDescriptionOverlay에 추가하는데 실패했습니다.");
        return;
    }

    //@Alignment
    OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
    OverlaySlot->SetVerticalAlignment(VAlign_Fill);

    UE_LOGFMT(LogInventoryUIContent, Log, "ItemDescriptionSlot이 성공적으로 생성되고 ItemDescriptionOverlay에 추가되었습니다.");
}

void UInventoryUIContent::UpdateAllItemSlotsVisibility()
{
    for (const auto& Pair : MItemSlots)
    {
        //@Visibility
        SetItemTypeVisibility(Pair.Key, Pair.Key == CurrentItemType);
    }
}

void UInventoryUIContent::SetItemTypeVisibility(EItemType ItemType, bool bVisible)
{
    //@Visibility: SelfHitTest 나타내기, Collapsed로 숨기기
    if (UUserWidget* Widget = GetItemSlotsUI(ItemType))
    {
        Widget->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInventoryUIContent::OnInventoryToolBarInitFinished()
{
    bInventoryToolBarReady = true;

    CheckInventoryUIContentInitialization();
}

void UInventoryUIContent::OnInventoryItemSlotsInitFinished()
{
    // 모든 ItemSlots가 초기화되었는지 확인
    bInventoryItemSlotsReady = true;

    CheckInventoryUIContentInitialization();

    // ItemSlotsOverlay에서 모든 ItemSlots 위젯 찾기
    for (UWidget* Child : ItemSlotsOverlay->GetAllChildren())
    {
        if (UItemSlots* ItemSlotsWidget = Cast<UItemSlots>(Child))
        {
            ItemSlotsReadyForBinding.Broadcast(this);
            break;
        }
    }
}

void UInventoryUIContent::OnInventoryItemDescriptionInitFinished()
{
    //@bInventoryItemDescriptionReady
    bInventoryItemDescriptionReady = true;

    //@Check 함수
    CheckInventoryUIContentInitialization();
}

void UInventoryUIContent::OnRequestCancelItemSlotsFocus()
{
    //@Set Focus
    SetFocus();
}

void UInventoryUIContent::OnInventoryToolBarButtonClicked(EItemType ItemType)
{
    //@EItemType
    if (CurrentItemType == ItemType)
    {
        return;
    }

    //@Current Item Type
    CurrentItemType = ItemType;

    //@Update Item Slots Visibility
    UpdateAllItemSlotsVisibility();

    UE_LOGFMT(LogInventoryUIContent, Log, "아이템 타입이 {0}(으)로 변경되었습니다.",
        *UEnum::GetValueAsString(ItemType));

    //@Set Focus
    SetFocus();

    // TODO: 필요한 경우 추가 로직 구현
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UUserWidget* UInventoryUIContent::GetItemSlotsUI(EItemType ItemType) const
{
    if (auto FoundWidget = MItemSlots.Find(ItemType))
    {
        return *FoundWidget;
    }
    return nullptr;
}

TArray<UItemSlots*> UInventoryUIContent::GetAllItemTypesItemSlots() const
{
    TArray<UItemSlots*> AllItemSlots;

    for (const auto& Pair : MItemSlots)
    {
        if (UItemSlots* ItemSlots = Cast<UItemSlots>(Pair.Value))
        {
            AllItemSlots.Add(ItemSlots);
        }
    }

    return AllItemSlots;
}
#pragma endregion