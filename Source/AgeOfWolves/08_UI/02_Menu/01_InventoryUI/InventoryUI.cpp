#include "InventoryUI.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryToolBar.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemDescriptionSlot.h"
#include "08_UI/InteractableItemSlot.h"

DEFINE_LOG_CATEGORY(LogInventoryUI)

//@Defualt Setting
#pragma region Default Setting
UInventoryUI::UInventoryUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    //@Menu Category
    MenuCategory = EMenuCategory::Inventory;
    InventoryToolBar = nullptr;
}

void UInventoryUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@가시성 변화 이벤트에 바인딩
    OnVisibilityChanged.AddDynamic(this, &UInventoryUI::OnUIVisibilityChanged);

    //@외부 바인딩
}

void UInventoryUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UInventoryUI::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);
}

void UInventoryUI::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UInventoryUI::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    //@Navigation 방향에 따라 처리
    switch (InNavigationEvent.GetNavigationType())
    {
    case EUINavigation::Down:
    {
        //@Down 무시
        UE_LOGFMT(LogInventoryUI, Log, "Navigation Down ignored in InventoryUI.");
        return FNavigationReply::Explicit(nullptr);
    }
    case EUINavigation::Up:
    {
        //@Up 무시
        UE_LOGFMT(LogInventoryUI, Log, "Navigation Up ignored in InventoryUI.");
        return FNavigationReply::Explicit(nullptr);
    }
    case EUINavigation::Left:
    case EUINavigation::Right:
    {
        //@좌우 방향키는 ToolBar에서 처리
        if (InventoryToolBar && InventoryToolBar->HasKeyboardFocus())
        {
            //@Navigation을 ToolBar로 전달
            return InDefaultReply;
        }
        break;
    }
    default:
        break;
    }

    //@기본적으로 Navigation 차단
    return FNavigationReply::Explicit(nullptr);
}

FReply UInventoryUI::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    //@SetDirectly(SetFocus())를 통한 포커스 시도 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return FReply::Handled().ClearUserFocus();
    }

    UE_LOGFMT(LogInventoryUI, Log, "포커스 : 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

    //@최초 포커스 시 InventoryToolBar에게 포커스 전달
    if (InventoryToolBar)
    {
        //@ToolBar에게 포커스 전달
        InventoryToolBar->SetFocus();
        UE_LOGFMT(LogInventoryUI, Log, "InventoryUI가 포커스를 받았습니다. InventoryToolBar에게 포커스를 전달합니다.");
    }
    else
    {
        UE_LOGFMT(LogInventoryUI, Warning, "InventoryToolBar가 유효하지 않아 포커스를 전달할 수 없습니다.");
    }

    return FReply::Handled();
}

void UInventoryUI::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    //@SetDirectly(SetFocus())를 통한 포커스 소실 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        SetFocus();
        return;
    }

    Super::NativeOnFocusLost(InFocusEvent);

    UE_LOGFMT(LogInventoryUI, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
}

FReply UInventoryUI::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogInventoryUI, Log, "키 입력 감지됨: {0}", *Key.ToString());

    //@Inventory Tool Bar
    if (!InventoryToolBar)
    {
        UE_LOGFMT(LogInventoryUI, Error, "InventoryToolBar를 찾을 수 없습니다.");
        return FReply::Unhandled();
    }

    //@Space Bar: ItemSlots로 포커스 이동 (기존 Enter 동작)
    if (Key == EKeys::Enter)
    {
        //@Current Item Slots
        UItemSlots* CurrentItemSlots = Cast<UItemSlots>(GetItemSlotsUI(CurrentItemType));
        if (!CurrentItemSlots)
        {
            UE_LOGFMT(LogInventoryUI, Error, "{0} 타입의 ItemSlots를 찾을 수 없습니다.", *UEnum::GetValueAsString(CurrentItemType));
            return FReply::Handled();
        }

        //@첫 번째 아이템 슬롯 유효한가?
        UInteractableItemSlot* FirstItemSlot = CurrentItemSlots->FindFirstItemSlot();
        if (!FirstItemSlot || !FirstItemSlot->GetUniqueItemID().IsValid())
        {
            UE_LOGFMT(LogInventoryUI, Warning, "{0} 타입의 첫 번째 Item Slot이 유효하지 않습니다.", *UEnum::GetValueAsString(CurrentItemType));
            return FReply::Handled();
        }

        //@첫 번째 아이템 슬롯의 강제 호버 상태 전환 요청 이벤트
        RequestFirstItemSlotHover.Broadcast(CurrentItemType);

        //@SetFocus
        CurrentItemSlots->SetFocus();

        UE_LOGFMT(LogInventoryUI, Log, "Space Bar: 포커스가 {0} 타입의 ItemSlots로 이동했습니다.", *UEnum::GetValueAsString(CurrentItemType));

        return FReply::Handled();
    }

    //@Escape: ItemSlots에 포커스가 있으면 여기서 소비하고 ToolBar로 포커스 반환, ToolBar에 있으면 상위(MenuUI)로 전달
    if (Key == EKeys::Escape)
    {
        UItemSlots* CurrentItemSlots = Cast<UItemSlots>(GetItemSlotsUI(CurrentItemType));
        if (CurrentItemSlots && CurrentItemSlots->HasKeyboardFocus())
        {
            //@현재 Hovered Item Slot 취소 요청
            RequestCancelCurrentHoveredItemSlot.Broadcast(CurrentItemType);

            //@ToolBar로 포커스 반환
            if (InventoryToolBar)
            {
                InventoryToolBar->SetFocus();
            }

            UE_LOGFMT(LogInventoryUI, Log, "Escape: ItemSlots에서 포커스를 회수하여 ToolBar로 전달하고 이벤트를 소비합니다.");
            return FReply::Handled();
        }

        //@Toolbar가 포커스를 가진 경우: MenuUI에서 처리하도록 위임
        if (InventoryToolBar && InventoryToolBar->HasKeyboardFocus())
        {
            return FReply::Unhandled();
        }

        //@그 외: 상위로 전달
        return FReply::Unhandled();
    }

    UE_LOGFMT(LogInventoryUI, Log, "Inventory UI에서 처리하지 않는 키 입력: {0}", *Key.ToString());
    return FReply::Unhandled();
}

void UInventoryUI::InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar)
{
    //@Tool Bar
    if (!ToolBar)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }

    //@내부 바인딩
    ToolBar->ToolBarInitFinished.BindUFunction(this, "OnInventoryToolBarInitFinished");
    ToolBar->InventoryToolBarButtonClicked.BindUFunction(this, "OnInventoryToolBarButtonClicked");
}

void UInventoryUI::InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget)
{
    //@Item Slots
    if (!ItemSlotsWidget)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemSlots UI가 유효하지 않습니다.");
        return;
    }
    //@초기화 완료 이벤트
    ItemSlotsWidget->ItemSlotsInitFinished.BindUFunction(this, "OnInventoryItemSlotsInitFinished");
    ItemSlotsWidget->RequestCancelItemSlotsFocus.BindUFunction(this, "OnRequestCancelItemSlotsFocus");
}

void UInventoryUI::InternalBindingToItemDescription(UItemDescriptionSlot* ItemDescription)
{
    //@Item Description
    if (!ItemDescription)
    {
        UE_LOGFMT(LogInventoryUI, Error, "Item Description UI가 유효하지 않습니다.");
        return;
    }
    //@초기화 완료 이벤트
    ItemDescription->ItemDescriptionSlotInitFinished.BindUFunction(this, "OnInventoryItemDescriptionInitFinished");
}

void UInventoryUI::InitializeMenuUIContent()
{
    //@Inventory Tool Bar
    CreateToolBar();
    //@Item Slots 
    CreateAllItemSlots();
    //@Item Description
    CreateItemDescription();
    //@초기화 요청 이벤트 호출
    RequestStartInitByInventoryUI.Broadcast();

    //@Super
    Super::InitializeMenuUIContent();
}

void UInventoryUI::CheckInventoryUIInitialization()
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
        ResetInventoryUI();

        //@Inventory UI 초기화 완료 체크
        bInventoryUIContentReady = true;
        CheckMenuUIContentInitFinished();
    }
}

void UInventoryUI::CheckMenuUIContentInitFinished()
{
    if (bInventoryUIContentReady)
    {
        UE_LOGFMT(LogInventoryUI, Log, "InventoryUI의 모든 서브위젯 초기화가 완료되었습니다.");

        //@Inventory UI Content Ready
        bInventoryUIContentReady = false;

        //@Super
        Super::CheckMenuUIContentInitFinished();
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
void UInventoryUI::ResetMenuUIContent()
{
    ResetInventoryUI();
    Super::ResetMenuUIContent();
}

void UInventoryUI::ResetInventoryUI()
{
    UE_LOGFMT(LogInventoryUI, Log, "Inventory UI 구성 위젯들을 초기 상태로 리셋합니다.");

    //@Current Item Type
    if (CurrentItemType != EItemType::MAX)
    {
        //@Item Slots
        UUserWidget* ItemSlotsUI = GetItemSlotsUI(CurrentItemType);
        if (UItemSlots* ItemSlotsWidget = Cast<UItemSlots>(ItemSlotsUI))
        {
            ItemSlotsWidget->ResetItemSlots();
            UE_LOGFMT(LogInventoryUI, Log, "현재 타입({0})의 Item Slots가 초기화되었습니다.", *UEnum::GetValueAsString(CurrentItemType));
        }
        else
        {
            UE_LOGFMT(LogInventoryUI, Warning, "현재 타입({0})의 Item Slots를 찾을 수 없거나 초기화하지 못했습니다.", *UEnum::GetValueAsString(CurrentItemType));
        }

        //@Item Description
        bool bItemDescriptionReset = false;
        for (auto OverlaySlot : ItemDescriptionOverlay->GetAllChildren())
        {
            if (UItemDescriptionSlot* ItemSlotWidget = Cast<UItemDescriptionSlot>(OverlaySlot))
            {
                ItemSlotWidget->ResetItemDescriptionSlot();
                bItemDescriptionReset = true;
                UE_LOGFMT(LogInventoryUI, Log, "Item Description Slot이 초기화되었습니다.");
                break;
            }
        }
        if (!bItemDescriptionReset)
        {
            UE_LOGFMT(LogInventoryUI, Warning, "Item Description Slot을 찾을 수 없거나 초기화하지 못했습니다.");
        }
    }

    //@Inventory Toolbar
    bool bToolbarReset = false;
    for (auto OverlaySlot : ToolBarOverlay->GetAllChildren())
    {
        UE_LOGFMT(LogInventoryUI, Warning, "{0}", *OverlaySlot->GetName());

        if (UInventoryToolBar* ToolBar = Cast<UInventoryToolBar>(OverlaySlot))
        {
            ToolBar->ResetToolBar();

            bToolbarReset = true;

            UE_LOGFMT(LogInventoryUI, Log, "Inventory Toolbar가 초기화되었습니다.");
            break;
        }
    }
    if (!bToolbarReset)
    {
        UE_LOGFMT(LogInventoryUI, Warning, "Inventory Toolbar를 찾을 수 없거나 초기화하지 못했습니다.");
    }

    UE_LOGFMT(LogInventoryUI, Log, "Inventory UI 구성 위젯들의 초기화가 완료되었습니다.");
}

void UInventoryUI::CreateToolBar()
{
    if (!ensureMsgf(InventoryToolBarClass && ToolBarOverlay, TEXT("InventoryToolBarClass 또는 ToolBarOverlay가 유효하지 않습니다.")))
    {
        return;
    }
    //@Tool Bar
    InventoryToolBar = CreateWidget<UInventoryToolBar>(this, InventoryToolBarClass);
    if (!IsValid(InventoryToolBar))
    {
        UE_LOGFMT(LogInventoryUI, Error, "InventoryToolBar 위젯 생성에 실패했습니다.");
        return;
    }
    //@비동기 초기화 이벤트
    RequestStartInitByInventoryUI.AddUFunction(InventoryToolBar, "InitializeToolBar");
    //@내부 바인딩
    InternalBindingToInventoryToolBar(InventoryToolBar);
    //@Tool Bar Overlay
    if (UOverlaySlot* OverlaySlot = ToolBarOverlay->AddChildToOverlay(InventoryToolBar))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    UE_LOGFMT(LogInventoryUI, Log, "InventoryToolBar가 성공적으로 생성되고 ToolBarOverlay에 추가되었습니다.");
}

void UInventoryUI::CreateAllItemSlots()
{
    //@Item Slots Overlay
    if (!IsValid(ItemSlotsOverlay))
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemSlotsOverlay가 유효하지 않습니다.");
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
            UE_LOGFMT(LogInventoryUI, Warning, "{0} 타입의 ItemSlots가 이미 생성되었습니다. 중복 생성을 건너뜁니다.", *UEnum::GetValueAsString(SlotInfo.ItemType));
            continue;
        }
        //@Item Slots 블루프린트 클래스
        if (!SlotInfo.ItemSlotsClass)
        {
            UE_LOGFMT(LogInventoryUI, Warning, "{0} 타입의 ItemSlotsClass가 설정되지 않았습니다.", *UEnum::GetValueAsString(SlotInfo.ItemType));
            continue;
        }
        //@Create Widget
        UItemSlots* NewItemSlots = CreateWidget<UItemSlots>(this, SlotInfo.ItemSlotsClass);
        if (!NewItemSlots)
        {
            UE_LOGFMT(LogInventoryUI, Error, "{0} 타입의 ItemSlots 생성에 실패했습니다.", *UEnum::GetValueAsString(SlotInfo.ItemType));
            continue;
        }
        //@Set Item Type
        NewItemSlots->SetItemType(SlotInfo.ItemType);
        //@TMap
        MItemSlots.Add(SlotInfo.ItemType, NewItemSlots);
        //@비동기 초기화 이벤트
        RequestStartInitByInventoryUI.AddUFunction(NewItemSlots, "InitializeItemSlots");
        //@내부 바인딩
        InternalBindingToItemSlots(NewItemSlots);
        //@Item Slots Overlay
        if (UOverlaySlot* OverlaySlot = ItemSlotsOverlay->AddChildToOverlay(NewItemSlots))
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
            OverlaySlot->SetVerticalAlignment(VAlign_Fill);
        }

        CreatedItemTypes.Add(SlotInfo.ItemType);

        UE_LOGFMT(LogInventoryUI, Log, "{0} 타입의 ItemSlots가 성공적으로 생성되고 ItemSlotsOverlay에 추가되었습니다.",
            *UEnum::GetValueAsString(SlotInfo.ItemType));
    }

    //@Debugging
    for (uint8 i = 0; i < static_cast<uint8>(EItemType::MAX); ++i)
    {
        EItemType ItemType = static_cast<EItemType>(i);
        if (!CreatedItemTypes.Contains(ItemType))
        {
            UE_LOGFMT(LogInventoryUI, Warning, "{0} 타입의 ItemSlots가 생성되지 않았습니다.", *UEnum::GetValueAsString(ItemType));
        }
    }
}

