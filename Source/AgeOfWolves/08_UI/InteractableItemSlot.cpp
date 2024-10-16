#include "InteractableItemSlot.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Button.h"
#include "Components/Image.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogInteractableItemSlot)

//@Default Settings
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

    SetIsFocusable(false);
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

FNavigationReply UInteractableItemSlot::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);

}

FReply UInteractableItemSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    // 모든 포커스 시도를 로깅
    UE_LOGFMT(LogInteractableItemSlot, Log, "포커스 시도: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

    // SetDirectly만 허용하고 나머지는 거부
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return FReply::Unhandled();
    }

    return FReply::Handled();
}

void UInteractableItemSlot::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{	
    // SetDirectly가 아닌 경우 포커스 유지
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        UE_LOGFMT(LogInteractableItemSlot, Log, "포커스 유지: 위젯: {0}, 원인: {1}",
            *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
        return;
    }

    Super::NativeOnFocusLost(InFocusEvent);

    UE_LOGFMT(LogInteractableItemSlot, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
}

FReply UInteractableItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply::Handled().PreventThrottling();
}

FReply UInteractableItemSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogInteractableItemSlot, Log, "키 입력 감지됨: {0}", *Key.ToString());

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
    InItemSlotButton->ButtonUnhovered.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonUnhovered);
    InItemSlotButton->ButtonSelected.AddUObject(this, &UInteractableItemSlot::OnItemSlotButtonClicked);

    UE_LOGFMT(LogInteractableItemSlot, Verbose, "CustomButton 이벤트가 성공적으로 바인딩되었습니다.");
}

void UInteractableItemSlot::InitializeItemSlot()
{
    //@CustomButton
    CreateButton();

    //@초기화 요청 이벤트
    Super::InitializeItemSlot();

    UE_LOGFMT(LogInteractableItemSlot, Log, "상호작용 가능한 아이템 슬롯이 초기화되었습니다.");
}
#pragma endregion

//@Property/Info...etc
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
    NotifyItemSlotButtonCanceled.AddUFunction(ItemSlotButton, "CancelSelectedButton");

    //@내부 바인딩
    InternalBindToItemSlotButton(ItemSlotButton);

    //@Remove Child, Slot Image의 Z-order를 가장 높게 설정하기 위해
    if (SlotImage)
    {
        SlotOverlay->RemoveChild(SlotImage);
    }

    //@Add Child To Overlay
    UOverlaySlot* OverlaySlot = SlotOverlay->AddChildToOverlay(ItemSlotButton);
    if (!OverlaySlot)
    {
        UE_LOGFMT(LogInteractableItemSlot, Error, "CustomButton을 SlotOverlay에 추가하지 못했습니다.");
        return;
    }

    //@Overlay Slot
    OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
    OverlaySlot->SetVerticalAlignment(VAlign_Fill);

    //@Add Child To Overlay, Slot Image 다시 추가
    if (SlotImage)
    {
        auto OverlayImageSlot = SlotOverlay->AddChildToOverlay(SlotImage);
        if (!OverlayImageSlot)
        {
            UE_LOGFMT(LogInteractableItemSlot, Error, "SlotImage를 SlotOverlay에 추가하지 못했습니다.");
            return;
        }
        OverlayImageSlot->SetHorizontalAlignment(HAlign_Fill);
        OverlayImageSlot->SetVerticalAlignment(VAlign_Fill);
    }

    //@Item Slot Button 비활성화
    DeactivateItemSlotInteraction();

    UE_LOGFMT(LogInteractableItemSlot, Log, "UCustomButton({0})이 생성되고 이벤트가 바인딩되었습니다.", *ItemSlotButtonClass->GetName());
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

//@Callbacks
#pragma region Callbacks
void UInteractableItemSlot::OnItemSlotButtonHovered_Implementation()
{
    //@Item Slot Button 호버 이벤트
    ItemSlotButtonHovered.Broadcast(UniqueItemID);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에 마우스가 올라갔습니다. ID: {0}", UniqueItemID.ToString());

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...

}

void UInteractableItemSlot::OnItemSlotButtonUnhovered_Implementation()
{
    //@Item Slot Button 언호버 이벤트
    ItemSlotButtonUnhovered.Broadcast(UniqueItemID);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼에서 마우스가 벗어났습니다. ID: {0}", UniqueItemID.ToString());

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...

}

void UInteractableItemSlot::OnItemSlotButtonClicked_Implementation()
{
    ItemSlotButtonClicked.Broadcast(UniqueItemID);

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼이 클릭되었습니다. ID: {0}", UniqueItemID.ToString());

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...

}

void UInteractableItemSlot::ItemSlotButtonCanceledNotified_Implementation(const FGuid& ItemID)
{
    if (ItemID != UniqueItemID)
    {
        return;
    }

    //@Item Slot Button 선택 취소 이벤트
    NotifyItemSlotButtonCanceled.Broadcast();

    UE_LOGFMT(LogInteractableItemSlot, Log, "아이템 슬롯 버튼 선택이 취소되었습니다. ID: {0}", ItemID.ToString());

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
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