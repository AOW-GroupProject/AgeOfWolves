#include "PlayerHUD.h"
#include "Logging/StructuredLog.h"    

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "08_UI/01_HUD/HUD_StatusUI.h"
#include "08_UI/01_HUD/HUD_QuickSlotsUI.h"
#include "08_UI/01_HUD/HUD_HPToolItemDotGauge.h"

#include "04_Component/UIComponent.h"

DEFINE_LOG_CATEGORY(LogHUD)

//@Defualt Setting
#pragma region Default Setting
UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    StatusUIRef = nullptr;
    QuickSlotUIRef = nullptr;
    HPToolItemDotGaugeRef = nullptr;
}

void UPlayerHUD::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@TODO: 외부 바인딩
    //@외부 바인딩
    ExternalBindingToUIComponent();
}

void UPlayerHUD::NativePreConstruct()
{
    Super::NativePreConstruct();

}

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();
}

void UPlayerHUD::NativeDestruct()
{
    Super::NativeDestruct();
}

void UPlayerHUD::ExternalBindingToUIComponent()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogHUD, Error, "PlayerHUD::ExternalBindingToUIComponent: 월드가 null입니다");
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogHUD, Error, "PlayerHUD::ExternalBindingToUIComponent: 플레이어 컨트롤러가 null입니다");
        return;
    }
    //@UI Comp
    UUIComponent* UIComp = PC->FindComponentByClass<UUIComponent>();
    if (!UIComp)
    {
        UE_LOGFMT(LogHUD, Error, "{0}: UI 컴포넌트가 유효하지 않습니다", __FUNCTION__);
        return;
    }
    //@외부 바인딩
    UIComp->WidgetVisibilityChanged.AddUObject(this, &UPlayerHUD::OnUIVisibilityChanged);
    UE_LOGFMT(LogHUD, Log, "PlayerHUD가 UIComponent의 WidgetVisibilityChanged 델리게이트에 성공적으로 바인딩되었습니다.");
}

void UPlayerHUD::InternalBindToStatusUI(UHUD_StatusUI* StatusUI)
{
    if (!StatusUI)
    {
        UE_LOGFMT(LogHUD, Error, "Status UI가 유효하지 않습니다.");
        return;
    }

    //@초기화 완료 이벤트 바인딩
    StatusUI->StatusUIInitFinished.BindUFunction(this, "OnStatusUIInitFinished");
}

void UPlayerHUD::InternalBindToQuickSlotsUI(UHUD_QuickSlotsUI* QuickSlotsUI)
{
    if (!QuickSlotsUI)
    {
        UE_LOGFMT(LogHUD, Error, "Quick Slots UI가 유효하지 않습니다.");
        return;
    }
    //@Internal Binding
    QuickSlotsUI->QuickSlotsInitFinished.BindUFunction(this, "OnQuickSlotsInitFinished");
}

void UPlayerHUD::InternalBindToHPToolItemDotGauge(UHUD_HPToolItemDotGauge* HPToolItemDotGauge)
{
    if (!HPToolItemDotGauge)
    {
        return;
    }

    //@내부 바인딩
    HPToolItemDotGauge->HorizontalDotGaugeInitFinished.BindUFunction(this, "OnHPToolItemDotGaugeInitFinished");
}

void UPlayerHUD::InitializePlayerHUD()
{
    //@Status UI 생성
    CreateStatusUI();
    //@Quick Slot UI 생성
    CreateQuickSlotUI();
    //@HP Potion Dot Gauge 생성
    CreateHPToolItemDotGauge();

    RequestStartInitByHUD.Broadcast();
}

void UPlayerHUD::CheckAllUIsInitFinsiehd()
{
    if (bStatusUIInitFinished && bQuickSlotsInitFinished && bHPToolItemDotGaugeInitFinished)
    {
        bStatusUIInitFinished = false;
        bQuickSlotsInitFinished = false;
        bHPToolItemDotGaugeInitFinished = false;

        HUDInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UPlayerHUD::CreateStatusUI()
{
    //@Status UI Blueprint class, Status UI Overlay 체크
    if (!ensureMsgf(StatusUIClass && StatusUIOverlay, TEXT("StatusUIClass 또는 StatusUIOverlay가 유효하지 않습니다.")))
    {
        return;
    }

    //@Status UI 생성
    UHUD_StatusUI* StatusUI = CreateWidget<UHUD_StatusUI>(this, StatusUIClass);
    if (!IsValid(StatusUI))
    {
        UE_LOGFMT(LogHUD, Error, "StatusUI 위젯 생성에 실패했습니다.");
        return;
    }

    //@초기화 요청 이벤트에 바인딩
    RequestStartInitByHUD.AddUFunction(StatusUI, "InitializeStatusUI");

    //@내부 바인딩
    InternalBindToStatusUI(StatusUI);

    //@Alignment 설정
    if (UOverlaySlot* OverlaySlot = StatusUIOverlay->AddChildToOverlay(StatusUI))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    StatusUIRef = StatusUI;

    UE_LOGFMT(LogHUD, Log, "StatusUI가 성공적으로 생성되고 StatusUIOverlay에 추가되었습니다.");
}

void UPlayerHUD::CreateQuickSlotUI()
{
    //@Quick Slot UI Blueprint class, Quick Slot UI Overlay 체크
    if (!ensureMsgf(QuickSlotUIClass && QuickSlotUIOverlay, TEXT("QuickSlotUIClass 또는 QuickSlotUIOverlay가 유효하지 않습니다.")))
    {
        return;
    }

    //@Quick Slot UI 생성
    UHUD_QuickSlotsUI* QuickSlotUI = CreateWidget<UHUD_QuickSlotsUI>(this, QuickSlotUIClass);
    if (!IsValid(QuickSlotUI))
    {
        UE_LOGFMT(LogHUD, Error, "QuickSlotUI 위젯 생성에 실패했습니다.");
        return;
    }

    //@초기화 요청 이벤트에 바인딩
    RequestStartInitByHUD.AddUFunction(QuickSlotUI, "InitializeQuickSlotsUI");

    //@내부 바인딩
    InternalBindToQuickSlotsUI(QuickSlotUI);

    //@Alignment 설정
    if (UOverlaySlot* OverlaySlot = QuickSlotUIOverlay->AddChildToOverlay(QuickSlotUI))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    QuickSlotUIRef = QuickSlotUI;

    UE_LOGFMT(LogHUD, Log, "QuickSlotUI가 성공적으로 생성되고 QuickSlotUIOverlay에 추가되었습니다.");
}

void UPlayerHUD::CreateHPToolItemDotGauge()
{
    //@HP Potion UI Blueprint class, HP Potion UI Overlay 체크
    if (!ensureMsgf(HPToolItemDotGaugeClass && HPToolItemDotGaugeOverlay, TEXT("HPToolItemDotGaugeClass 또는 HPToolItemDotGaugeOverlay가 유효하지 않습니다.")))
    {
        return;
    }

    //@HP Potion UI 생성
    UHUD_HPToolItemDotGauge* HPToolItemDotGauge = CreateWidget<UHUD_HPToolItemDotGauge>(this, HPToolItemDotGaugeClass);
    if (!IsValid(HPToolItemDotGauge))
    {
        UE_LOGFMT(LogHUD, Error, "HPToolItemDotGauge 위젯 생성에 실패했습니다.");
        return;
    }

    //@비동기 초기화
    RequestStartInitByHUD.AddUFunction(HPToolItemDotGauge, "InitializeHorizontalDotGauge");

    //@내부 바인딩
    InternalBindToHPToolItemDotGauge(HPToolItemDotGauge);

    //@Alignment 설정
    if (UOverlaySlot* OverlaySlot = HPToolItemDotGaugeOverlay->AddChildToOverlay(HPToolItemDotGauge))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    HPToolItemDotGaugeRef = HPToolItemDotGauge;

    UE_LOGFMT(LogHUD, Log, "HPToolItemDotGauge가 성공적으로 생성되고 HPToolItemDotGaugeOverlay에 추가되었습니다.");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UPlayerHUD::OnUIVisibilityChanged_Implementation(UUserWidget* Widget, bool bVisible)
{
    if (Widget != this)
    {
        return;
    }

    if (bVisible)
    {
        SetVisibility(ESlateVisibility::HitTestInvisible);
        UE_LOGFMT(LogHUD, Log, "PlayerHUD가 표시되었습니다.");

        // TODO: HUD가 표시될 때 필요한 추가 로직

    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
        UE_LOGFMT(LogHUD, Log, "PlayerHUD가 숨겨졌습니다.");

        // TODO: HUD가 숨겨질 때 필요한 추가 로직

    }
}

void UPlayerHUD::OnStatusUIInitFinished()
{
    bStatusUIInitFinished = true;

    CheckAllUIsInitFinsiehd();
}

void UPlayerHUD::OnHPToolItemDotGaugeInitFinished()
{
    bHPToolItemDotGaugeInitFinished = true;

    CheckAllUIsInitFinsiehd();
}

void UPlayerHUD::OnQuickSlotsInitFinished()
{
    bQuickSlotsInitFinished = true;

    CheckAllUIsInitFinsiehd();
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion