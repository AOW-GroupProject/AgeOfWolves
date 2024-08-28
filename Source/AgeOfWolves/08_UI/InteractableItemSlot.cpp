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
    //@Item Slot Button
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

    //@내부 바인딩
    InternalBindToItemSlotButton(ItemSlotButton);

    //@Normal Style
    NormalStyle = FButtonStyle();
    NormalStyle.Normal.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
    NormalStyle.Normal.DrawAs = ESlateBrushDrawType::Image;

    //@Hovered Style
    HoveredStyle = FButtonStyle();
    HoveredStyle.Normal.SetResourceObject(ButtonFocusImage.LoadSynchronous());
    HoveredStyle.Normal.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
    HoveredStyle.Normal.DrawAs = ESlateBrushDrawType::Image;

    //@Selected Style
    SelectedStyle = FButtonStyle();
    SelectedStyle.Normal.SetResourceObject(ButtonFocusImage.LoadSynchronous());
    SelectedStyle.Normal.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
    SelectedStyle.Normal.DrawAs = ESlateBrushDrawType::Image;

    //@모든 스타일의 다른 상태(Hovered, Pressed 등)도 Normal과 동일하게 설정
    for (FButtonStyle* Style : { &NormalStyle, &HoveredStyle, &SelectedStyle })
    {
        Style->Hovered = Style->Normal;
        Style->Pressed = Style->Normal;
        Style->Disabled = Style->Normal;
    }

    //@Current Button State
    SetButtonState(EItemSlotButtonState::Normal);

    //@Item Slot Button 상호작용 비활성화
    DeactivateItemSlotInteraction();

    UE_LOGFMT(LogInteractableItemSlot, Log, "버튼이 생성되고 이벤트가 바인딩되었습니다.");
}

void UInteractableItemSlot::CreateDropDownMenu()
{
    //@TODO: Drop Down Menu의 초기화 작업 수행...

}

void UInteractableItemSlot::SetButtonState(EItemSlotButtonState NewState)
{
    if (CurrentButtonState != NewState)
    {
        //@Current Button State
        CurrentButtonState = NewState;
        //@Update Button Style
        UpdateButtonStyle(CurrentButtonState);
    }
}


void UInteractableItemSlot::UpdateButtonStyle(EItemSlotButtonState NewState)
{
    if (!ItemSlotButton) return;

    switch (NewState)
    {
    case EItemSlotButtonState::Normal:
        ItemSlotButton->SetStyle(NormalStyle);
        break;
    case EItemSlotButtonState::Hovered:
        ItemSlotButton->SetStyle(HoveredStyle);
        break;
    case EItemSlotButtonState::Selected:
        ItemSlotButton->SetStyle(SelectedStyle);
        break;
    }

    UE_LOGFMT(LogInteractableItemSlot, Log, "버튼 스타일이 {0} 상태로 업데이트되었습니다.", *UEnum::GetValueAsString(NewState));
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
    if (!ItemSlotButton->GetIsEnabled())
    {
        UE_LOGFMT(LogInteractableItemSlot, Warning, "아이템 슬롯 버튼이 이미 비활성화 상태입니다. 선택이 무시됩니다. ID: {0}", UniqueItemID.ToString());
        return;
    }

    //@Current Button State
    SetButtonState(EItemSlotButtonState::Selected);
    //@Item Slot Button의 비 활성화
    DeactivateItemSlotInteraction();
    //@Item Slot Button의 클릭 이벤트
    ItemSlotButtonClicked.Broadcast(UniqueItemID);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 성공적으로 선택되었습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonHovered_Implementation()
{
    if (CurrentButtonState != EItemSlotButtonState::Normal)
    {
        return;
    }
    //@Current Button State
    SetButtonState(EItemSlotButtonState::Hovered);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에 마우스가 올라갔습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonUnhovered_Implementation()
{
    if (CurrentButtonState != EItemSlotButtonState::Hovered)
    {
        return;
    }

    //@Current Button State
    SetButtonState(EItemSlotButtonState::Normal);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에서 마우스가 벗어났습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonCanceled_Implementation(const FGuid& ItemID)
{
    if (ItemID != UniqueItemID || ItemSlotButton->GetIsEnabled())
    {
        return;
    }
    //@Current Button State
    SetButtonState(EItemSlotButtonState::Normal);
    //@Item Slot Button의 상호작용 활성화
    ActivateItemSlotInteraction();

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 선택이 취소되었습니다. ID: {0}", ItemID.ToString());
}
#pragma endregion
