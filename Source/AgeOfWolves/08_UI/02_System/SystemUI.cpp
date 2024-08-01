#include "SystemUI.h"
#include "Logging/StructuredLog.h"    

#include "08_UI/02_System/InventoryUI.h"

#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"

DEFINE_LOG_CATEGORY(LogSystemUI)

#pragma region Default Setting
USystemUI::USystemUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void USystemUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void USystemUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void USystemUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void USystemUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void USystemUI::InternalBindingToInventoryUI(UInventoryUI* InventoryUI)
{
    //@Inventory UI
    if (!InventoryUI)
    {
        UE_LOGFMT(LogSystemUI, Error, "Inventory UI가 유효하지 않습니다.");
        return;
    }
    //@Internal Binding
    InventoryUI->InventoryUIInitFinished.BindUFunction(this, "OnInventoryUIInitFinished");

    UE_LOGFMT(LogSystemUI, Log, "SystemUI: InventoryUI의 OnInitFinished에 OnInventoryUIInitFinished 함수를 바인딩했습니다.");
}

void USystemUI::InitializeSystemUI()
{
    UE_LOGFMT(LogSystemUI, Log, "SystemUI 초기화 시작");

    CreateInventoryUI();

    RequestStartInitBySystemUI.Broadcast();

    UE_LOGFMT(LogSystemUI, Log, "SystemUI 초기화 완료");
}
#pragma endregion

#pragma region Inventory UI
void USystemUI::CreateInventoryUI()
{
    //@Scale Box
    if (!IsValid(InventoryUIBox))
    {
        UE_LOGFMT(LogSystemUI, Error, "InventoryUIBox가 유효하지 않습니다.");
        return;
    }
    //@BP Class
    if (!ensureMsgf(InventoryUIClass, TEXT("InventoryUIClass가 설정되지 않았습니다.")))
    {
        return;
    }
    //@Create Widget
    UInventoryUI* InventoryUI = CreateWidget<UInventoryUI>(this, InventoryUIClass);
    if (!IsValid(InventoryUI))
    {
        UE_LOGFMT(LogSystemUI, Error, "InventoryUI 위젯 생성에 실패했습니다.");
        return;
    }
    //@초기화 바인딩(지연 초기화)
    RequestStartInitBySystemUI.AddUFunction(InventoryUI, "InitializeInventoryUI");
    //@Internal Binding
    InternalBindingToInventoryUI(InventoryUI);
    //@Allignment
    UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(InventoryUIBox->AddChild(InventoryUI));
    if (ScaleBoxSlot)
    {
        ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);

        UE_LOGFMT(LogSystemUI, Log, "InventoryUI 위젯이 성공적으로 추가되었습니다.");
    }
}

UInventoryUI* USystemUI::GetInventoryUI() const
{
    //@Scale Box
    if (!InventoryUIBox)
    {
        UE_LOGFMT(LogSystemUI, Warning, "Inventory UI Box가 유효하지 않습니다. : {0}", __FUNCTION__);
        return nullptr;
    }
    //@Child Widget
    UWidget* ChildWidget = InventoryUIBox->GetChildAt(0);
    if (!ChildWidget)
    {
        UE_LOGFMT(LogSystemUI, Warning, "Inventory UI Box에 유효한 자식 Widget이 존재하지 않습니다.");
        return nullptr;
    }
    //@Casting
    auto InventoryUI = Cast<UInventoryUI>(ChildWidget);
    if (!InventoryUI)
    {
        UE_LOGFMT(LogSystemUI, Warning, "Casting Failed : {0}", __FUNCTION__);
        return nullptr;
    }

    return InventoryUI;
}

void USystemUI::OnInventoryUIInitFinished()
{
    //@Delegate
    NotifyInventoryUIInitFinished.ExecuteIfBound();
}
#pragma endregion