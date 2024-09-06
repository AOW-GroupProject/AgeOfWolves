#include "InventoryUI.h"
#include "Logging/StructuredLog.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryUIContent.H"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogInventoryUI)

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

void UInventoryUI::InternalBindingToInventoryUIContent(UInventoryUIContent* InventoryUIContent)
{
    //@Item Description
    if (!InventoryUIContent)
    {
        UE_LOGFMT(LogInventoryUI, Error, "InventoryUIContent UI가 유효하지 않습니다.");
        return;
    }

    InventoryUIContent->InventoryUIContentInitFinished.BindUFunction(this, "OnInventoryUIContentInitFinished");
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

        bInventoryUIContentReady = false;

        //@Super
        Super::CheckMenuUIContentInitFinished();
    }

}
#pragma endregion

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
    if (!InventoryUIContentOverlay)
    {
        UE_LOGFMT(LogInventoryUI, Error, "InventoryUIContentOverlay가 유효하지 않습니다.");
        return;
    }

    if (!ensureMsgf(InventoryUIContentClass, TEXT("InventoryUIContentClass가 설정되지 않았습니다.")))
    {
        return;
    }

    UInventoryUIContent* InventoryUIContent = CreateWidget<UInventoryUIContent>(this, InventoryUIContentClass);
    if (InventoryUIContent)
    {
        //@비동기 초기화 이벤트
        RequestStartInitByInventoryUI.AddUFunction(InventoryUIContent, "InitializeInventoryUIContent");

        //@내부 바인딩
        InternalBindingToInventoryUIContent(InventoryUIContent);

        UOverlaySlot* OverlaySlot = InventoryUIContentOverlay->AddChildToOverlay(InventoryUIContent);
        if (OverlaySlot)
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
            OverlaySlot->SetVerticalAlignment(VAlign_Fill);
        }

        UE_LOGFMT(LogInventoryUI, Log, "Inventory Content UI 위젯이 성공적으로 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogInventoryUI, Error, "Inventory Content UI 위젯 생성에 실패했습니다.");
    }
}
#pragma endregion

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

    //@가시성 변화 이벤트 호출시 수행할 동작들 아래에서 작성...
    
    //@초기 설정으로 Reset
    ResetMenuUIContent();

}
#pragma endregion