void UInventoryUI::CreateItemDescription()
{
    //@ItemDescriptionOverlay, ItemDescriptionSlot 블루프린트 클래스
    if (!ensureMsgf(ItemDescriptionSlotClass && ItemDescriptionOverlay,
        TEXT("ItemDescriptionSlotClass 또는 ItemDescriptionOverlay가 유효하지 않습니다.")))
    {
        UE_LOGFMT(LogInventoryUI, Error, "아이템 설명 UI 생성에 필요한 클래스 또는 오버레이가 설정되지 않았습니다.");
        return;
    }

    //@Create Widget
    UItemDescriptionSlot* ItemDescription = CreateWidget<UItemDescriptionSlot>(this, ItemDescriptionSlotClass);
    if (!IsValid(ItemDescription))
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemDescriptionSlot 위젯 생성에 실패했습니다.");
        return;
    }

    //@비동기 초기화 이벤트에 바인딩
    RequestStartInitByInventoryUI.AddUFunction(ItemDescription, "InitializeItemDescriptionSlot");

    //@Item Slots의 바인딩 준비 완료 이벤트에 바인딩(Item Description에서 직접적으로 가져오기가 빡셈)
    ItemSlotsReadyForBinding.AddUFunction(ItemDescription, "OnItemSlotsReadyForBinding");

    //@내부 바인딩
    InternalBindingToItemDescription(ItemDescription);

    //@Add Child To Overlay
    UOverlaySlot* OverlaySlot = ItemDescriptionOverlay->AddChildToOverlay(ItemDescription);
    if (!OverlaySlot)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemDescriptionSlot을 ItemDescriptionOverlay에 추가하는데 실패했습니다.");
        return;
    }

    //@Alignment
    OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
    OverlaySlot->SetVerticalAlignment(VAlign_Fill);

    UE_LOGFMT(LogInventoryUI, Log, "ItemDescriptionSlot이 성공적으로 생성되고 ItemDescriptionOverlay에 추가되었습니다.");
}

