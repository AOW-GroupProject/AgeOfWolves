// PlayerHUD.cpp

#include "PlayerHUD.h"
#include "Logging/StructuredLog.h"    

#include "08_UI/01_HUD/QuickSlots.h"
#include "08_UI/01_HUD/StateBars.h"

#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"

#include "04_Component/UIComponent.h"

DEFINE_LOG_CATEGORY(LogHUD)

#pragma region Default Setting
UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UPlayerHUD::NativeOnInitialized()
{
    Super::NativeOnInitialized();

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

void UPlayerHUD::InternalBindToStateBars(UStateBars* StateBars)
{
    if (!StateBars)
    {
        UE_LOGFMT(LogHUD, Error, "State Bars UI가 유효하지 않습니다.");
        return;
    }

    StateBars->StateBarsInitFinished.BindUFunction(this, "OnStateBarsInitFinished");
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

    UE_LOGFMT(LogHUD, Log, "PlayerHUD 초기화 시작");

    //@Init
    CreateStateBars();
    CreateQuickSlots();
    //@초기화 요청
    RequestStartInitByHUD.Broadcast();

    UE_LOGFMT(LogHUD, Log, "PlayerHUD 초기화 완료");

}
#pragma endregion

#pragma region Subwidgets
void UPlayerHUD::CreateStateBars()
{
    //@Scale Box
    if (!IsValid(StateBarsBox))
    {
        UE_LOGFMT(LogHUD, Error, "StateBarsBox가 유효하지 않습니다.");
        return;
    }
    //@BP Class
    if (!ensureMsgf(StateBarsClass, TEXT("StateBarsClass가 설정되지 않았습니다.")))
    {
        return;
    }
    //@Create Widget
    UStateBars* StateBars = CreateWidget<UStateBars>(this, StateBarsClass);
    //@State Bars
    if (!IsValid(StateBars))
    {
        UE_LOGFMT(LogHUD, Error, "StateBars 위젯 생성에 실패했습니다.");
        return;
    }
    //@Binding(초기화 작업 비동기화 위한 바인딩)
    RequestStartInitByHUD.AddUFunction(StateBars, "InitializeStateBars");
    //@Internal Binding
    InternalBindToStateBars(StateBars);
    //@Allignment
    UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(StateBarsBox->AddChild(StateBars));
    if (ScaleBoxSlot)
    {
        ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);

        UE_LOGFMT(LogHUD, Log, "StateBars 위젯이 성공적으로 추가되었습니다.");
    }
}

void UPlayerHUD::CreateQuickSlots()
{
    //@Scale Box
    if (!IsValid(QuickSlotsBox))
    {
        UE_LOGFMT(LogHUD, Error, "QuickSlotsBox가 유효하지 않습니다.");
        return;
    }
    //@BP Class
    if (!ensureMsgf(QuickSlotsClass, TEXT("QuickSlotsClass가 설정되지 않았습니다.")))
    {
        return;
    }
    //@Create Widget
    UQuickSlots* QuickSlots = CreateWidget<UQuickSlots>(this, QuickSlotsClass);
    if (!IsValid(QuickSlots))
    {
        UE_LOGFMT(LogHUD, Error, "QuickSlots 위젯 생성에 실패했습니다.");
        return;
    }
    //@Binding(초기화 작업 동기화를 위한 바인딩)
    RequestStartInitByHUD.AddUFunction(QuickSlots, "InitializeQuickSlots");
    //@Internal Binding
    InternalBindToQuickSlots(QuickSlots);
    //@Alignment
    UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(QuickSlotsBox->AddChild(QuickSlots));
    if (ScaleBoxSlot)
    {
        ScaleBoxSlot->SetHorizontalAlignment(HAlign_Right);
        ScaleBoxSlot->SetVerticalAlignment(VAlign_Bottom);

        UE_LOGFMT(LogHUD, Log, "QuickSlots 위젯이 성공적으로 추가되었습니다.");
    }
}

UQuickSlots* UPlayerHUD::GetQuickSlotsUI() const
{
    if (!QuickSlotsBox)
    {
        UE_LOGFMT(LogHUD, Warning, "Quick Slots Box가 유효하지 않습니다. : {0}", __FUNCTION__);
        return nullptr;
    }

    UWidget* ChildWidget = QuickSlotsBox->GetChildAt(0);
    if (!ChildWidget)
    {
        UE_LOGFMT(LogHUD, Warning, "Quick Slot Box에 유효한 자식 Widget이 존재하지 않습니다.");
        return nullptr;
    }
   
    auto QuickSlots = Cast<UQuickSlots>(ChildWidget);
    if(!QuickSlots)
    {
        UE_LOGFMT(LogHUD, Warning, "Casting Failed : {0}", __FUNCTION__);
        return nullptr;
    }

    return QuickSlots;
}

UStateBars* UPlayerHUD::GetStateBarsUI() const
{
    if (!StateBarsBox)
    {
        UE_LOGFMT(LogHUD, Warning, "State Bars Box가 유효하지 않습니다. : {0}", __FUNCTION__);
        return nullptr;
    }

    UWidget* ChildWidget = StateBarsBox->GetChildAt(0);
    if (!ChildWidget)
    {
        UE_LOGFMT(LogHUD, Warning, "State Bar Box에 유효한 자식 Widget이 존재하지 않습니다.");
        return nullptr;
    }

    auto StateBars = Cast<UStateBars>(ChildWidget);
    if (!StateBars)
    {
        UE_LOGFMT(LogHUD, Warning, "Casting Failed : {0}", __FUNCTION__);
        return nullptr;
    }

    return StateBars;
}
#pragma endregion

#pragma region Callbacks
void UPlayerHUD::OnStateBarsInitFinished()
{
    //@Deelgate
    NotifyStateBarsInitFinished.ExecuteIfBound();
}

void UPlayerHUD::OnQuickSlotsInitFinished()
{
    //@Delgate
    NotifyQuickSlotsInitFinished.ExecuteIfBound();

}
#pragma endregion