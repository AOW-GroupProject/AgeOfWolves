#include "InteractableItemSlot.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Components/Overlay.h"

#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"

DEFINE_LOG_CATEGORY(LogInteractableItemSlot)

#pragma region Default Setting
UInteractableItemSlot::UInteractableItemSlot(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    ItemSlotButton = nullptr;
}

void UInteractableItemSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

}

void UInteractableItemSlot::NativePreConstruct()
{
    Super::NativePreConstruct();

}

void UInteractableItemSlot::NativeConstruct()
{
    Super::NativeConstruct();

    //@Root Widget
    auto RootWidget = GetRootWidget();
    if (RootWidget)
    {
        RootWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        UE_LOGFMT(LogInteractableItemSlot, Log, "ItemSlot: RootWidget 가시성이 Self Hit Test Invisible로 설정되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogInteractableItemSlot, Warning, "ItemSlot: RootWidget이 null입니다. 가시성을 설정할 수 없습니다.");
    }
}

void UInteractableItemSlot::NativeDestruct()
{
    Super::NativeDestruct();

}

void UInteractableItemSlot::InternalBindToItemSlotButton(UButton* Button)
{
    if (!Button)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "ItemSlotButton이 null입니다. 위젯 블루프린트에서 올바르게 바인딩되었는지 확인하세요.");
        return;
    }

    //@내부 바인딩
    Button->OnClicked.AddDynamic(this, &UInteractableItemSlot::OnItemSlotButtonSelected);
    Button->OnHovered.AddDynamic(this, &UInteractableItemSlot::OnItemSlotButtonHovered);
    Button->OnUnhovered.AddDynamic(this, &UInteractableItemSlot::OnItemSlotButtonUnhovered);
}

void UInteractableItemSlot::InitializeItemSlot()
{
    //@TODO: 상호작용 가능한 아이템 슬롯의 초기화 작업 아래에서 수행...

    //@Button
    CreateButton();
    //@PopUp UI
    CreateDropDownMenu();

    //Super, 초기화 요청 완료 이벤트 호출
    Super::InitializeItemSlot();

    UE_LOGFMT(LogInteractableItemSlot, Log, "상호작용 가능한 아이템 슬롯이 초기화되었습니다.");
}
#pragma endregion

#pragma region SubWidgets
void UInteractableItemSlot::CreateButton()
{
    if (!ItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "ItemSlotButton이 null입니다. 위젯 블루프린트에서 올바르게 바인딩되었는지 확인하세요.");
        return;
    }

    //@ButtonStyle
    OriginalButtonStyle = ItemSlotButton->GetStyle();

    //@내부 바인딩
    InternalBindToItemSlotButton(ItemSlotButton);

    UE_LOGFMT(LogInteractableItemSlot, Log, "버튼이 생성되고 이벤트가 바인딩되었습니다.");
}

void UInteractableItemSlot::CreateDropDownMenu()
{
    //@TODO: Drop Down Menu의 초기화 작업 수행...

}

void UInteractableItemSlot::UpdateButtonStyle(const FButtonStyle& NewStyle)
{
    if (ItemSlotButton)
    {
        ItemSlotButton->SetStyle(NewStyle);
    }
}

void UInteractableItemSlot::ActivateItemSlotInteraction()
{
    ItemSlotButton->SetIsEnabled(true);
}

void UInteractableItemSlot::DeactivateItemSlotInteraction()
{
    ItemSlotButton->SetIsEnabled(false);
}
#pragma endregion

#pragma region Callbacks
void UInteractableItemSlot::OnItemSlotButtonSelected_Implementation()
{
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 선택되었습니다.");

    //@Item Slot Button 상호작용 비활성화
    DeactivateItemSlotInteraction();
    //@버튼 클릭 이벤트 호출 
    ItemSlotButtonClicked.Broadcast(UniqueItemID);

}

void UInteractableItemSlot::OnItemSlotButtonHovered_Implementation()
{
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 위에 마우스가 올라갔습니다.");

    //@블루프린트에서 상세 구현 진행...
    //@예: 애니메이션...
}

void UInteractableItemSlot::OnItemSlotButtonUnhovered_Implementation()
{
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에서 마우스가 벗어났습니다.");

    //@블루프린트에서 상세 구현 진행...
    //@예: 애니메이션...

}

void UInteractableItemSlot::OnItemSlotButtonCanceled_Implementation(const FGuid& ItemID)
{
    //@FGuid
    if (ItemID != UniqueItemID)
    {
        return;
    }

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 선택이 취소되었습니다.");

    //@블루프린트에서 상세 구현 진행...
    //@예: 애니메이션...
}
#pragma endregion
