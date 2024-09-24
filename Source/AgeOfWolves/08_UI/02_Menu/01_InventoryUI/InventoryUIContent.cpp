#include "InventoryUIContent.h"
#include "Logging/StructuredLog.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryToolBar.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemDescriptionSlot.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogInventoryUIContent)

#pragma region Default Setting
UInventoryUIContent::UInventoryUIContent(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

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
}

void UInventoryUIContent::NativeDestruct()
{
    Super::NativeDestruct();
}

void UInventoryUIContent::InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar)
{
    //@Tool Bar
    if (!ToolBar)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }

    //@초기화 완료 이벤트
    ToolBar->InventoryToolBarInitFinished.BindUFunction(this, "OnInventoryToolBarInitFinished");
    //@버튼 클릭 이벤트
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

        //@Reset
        ResetInventoryUIContent();

        //@Inventory UI Content 초기화 완료 이벤트 호출
        InventoryUIContentInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

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

        //@ItemSlot_

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
    UInventoryToolBar* ToolBar = CreateWidget<UInventoryToolBar>(this, InventoryToolBarClass);
    if (!IsValid(ToolBar))
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "InventoryToolBar 위젯 생성에 실패했습니다.");
        return;
    }
    //@비동기 초기화 이벤트
    RequestStartInitByInventoryUIContent.AddUFunction(ToolBar, "InitializeInventoryToolBar");
    //@내부 바인딩
    InternalBindingToInventoryToolBar(ToolBar);
    //@Tool Bar Overlay
    if (UOverlaySlot* OverlaySlot = ToolBarOverlay->AddChildToOverlay(ToolBar))
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
    for (const FItemSlotsInfo& SlotInfo : ItemSlots)
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

    // TODO: 필요한 경우 추가 로직 구현
}
#pragma endregion

#pragma region Utility
UUserWidget* UInventoryUIContent::GetItemSlotsUI(EItemType ItemType) const
{
    if (auto FoundWidget = MItemSlots.Find(ItemType))
    {
        return *FoundWidget;
    }
    return nullptr;
}
#pragma endregion
