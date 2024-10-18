#include "InventoryUI.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryUIContent.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"

DEFINE_LOG_CATEGORY(LogInventoryUI)

//@Defualt Setting
#pragma region Default Setting
UInventoryUI::UInventoryUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    //@Menu Category
    MenuCategory = EMenuCategory::Inventory;
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
}

void UInventoryUI::NativeDestruct()
{
    Super::NativeDestruct();
}

FReply UInventoryUI::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    FReply Reply = Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

    //@SetDirectly(SetFocus())에 의한 포커스가 아닐 경우, 포커스를 해제합니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return Reply.Handled().ClearUserFocus();
    }

    if (InFocusEvent.GetCause() == EFocusCause::SetDirectly && Reply.IsEventHandled())
    {
        if (!InventoryUIContent)
        {
            UE_LOGFMT(LogInventoryUI, Warning, "InventoryUIContent가 유효하지 않습니다. 포커스를 설정할 수 없습니다.");
            return Reply;
        }

        //@Set Focus
        InventoryUIContent->SetFocus();
    }

    return Reply;
}

void UInventoryUI::InternalBindingToInventoryUIContent(UInventoryUIContent* Content)
{
    //@Item Description
    if (!Content)
    {
        UE_LOGFMT(LogInventoryUI, Error, "InventoryUIContent UI가 유효하지 않습니다.");
        return;
    }

    Content->InventoryUIContentInitFinished.BindUFunction(this, "OnInventoryUIContentInitFinished");
}

void UInventoryUI::InitializeMenuUIContent()
{

    //@Create Inventory Content UI
    CreateInventoryContent();

    //@Super
    Super::InitializeMenuUIContent();

    //@초기화 완료 이벤트 호출
    RequestStartInitByInventoryUI.Broadcast();
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
    //@Inventory UI Content
    for (auto& OverlaySlot : InventoryUIContentOverlay->GetAllChildren())
    {
        if (auto Widget = Cast<UInventoryUIContent>(OverlaySlot))
        {
            Widget->ResetInventoryUIContent();
            break;
        }
    }

    Super::ResetMenuUIContent();
}

void UInventoryUI::CreateInventoryContent()
{
    //@Inventory UI Content 오버레이
    if (!InventoryUIContentOverlay)
    {
        UE_LOGFMT(LogInventoryUI, Error, "InventoryUIContentOverlay가 유효하지 않습니다.");
        return;
    }

    if (!ensureMsgf(InventoryUIContentClass, TEXT("InventoryUIContentClass가 설정되지 않았습니다.")))
    {
        return;
    }

    //@Inventory UI Content
    InventoryUIContent = CreateWidget<UInventoryUIContent>(this, InventoryUIContentClass);

    if (!InventoryUIContent)
    {
        UE_LOGFMT(LogInventoryUI, Error, "Inventory Content UI 위젯 생성에 실패했습니다.");
        return;
    }

    //@비동기 초기화 이벤트
    RequestStartInitByInventoryUI.AddUFunction(InventoryUIContent, "InitializeInventoryUIContent");

    //@내부 바인딩
    InternalBindingToInventoryUIContent(InventoryUIContent);

    //@Alignment
    UOverlaySlot* OverlaySlot = InventoryUIContentOverlay->AddChildToOverlay(InventoryUIContent);
    if (OverlaySlot)
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    UE_LOGFMT(LogInventoryUI, Log, "Inventory Content UI 위젯이 성공적으로 추가되었습니다.");

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInventoryUI::OnInventoryUIContentInitFinished()
{
    UE_LOGFMT(LogInventoryUI, Log, "InventoryUIContent 초기화가 완료되었습니다.");

    //@Init Finished
    bInventoryUIContentReady = true;
    //@Inventory UI의 초기화 완료 여부 체크
    CheckMenuUIContentInitFinished();
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
UItemSlots* UInventoryUI::GetItemSlotsByType(EItemType ItemType) const
{
    if (!InventoryUIContent.Get())
    {
        UE_LOGFMT(LogInventoryUI, Error, "Inventory UI 내부 컨텐츠가 유효하지 않습니다.");
        return nullptr;
    }

    if (InventoryUIContent)
    {
        if (UItemSlots* ItemSlotsWidget = CastChecked<UItemSlots>(InventoryUIContent->GetItemSlotsUI(ItemType)))
        {
            return ItemSlotsWidget;
        }
    }

    return nullptr;
}
#pragma endregion