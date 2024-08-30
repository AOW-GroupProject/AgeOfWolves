#include "InteractableItemSlot.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Button.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogInteractableItemSlot)

#pragma region Default Setting
UInteractableItemSlot::UInteractableItemSlot(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ItemSlotButtonClass = UCustomButton::StaticClass();
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

void UInteractableItemSlot::InternalBindToItemSlotButton(UCustomButton* InItemSlotButton)
{
    if (!InItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "InItemSlotButton이 null입니다. 바인딩을 수행할 수 없습니다.");
        return;
    }

    //@Button의 상태 별 이벤트에 바인딩 아래에서 수행...
    //@내부 바인딩
    InItemSlotButton->ButtonHovered.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonHovered);
    InItemSlotButton->ButtonSelected.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonClicked);
    InItemSlotButton->ButtonUnhovered.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonUnhovered);

    UE_LOGFMT(LogInteractableItemSlot, Verbose, "CustomButton 이벤트가 성공적으로 바인딩되었습니다.");
}

void UInteractableItemSlot::InitializeItemSlot()
{
    //@CustomButton
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
    //@Slot Overlay
    if (!SlotOverlay)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "SlotOverlay가 null입니다. 위젯 블루프린트에서 올바르게 바인딩되었는지 확인하세요.");
        return;
    }

    //@ItemSlotButtonClass
    if (!ItemSlotButtonClass)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "ItemSlotButtonClass가 설정되지 않았습니다. 에디터에서 ItemSlotButtonClass를 설정해주세요.");
        return;
    }

    //@Create Widget
    UCustomButton* ItemSlotButton = CreateWidget<UCustomButton>(this, ItemSlotButtonClass);
    if (!ItemSlotButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "UCustomButton 위젯을 생성하지 못했습니다. ItemSlotButtonClass: {0}", *ItemSlotButtonClass->GetName());
        return;
    }


    //@선택 취소 이벤트 바인딩
    NotifyItemSlotButtonCanceled.AddUFunction(ItemSlotButton, "ButtonCanceledNotified");

    //@내부 바인딩
    InternalBindToItemSlotButton(ItemSlotButton);

    //@Alignment
    UOverlaySlot* OverlaySlot = SlotOverlay->AddChildToOverlay(ItemSlotButton);
    if (!OverlaySlot)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "CustomButton을 SlotOverlay에 추가하지 못했습니다.");
        return;
    }

    OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
    OverlaySlot->SetVerticalAlignment(VAlign_Fill);

    //@Item Slot Button 비활성화
    DeactivateItemSlotInteraction();

    UE_LOGFMT(LogInteractableItemSlot, Log, "UCustomButton({0})이 생성되고 이벤트가 바인딩되었습니다.", *ItemSlotButtonClass->GetName());
}

void UInteractableItemSlot::CreateDropDownMenu()
{
    //@TODO: Drop Down Menu의 초기화 작업 수행...

}

void UInteractableItemSlot::ActivateItemSlotInteraction()
{
    UCustomButton* CustomButton = GetItemSlotButton();
    if (!CustomButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Warning, "CustomButton을 찾을 수 없습니다. 버튼을 활성화할 수 없습니다. ID: {0}", UniqueItemID.ToString());
        return;
    }
    //@Activate Button
    CustomButton->ActivateButton();

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 활성화되었습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::DeactivateItemSlotInteraction()
{
    UCustomButton* CustomButton = GetItemSlotButton();
    if (!CustomButton)
    {
        UE_LOGFMT(LogInteractableItemSlot, Warning, "CustomButton을 찾을 수 없습니다. 버튼을 비활성화할 수 없습니다. ID: {0}", UniqueItemID.ToString());
        return;
    }
    //@Deactivate Button
    CustomButton->DeactivateButton();

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 비활성화되었습니다. ID: {0}", UniqueItemID.ToString());
}
#pragma endregion

#pragma region Callbacks
void UInteractableItemSlot::OnItemSlotButtonHovered()
{

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에 마우스가 올라갔습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonUnhovered()
{

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에서 마우스가 벗어났습니다. ID: {0}", UniqueItemID.ToString());
}

void UInteractableItemSlot::OnItemSlotButtonPressed()
{
    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 눌렸습니다. ID: {0}", UniqueItemID.ToString());

}

void UInteractableItemSlot::OnItemSlotButtonClicked()
{
    //@Item Slot Button 선택 이벤트
    ItemSlotButtonClicked.Broadcast(UniqueItemID);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 클릭되었습니다. ID: {0}", UniqueItemID.ToString());
}


void UInteractableItemSlot::OnItemSlotButtonCanceled(const FGuid& ItemID)
{
    if (ItemID != UniqueItemID)
    {
        return;
    }

    //@Item Slot Button 선택 취소 이벤트
    NotifyItemSlotButtonCanceled.Broadcast();

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 선택이 취소되었습니다. ID: {0}", ItemID.ToString());
}
#pragma endregion

#pragma region Utility
UCustomButton* UInteractableItemSlot::GetItemSlotButton() const
{
    //@Slot Overlay
    if (!SlotOverlay)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "SlotOverlay가 null입니다. CustomButton을 찾을 수 없습니다.");
        return nullptr;
    }
    //@Custom Button
    for (UWidget* Child : SlotOverlay->GetAllChildren())
    {
        if (UCustomButton* CustomButton = Cast<UCustomButton>(Child))
        {
            return CustomButton;
        }
    }

    return nullptr;
}
#pragma endregion
