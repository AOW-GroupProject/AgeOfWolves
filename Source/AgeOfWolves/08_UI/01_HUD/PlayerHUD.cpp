// PlayerHUD.cpp

#include "PlayerHUD.h"
#include "Logging/StructuredLog.h"    

#include "04_Component/UIComponent.h"

#include "08_UI/01_HUD/QuickSlots.h"
#include "08_UI/01_HUD/StateBars.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "08_UI/01_HUD/HUD_StatusUI.h"

#include "04_Component/UIComponent.h"

DEFINE_LOG_CATEGORY(LogHUD)

//@Defualt Setting
#pragma region Default Setting
UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    StatusUIRef = nullptr;
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

void UPlayerHUD::InternalBindToQuickSlots(UQuickSlots* QuickSlots)
{

    if (!QuickSlots)
    {
        UE_LOGFMT(LogHUD, Error, "Quick Slots UI가 유효하지 않습니다.");
        return;
    }
    //@Internal Binding
    QuickSlots->QuickSlotsInitFinished.BindUFunction(this, "OnQuickSlotsInitFinished");
}

void UPlayerHUD::InitializePlayerHUD()
{
    // @TODO: 임시, 초기화 완료 체크 함수들로 이동 예정
    CreateStatusUI();

    RequestStartInitByHUD.Broadcast();

    OnQuickSlotsInitFinished();
}

void UPlayerHUD::CheckAllUIsInitFinsiehd()
{
    if (bStatusUIInitFinished && bQuickSlotsInitFinished)
    {
        bStatusUIInitFinished = false;
        bQuickSlotsInitFinished = false;

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
}

void UPlayerHUD::CreateHPPotionUI()
{
}
#pragma endregion

//@Delegates
#pragma region Delegates
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

void UPlayerHUD::OnQuickSlotsInitFinished()
{
    bQuickSlotsInitFinished = true;

    CheckAllUIsInitFinsiehd();
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion