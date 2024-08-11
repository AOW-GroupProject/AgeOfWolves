#include "MenuUI.h"
#include "Logging/StructuredLog.h"    

#include "04_Component/UIComponent.h"

#include "08_UI/02_Menu/MenuUIToolBar.h"
#include "08_UI/02_Menu/01_InventoryUI/InventoryUI.h"
#include "08_UI/02_Menu/02_LevelUI/LevelUI.h"
#include "08_UI/02_Menu/03_MapUI/MapUI.h"
#include "08_UI/02_Menu/04_SystemUI/SystemUI.h"

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

    //@TODO: UI Component에 외부 바인딩
    //@외부 바인딩
    ExternalBindingToUIComponent();

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

void UMenuUI::ExternalBindingToUIComponent()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogMenuUI, Error, "MenuUI::ExternalBindingToUIComponent: 월드가 null입니다");
        return;
    }

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogMenuUI, Error, "MenuUI::ExternalBindingToUIComponent: 플레이어 컨트롤러가 null입니다");
        return;
    }

    UUIComponent* UIComp = PC->FindComponentByClass<UUIComponent>();
    if (!UIComp)
    {
        UE_LOGFMT(LogMenuUI, Error, "{0}: UI 컴포넌트가 유효하지 않습니다", __FUNCTION__);
        return;
    }

    UIComp->WidgetVisibilityChanged.AddUObject(this, &UMenuUI::OnUIVisibilityChanged);
    UE_LOGFMT(LogMenuUI, Log, "MenuUI가 UIComponent의 WidgetVisibilityChanged 델리게이트에 성공적으로 바인딩되었습니다.");
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
    ToolBar->MenuCategoryButtonClikced.BindUFunction(this, "OnMenuCategoryButtonClikced");

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

void UMenuUI::InternalBindingToLevelUI(ULevelUI* LevelUI)
{
    if (!LevelUI)
    {
        UE_LOGFMT(LogMenuUI, Error, "Level UI가 유효하지 않습니다.");
        return;
    }
    LevelUI->LevelUIInitFinished.BindUFunction(this, "OnLevelUIInitFinished");
    UE_LOGFMT(LogMenuUI, Log, "MenuUI: LevelUI의 LevelUIInitFinished에 OnLevelUIInitFinished 함수를 바인딩했습니다.");
}

void UMenuUI::InternalBindingToMapUI(UMapUI* MapUI)
{
    if (!MapUI)
    {
        UE_LOGFMT(LogMenuUI, Error, "Map UI가 유효하지 않습니다.");
        return;
    }
    MapUI->MapUIInitFinished.BindUFunction(this, "OnMapUIInitFinished");
    UE_LOGFMT(LogMenuUI, Log, "MenuUI: MapUI의 MapUIInitFinished에 OnMapUIInitFinished 함수를 바인딩했습니다.");
}

void UMenuUI::InternalBindingToSystemUI(USystemUI* SystemUI)
{
    if (!SystemUI)
    {
        UE_LOGFMT(LogMenuUI, Error, "System UI가 유효하지 않습니다.");
        return;
    }
    SystemUI->SystemUIInitFinished.BindUFunction(this, "OnSystemUIInitFinished");
    UE_LOGFMT(LogMenuUI, Log, "MenuUI: SystemUI의 SystemUIInitFinished에 OnSystemUIInitFinished 함수를 바인딩했습니다.");
}

void UMenuUI::InitializeMenuUI()
{
    //@BG
    CreateBG();
    //@TODO: Tool Bar
    CreateToolBar();
    //@All UIs
    CreateAllCategoryUIs();
    //@초기화 요청 이벤트 호출
    RequestStartInitByMenuUI.Broadcast();
}
#pragma endregion

