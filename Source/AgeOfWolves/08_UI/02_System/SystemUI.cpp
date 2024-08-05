#include "SystemUI.h"
#include "Logging/StructuredLog.h"    

#include "08_UI/02_System/InventoryUI.h"
#include "08_UI/02_System/SystemUIToolBar.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/Image.h"

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

void USystemUI::InternalBindingToToolBar(USystemUIToolBar* ToolBar)
{
    //@Inventory UI
    if (!ToolBar)
    {
        UE_LOGFMT(LogSystemUI, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }
    //@Internal Binding
    ToolBar->ToolBarInitFinished.BindUFunction(this, "OnToolBarInitFinished");

    UE_LOGFMT(LogSystemUI, Log, "SystemUI: Tool Bar UI의 OnInitFinished에 OnToolBarInitFinished 함수를 바인딩했습니다.");
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

    //@TODO: Sub UI들의 생성 작업 여기
    CreateBG();
    //CreateToolBar();
    CreateInventoryUI();

    RequestStartInitBySystemUI.Broadcast();

    UE_LOGFMT(LogSystemUI, Log, "SystemUI 초기화 완료");
}
#pragma endregion

#pragma region SubWidgets
void USystemUI::CreateBG()
{
    if (!SystemUIOverlay || !SystemUI_Outer_BG || !SystemUI_Inner_BG)
    {
        UE_LOGFMT(LogSystemUI, Error, "배경 설정에 필요한 위젯이 없습니다.");
        return;
    }

    // Outer BG 설정
    UOverlaySlot* OuterBGSlot = Cast<UOverlaySlot>(SystemUI_Outer_BG->Slot);
    if (OuterBGSlot)
    {
        OuterBGSlot->SetHorizontalAlignment(HAlign_Fill);
        OuterBGSlot->SetVerticalAlignment(VAlign_Fill);
    }

    // Inner BG 설정
    UOverlaySlot* InnerBGSlot = Cast<UOverlaySlot>(SystemUI_Inner_BG->Slot);
    if (InnerBGSlot)
    {
        InnerBGSlot->SetHorizontalAlignment(HAlign_Fill);
        InnerBGSlot->SetVerticalAlignment(VAlign_Fill);

        // 뷰포트 크기 가져오기
        FVector2D ViewportSize;
        if (GEngine && GEngine->GameViewport)
        {
            GEngine->GameViewport->GetViewportSize(ViewportSize);
        }

        // 패딩 계산
        float PaddingValue = FMath::Min(ViewportSize.X, ViewportSize.Y) * (Inner_BG_Padding / 100.0f);

        // 패딩 설정
        InnerBGSlot->SetPadding(FMargin(PaddingValue));
    }
}

void USystemUI::CreateToolBar()
{
    //@Scale Box
    if (!IsValid(ToolBarBox))
    {
        UE_LOGFMT(LogSystemUI, Error, "ToolBarBox가 유효하지 않습니다.");
        return;
    }
    //@Tool Bar Class
    if (!ensureMsgf(ToolBarClass, TEXT("ToolBarClass가 설정되지 않았습니다.")))
    {
        return;
    }
    //@Create Widget
    USystemUIToolBar* ToolBar = CreateWidget<USystemUIToolBar>(this, ToolBarClass);
    if (!IsValid(ToolBar))
    {
        UE_LOGFMT(LogSystemUI, Error, "ToolBar 위젯 생성에 실패했습니다.");
        return;
    }
    //@비동기 초기화를 위한 바인딩
    RequestStartInitBySystemUI.AddUFunction(ToolBar, "InitializeToolBar");
    //@Internal Binding
    InternalBindingToToolBar(ToolBar);
    //@Add Child
    UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(ToolBarBox->AddChild(ToolBar));
    if (ScaleBoxSlot)
    {
        ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);

        UE_LOGFMT(LogSystemUI, Log, "ToolBar 위젯이 성공적으로 추가되었습니다.");
    }
}

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
#pragma endregion

#pragma region Callbacks
void USystemUI::OnInventoryUIInitFinished()
{
    //@Delegate
    NotifyInventoryUIInitFinished.ExecuteIfBound();
}

void USystemUI::OnToolBarInitFinished()
{
    if (NotifyToolBarInitFinished.IsBound())
    {
        NotifyToolBarInitFinished.Execute();
    }
}
#pragma endregion