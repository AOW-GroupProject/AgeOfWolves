#include "InteractableItemSlot.h"
#include "Logging/StructuredLog.h"
#include "Components/Overlay.h"
#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogInteractableItemSlot)

//@Default Settings
#pragma region Default Setting
UInteractableItemSlot::UInteractableItemSlot(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ItemSlotButton = nullptr;
}

void UInteractableItemSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@블루프린트에서 바인딩된 버튼 검증 및 이벤트 연결
    if (!ItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "ItemSlotButton이 블루프린트에 바인딩되지 않았습니다!");
        return;
    }

    //@내부 바인딩
    InternalBindToItemSlotButton();

    UE_LOGFMT(LogInteractableItemSlot, Log, "ItemSlotButton이 성공적으로 바인딩되었습니다.");
}

void UInteractableItemSlot::NativePreConstruct()
{
    Super::NativePreConstruct();
    SetIsFocusable(false);
}

void UInteractableItemSlot::NativeConstruct()
{
    Super::NativeConstruct();

    auto RootWidget = GetRootWidget();
    if (RootWidget)
    {
        RootWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        UE_LOGFMT(LogInteractableItemSlot, Log, "RootWidget 가시성이 SelfHitTestInvisible로 설정되었습니다.");
    }
}

void UInteractableItemSlot::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UInteractableItemSlot::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);
}

void UInteractableItemSlot::InternalBindToItemSlotButton()
{
    if (!ItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "ItemSlotButton이 유효하지 않습니다. 바인딩을 수행할 수 없습니다.");
        return;
    }

    //@델리게이트 바인딩
    ItemSlotButton->ButtonHovered.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonHovered);
    ItemSlotButton->ButtonUnhovered.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonUnhovered);
    ItemSlotButton->ButtonSelected.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonClicked);

    //@선택 취소 이벤트 바인딩
    NotifyItemSlotButtonCanceled.AddUFunction(ItemSlotButton, "CancelSelectedButton");

    UE_LOGFMT(LogInteractableItemSlot, Log, "ItemSlotButton 이벤트가 성공적으로 바인딩되었습니다.");
}

void UInteractableItemSlot::InitializeItemSlot()
{
    //@부모 초기화
    Super::InitializeItemSlot();

    //@초기 상태: 비활성화
    DeactivateItemSlotInteraction();

    UE_LOGFMT(LogInteractableItemSlot, Log, "상호작용 가능한 아이템 슬롯이 초기화되었습니다.");
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
void UInteractableItemSlot::AssignNewItem_Implementation(const FGuid& ID, FItemInformation ItemInformation, int32 ItemCount)
{
    Super::AssignNewItem_Implementation(ID, ItemInformation, ItemCount);
    ActivateItemSlotInteraction();
}

void UInteractableItemSlot::AssignNewItemFromSlot_Implementation(UItemSlot* FromSlot)
{
    Super::AssignNewItemFromSlot_Implementation(FromSlot);
    ActivateItemSlotInteraction();
}

void UInteractableItemSlot::UpdateItemCount_Implementation(int32 NewCount)
{
    Super::UpdateItemCount_Implementation(NewCount);
}

void UInteractableItemSlot::ClearAssignedItem_Implementation(bool bForceClear)
{
    Super::ClearAssignedItem_Implementation(bForceClear);
    DeactivateItemSlotInteraction();
}

void UInteractableItemSlot::ActivateItemSlotInteraction()
{
    if (!ItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Warning, "ItemSlotButton을 찾을 수 없습니다. ID: {0}", UniqueItemID.ToString());
        return;
    }

    ItemSlotButton->ActivateButton();
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 활성화되었습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::DeactivateItemSlotInteraction()
{
    if (!ItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Warning, "ItemSlotButton을 찾을 수 없습니다. ID: {0}", UniqueItemID.ToString());
        return;
    }

    ItemSlotButton->DeactivateButton(false);
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 비활성화되었습니다. ID: {0}", UniqueItemID.ToString());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInteractableItemSlot::OnItemSlotButtonHovered_Implementation(EInteractionMethod InteractionMethodType)
{
    ItemSlotButtonHovered.Broadcast(UniqueItemID, InteractionMethodType);
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 호버. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonClicked_Implementation(EInteractionMethod InteractionMethodType)
{
    ItemSlotButtonClicked.Broadcast(UniqueItemID, InteractionMethodType);
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 클릭. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonUnhovered_Implementation()
{
    ItemSlotButtonUnhovered.Broadcast(UniqueItemID);
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 언호버. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::ItemSlotButtonCanceledNotified_Implementation(const FGuid& ItemID)
{
    if (ItemID != UniqueItemID) return;

    NotifyItemSlotButtonCanceled.Broadcast();
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 선택 취소. ID: {0}", ItemID.ToString());
}
#pragma endregion