#pragma region SubWidgets
void UMenuUI::CreateBG()
{
    if (!MenuUIOverlay || !MenuUI_Outer_BG || !MenuUI_Inner_BG)
    {
        UE_LOGFMT(LogMenuUI, Error, "배경 설정에 필요한 위젯이 없습니다.");
        return;
    }

    UE_LOGFMT(LogMenuUI, Log, "배경 생성을 시작합니다.");

    // Outer BG 설정
    UOverlaySlot* OuterBGSlot = Cast<UOverlaySlot>(MenuUI_Outer_BG->Slot);
    if (!OuterBGSlot)
    {
        UE_LOGFMT(LogMenuUI, Warning, "Outer BG 슬롯 설정에 실패했습니다.");
        return;
    }

    // Inner BG 설정
    UOverlaySlot* InnerBGSlot = Cast<UOverlaySlot>(MenuUI_Inner_BG->Slot);
    if (!InnerBGSlot)
    {
        UE_LOGFMT(LogMenuUI, Warning, "Inner BG 슬롯 설정에 실패했습니다.");
        return;
    }

    UE_LOGFMT(LogMenuUI, Log, "배경 생성이 완료되었습니다.");
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
    //@Inventory UI
    CreateInventoryUI();
    //@Level UI
    CreateLevelUI();
    //@Map UI
    CreateMapUI();
    //@System UI
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
        //@Scale Box
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(InventoryUIBox->AddChild(InventoryUI));
        if (ScaleBoxSlot)
        {
            ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }
        //@TMap
        ChildUIs.Add(EMenuCategory::Inventory, InventoryUI);
        //@바인딩
        RequestStartInitByMenuUI.AddUFunction(InventoryUI, "InitializeInventoryUI");
        //@내부 바인딩
        InternalBindingToInventoryUI(InventoryUI);

        UE_LOGFMT(LogMenuUI, Log, "InventoryUI가 성공적으로 생성되고 ScaleBox에 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogMenuUI, Error, "InventoryUI 생성에 실패했습니다.");
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
        //@Scale Box
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(LevelUIBox->AddChild(LevelUI));
        if (ScaleBoxSlot)
        {
            ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }
        //@TMap
        ChildUIs.Add(EMenuCategory::Level, LevelUI);
        //@바인딩
        RequestStartInitByMenuUI.AddUFunction(LevelUI, "InitializeLevelUI");
        // 필요한 경우 내부 바인딩 추가

        UE_LOGFMT(LogMenuUI, Log, "LevelUI가 성공적으로 생성되고 ScaleBox에 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogMenuUI, Error, "LevelUI 생성에 실패했습니다.");
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
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(MapUIBox->AddChild(MapUI));
        if (ScaleBoxSlot)
        {
            ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }
        ChildUIs.Add(EMenuCategory::Map, MapUI);
        RequestStartInitByMenuUI.AddUFunction(MapUI, "InitializeMapUI");
        // 필요한 경우 내부 바인딩 추가

        UE_LOGFMT(LogMenuUI, Log, "MapUI가 성공적으로 생성되고 ScaleBox에 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogMenuUI, Error, "MapUI 생성에 실패했습니다.");
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
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(SystemUIBox->AddChild(SystemUI));
        if (ScaleBoxSlot)
        {
            ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }
        ChildUIs.Add(EMenuCategory::System, SystemUI);
        RequestStartInitByMenuUI.AddUFunction(SystemUI, "InitializeSystemUI");
        // 필요한 경우 내부 바인딩 추가

        UE_LOGFMT(LogMenuUI, Log, "SystemUI가 성공적으로 생성되고 ScaleBox에 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogMenuUI, Error, "SystemUI 생성에 실패했습니다.");
    }
}

void UMenuUI::SetCategoryVisibility(EMenuCategory Category, bool bVisible)
{
    //@Widget
    if (UUserWidget* Widget = GetCategoryUI(Category))
    {
        Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

UUserWidget* UMenuUI::GetCategoryUI(EMenuCategory Category) const
{
    if (auto FoundWidget = ChildUIs.Find(Category))
    {
        return *FoundWidget;
    }
    return nullptr;
}
#pragma endregion

#pragma region Callbacks
void UMenuUI::OnUIVisibilityChanged(UUserWidget* Widget, bool bVisible)
{
    if (Widget != this)
    {
        return;
    }

    if (bVisible)
    {
        // 현재 카테고리 UI만 표시하고 나머지는 숨김
        for (auto& Pair : ChildUIs)
        {
            SetCategoryVisibility(Pair.Key, Pair.Key == CurrentCategory);
        }

        // MenuUI를 뷰포트에 추가
        AddToViewport();

        UE_LOGFMT(LogMenuUI, Log, "MenuUI가 뷰포트에 추가되었습니다. 현재 카테고리: {0}", *UEnum::GetValueAsString(CurrentCategory));

        // TODO: MenuUI가 표시될 때 필요한 추가 로직
        // 예: 애니메이션 재생, 초기 포커스 설정 등
    }
    else
    {
        // MenuUI가 숨겨질 때
        // 모든 카테고리 UI 숨김
        for (auto& Pair : ChildUIs)
        {
            SetCategoryVisibility(Pair.Key, false);
        }

        RemoveFromParent();
        UE_LOGFMT(LogMenuUI, Log, "MenuUI가 부모로부터 제거되었습니다.");

        // TODO: MenuUI가 숨겨질 때 필요한 추가 로직
        // 예: 데이터 저장, 정리 작업 등
    }
}

void UMenuUI::OnToolBarInitFinished()
{
    if (NotifyToolBarInitFinished.IsBound())
    {
        NotifyToolBarInitFinished.Execute();
    }
}

void UMenuUI::OnInventoryUIInitFinished()
{
    //@Delegate
    NotifyInventoryUIInitFinished.ExecuteIfBound();
}

void UMenuUI::OnLevelUIInitFinished()
{
}

void UMenuUI::OnMapUIInitFinished()
{
}

void UMenuUI::OnSystemUIInitFinished()
{
}

void UMenuUI::OnMenuCategoryButtonClikced(EMenuCategory MenuCategory)
{
}
#pragma endregion