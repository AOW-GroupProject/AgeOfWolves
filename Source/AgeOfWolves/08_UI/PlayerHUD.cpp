// PlayerHUD.cpp

#include "PlayerHUD.h"
#include "Logging/StructuredLog.h"    

#include "08_UI/QuickSlots.h"
#include "08_UI/StateBars.h"

#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"

#include "04_Component/UIComponent.h"

DEFINE_LOG_CATEGORY(LogHUD)

UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UPlayerHUD::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    UE_LOGFMT(LogHUD, Log, "PlayerHUD 초기화 시작");

    InitializeStateBars();
    InitializeQuickSlots();

    UE_LOGFMT(LogHUD, Log, "PlayerHUD 초기화 완료");
}

void UPlayerHUD::NativePreConstruct()
{
    Super::NativePreConstruct();

}

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOGFMT(LogHUD, Log, "PlayerHUD 구성 완료");
}

void UPlayerHUD::NativeDestruct()
{
    Super::NativeDestruct();

    UE_LOGFMT(LogHUD, Log, "PlayerHUD 소멸");
}

void UPlayerHUD::InitializeStateBars()
{
    if (IsValid(StateBarsBox))
    {
        if (ensureMsgf(StateBarsClass, TEXT("StateBarsClass가 설정되지 않았습니다.")))
        {
            UStateBars* StateBars = CreateWidget<UStateBars>(this, StateBarsClass);
            if (IsValid(StateBars))
            {
                if (UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(StateBarsBox->AddChild(StateBars)))
                {
                    ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
                    ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
                    UE_LOGFMT(LogHUD, Log, "StateBars 위젯이 성공적으로 추가되었습니다.");
                }
            }
            else
            {
                UE_LOGFMT(LogHUD, Error, "StateBars 위젯 생성에 실패했습니다.");
            }
        }
    }
    else
    {
        UE_LOGFMT(LogHUD, Error, "StateBarsBox가 유효하지 않습니다.");
    }
}

void UPlayerHUD::InitializeQuickSlots()
{
    if (IsValid(QuickSlotsBox))
    {
        if (ensureMsgf(QuickSlotsClass, TEXT("QuickSlotsClass가 설정되지 않았습니다.")))
        {
            UQuickSlots* QuickSlots = CreateWidget<UQuickSlots>(this, QuickSlotsClass);
            if (IsValid(QuickSlots))
            {
                if (UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(QuickSlotsBox->AddChild(QuickSlots)))
                {
                    ScaleBoxSlot->SetHorizontalAlignment(HAlign_Right);
                    ScaleBoxSlot->SetVerticalAlignment(VAlign_Bottom);
                    UE_LOGFMT(LogHUD, Log, "QuickSlots 위젯이 성공적으로 추가되었습니다.");
                }
            }
            else
            {
                UE_LOGFMT(LogHUD, Error, "QuickSlots 위젯 생성에 실패했습니다.");
            }
        }
    }
    else
    {
        UE_LOGFMT(LogHUD, Error, "QuickSlotsBox가 유효하지 않습니다.");
    }
}

UQuickSlots* UPlayerHUD::GetQuickSlotsWidget() const
{
    if (IsValid(QuickSlotsBox))
    {
        UWidget* ChildWidget = QuickSlotsBox->GetChildAt(0);
        return Cast<UQuickSlots>(ChildWidget);
    }

    UE_LOGFMT(LogHUD, Warning, "QuickSlotsBox가 유효하지 않거나 자식 위젯이 없습니다.");
    return nullptr;
}