void UInventoryUI::UpdateAllItemSlotsVisibility()
{
    for (const auto& Pair : MItemSlots)
    {
        //@Visibility
        SetItemTypeVisibility(Pair.Key, Pair.Key == CurrentItemType);
    }
}

void UInventoryUI::SetItemTypeVisibility(EItemType ItemType, bool bVisible)
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
void UInventoryUI::OnInventoryToolBarInitFinished()
{
    bInventoryToolBarReady = true;

    CheckInventoryUIInitialization();
}

void UInventoryUI::OnInventoryItemSlotsInitFinished()
{
    // 모든 ItemSlots가 초기화되었는지 확인
    bInventoryItemSlotsReady = true;

    CheckInventoryUIInitialization();

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

void UInventoryUI::OnInventoryItemDescriptionInitFinished()
{
    //@bInventoryItemDescriptionReady
    bInventoryItemDescriptionReady = true;

    //@Check 함수
    CheckInventoryUIInitialization();
}

void UInventoryUI::OnRequestCancelItemSlotsFocus()
{
    //@Set Focus
    SetFocus();
}

void UInventoryUI::OnInventoryToolBarButtonClicked(EItemType ItemType)
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

    UE_LOGFMT(LogInventoryUI, Log, "아이템 타입이 {0}(으)로 변경되었습니다.",
        *UEnum::GetValueAsString(ItemType));

    //@Set Focus
    SetFocus();

    // TODO: 필요한 경우 추가 로직 구현
}

void UInventoryUI::OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType)
{    
    Super::OnUIVisibilityChanged_Implementation(VisibilityType);

    if (VisibilityType == ESlateVisibility::SelfHitTestInvisible)
    {
        //@초기 설정으로 Reset
        ResetMenuUIContent();
    }

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UUserWidget* UInventoryUI::GetItemSlotsUI(EItemType ItemType) const
{
    // 기존 맵에서 찾기 시도
    if (auto FoundWidget = MItemSlots.Find(ItemType))
    {
        return *FoundWidget;
    }

    // SpecUp 타입에 대한 특별한 처리
    if (ItemType == EItemType::SpecUp)
    {
        UE_LOGFMT(LogInventoryUI, Warning,
            "SpecUp 타입의 ItemSlots가 설정되지 않았습니다. 이는 ItemSlotInformations 배열에 SpecUp 설정이 누락되었기 때문일 수 있습니다.");

        // TODO: 향후 SpecUp 전용 UI가 필요하다면 여기서 동적 생성 고려
        return nullptr;
    }

    // 다른 타입들에 대한 일반적인 경고
    UE_LOGFMT(LogInventoryUI, Warning,
        "{0} 타입의 ItemSlots를 찾을 수 없습니다. ItemSlotInformations에서 해당 타입이 설정되었는지 확인하세요.",
        *UEnum::GetValueAsString(ItemType));

    return nullptr;
}

TArray<UItemSlots*> UInventoryUI::GetAllItemTypesItemSlots() const
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

UItemSlots* UInventoryUI::GetItemSlotsByType(EItemType ItemType) const
{
    if (UItemSlots* ItemSlotsWidget = CastChecked<UItemSlots>(GetItemSlotsUI(ItemType)))
    {
        return ItemSlotsWidget;
    }

    return nullptr;
}
#pragma endregion