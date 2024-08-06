#include "MenuUI.h"
#include "Logging/StructuredLog.h"    

#include "08_UI/02_Menu/MenuUIToolBar.h"
#include "08_UI/02_Menu/InventoryUI.h"
#include "08_UI/02_Menu/LevelUI.h"
#include "08_UI/02_Menu/MapUI.h"
#include "08_UI/02_Menu/SystemUI.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/Image.h"

DEFINE_LOG_CATEGORY(LogMenuUI)

#pragma region Default Setting
UMenuUI::UMenuUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UMenuUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UMenuUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UMenuUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMenuUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void UMenuUI::InternalBindingToToolBar(UMenuUIToolBar* ToolBar)
{
    //@Inventory UI
    if (!ToolBar)
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }
    //@Internal Binding
    ToolBar->ToolBarInitFinished.BindUFunction(this, "OnToolBarInitFinished");

    UE_LOGFMT(LogMenuUI, Log, "MenuUI: Tool Bar UI의 OnInitFinished에 OnToolBarInitFinished 함수를 바인딩했습니다.");
}

void UMenuUI::InternalBindingToInventoryUI(UInventoryUI* InventoryUI)
{
    //@Inventory UI
    if (!InventoryUI)
    {
        UE_LOGFMT(LogMenuUI, Error, "Inventory UI가 유효하지 않습니다.");
        return;
    }
    //@Internal Binding
    InventoryUI->InventoryUIInitFinished.BindUFunction(this, "OnInventoryUIInitFinished");

    UE_LOGFMT(LogMenuUI, Log, "MenuUI: InventoryUI의 OnInitFinished에 OnInventoryUIInitFinished 함수를 바인딩했습니다.");
}

void UMenuUI::InitializeMenuUI()
{
    UE_LOGFMT(LogMenuUI, Log, "MenuUI 초기화 시작");

    //@BG
    CreateBG();
    //@TODO: Tool Bar

    //@All UIs
    CreateAllCategoryUIs();

    RequestStartInitByMenuUI.Broadcast();

    UE_LOGFMT(LogMenuUI, Log, "MenuUI 초기화 완료");
}
#pragma endregion

#pragma region SubWidgets
UUserWidget* UMenuUI::GetCategoryUI(EMenuCategory Category) const
{
    if (auto FoundWidget = CategoryWidgets.Find(Category))
    {
        return *FoundWidget;
    }
    return nullptr;
}

void UMenuUI::CreateBG()
{
    if (!MenuUIOverlay || !MenuUI_Outer_BG || !MenuUI_Inner_BG)
    {
        UE_LOGFMT(LogMenuUI, Error, "배경 설정에 필요한 위젯이 없습니다.");
        return;
    }

    // Outer BG 설정
    UOverlaySlot* OuterBGSlot = Cast<UOverlaySlot>(MenuUI_Outer_BG->Slot);
    if (OuterBGSlot)
    {
        OuterBGSlot->SetHorizontalAlignment(HAlign_Fill);
        OuterBGSlot->SetVerticalAlignment(VAlign_Fill);
    }

    // Inner BG 설정
    UOverlaySlot* InnerBGSlot = Cast<UOverlaySlot>(MenuUI_Inner_BG->Slot);
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

void UMenuUI::CreateToolBar()
{
    //@Scale Box
    if (!IsValid(ToolBarBox))
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBarBox가 유효하지 않습니다.");
        return;
    }
    //@Tool Bar Class
    if (!ensureMsgf(ToolBarClass, TEXT("ToolBarClass가 설정되지 않았습니다.")))
    {
        return;
    }
    //@Create Widget
    UMenuUIToolBar* ToolBar = CreateWidget<UMenuUIToolBar>(this, ToolBarClass);
    if (!IsValid(ToolBar))
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar 위젯 생성에 실패했습니다.");
        return;
    }
    //@비동기 초기화를 위한 바인딩
    RequestStartInitByMenuUI.AddUFunction(ToolBar, "InitializeToolBar");
    //@Internal Binding
    InternalBindingToToolBar(ToolBar);
    //@Add Child
    UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(ToolBarBox->AddChild(ToolBar));
    if (ScaleBoxSlot)
    {
        ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);

        UE_LOGFMT(LogMenuUI, Log, "ToolBar 위젯이 성공적으로 추가되었습니다.");
    }
}

void UMenuUI::CreateAllCategoryUIs()
{
    CreateInventoryUI();
    CreateLevelUI();
    CreateMapUI();
    CreateSystemUI();
}

void UMenuUI::CreateInventoryUI()
{
    if (!IsValid(InventoryUIBox) || !InventoryUIClass)
    {
        UE_LOGFMT(LogMenuUI, Error, "InventoryUIBox 또는 InventoryUIClass가 유효하지 않습니다.");
        return;
    }

    UInventoryUI* InventoryUI = CreateWidget<UInventoryUI>(this, InventoryUIClass);
    if (IsValid(InventoryUI))
    {
        InventoryUIBox->AddChild(InventoryUI);
        CategoryWidgets.Add(EMenuCategory::Inventory, InventoryUI);
        RequestStartInitByMenuUI.AddUFunction(InventoryUI, "InitializeInventoryUI");
        InternalBindingToInventoryUI(InventoryUI);
    }
}

void UMenuUI::CreateLevelUI()
{
    if (!IsValid(LevelUIBox) || !LevelUIClass)
    {
        UE_LOGFMT(LogMenuUI, Error, "LevelUIBox 또는 LevelUIClass가 유효하지 않습니다.");
        return;
    }

    ULevelUI* LevelUI = CreateWidget<ULevelUI>(this, LevelUIClass);
    if (IsValid(LevelUI))
    {
        LevelUIBox->AddChild(LevelUI);
        CategoryWidgets.Add(EMenuCategory::Level, LevelUI);
        RequestStartInitByMenuUI.AddUFunction(LevelUI, "InitializeLevelUI");
        // 필요한 경우 내부 바인딩 추가

    }
}

void UMenuUI::CreateMapUI()
{
    if (!IsValid(MapUIBox) || !MapUIClass)
    {
        UE_LOGFMT(LogMenuUI, Error, "MapUIBox 또는 MapUIClass가 유효하지 않습니다.");
        return;
    }

    UMapUI* MapUI = CreateWidget<UMapUI>(this, MapUIClass);
    if (IsValid(MapUI))
    {
        MapUIBox->AddChild(MapUI);
        CategoryWidgets.Add(EMenuCategory::Map, MapUI);
        RequestStartInitByMenuUI.AddUFunction(MapUI, "InitializeMapUI");
        // 필요한 경우 내부 바인딩 추가
    }
}

void UMenuUI::CreateSystemUI()
{
    if (!IsValid(SystemUIBox) || !SystemUIClass)
    {
        UE_LOGFMT(LogMenuUI, Error, "SystemUIBox 또는 SystemUIClass가 유효하지 않습니다.");
        return;
    }

    USystemUI* SystemUI = CreateWidget<USystemUI>(this, SystemUIClass);
    if (IsValid(SystemUI))
    {
        SystemUIBox->AddChild(SystemUI);
        CategoryWidgets.Add(EMenuCategory::System, SystemUI);
        RequestStartInitByMenuUI.AddUFunction(SystemUI, "InitializeSystemUI");
        // 필요한 경우 내부 바인딩 추가
    }
}
#pragma endregion

#pragma region Callbacks
void UMenuUI::OnInventoryUIInitFinished()
{
    //@Delegate
    NotifyInventoryUIInitFinished.ExecuteIfBound();
}

void UMenuUI::OnToolBarInitFinished()
{
    if (NotifyToolBarInitFinished.IsBound())
    {
        NotifyToolBarInitFinished.Execute();
    }
}
#pragma endregion