// Fill out your copyright notice in the Description page of Project Settings.


#include "DotGaugeUnit.h"
#include "Logging/StructuredLog.h"

#include "Components/Image.h"

DEFINE_LOG_CATEGORY(LogDotGaugeUnit)

//@Defualt Setting
#pragma region Default Setting
UDotGaugeUnit::UDotGaugeUnit(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsFilled = true;
    bIsActive = false;
}

void UDotGaugeUnit::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@BG Image, Fill Image
    if (!BackgroundImage || !FillImage)
    {
        UE_LOGFMT(LogDotGaugeUnit, Warning, "이미지 위젯이 유효하지 않습니다.");
        return;
    }

    //@Texutr2D
    if (!DotGaugeUnitInformation.BackgroundImageTexture || !DotGaugeUnitInformation.FillImageTexture)
    {
        UE_LOGFMT(LogDotGaugeUnit, Warning, "텍스처가 유효하지 않습니다.");
        return;
    }

    //@BG Image
    BackgroundImage->SetBrushFromTexture(DotGaugeUnitInformation.BackgroundImageTexture);
    //@Fill Image
    FillImage->SetBrushFromTexture(DotGaugeUnitInformation.FillImageTexture);

    //@Deactivate
    DeactivateDotGaugeUnit();

    UE_LOGFMT(LogDotGaugeUnit, Log, "게이지 유닛 이미지가 초기화되었습니다.");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UDotGaugeUnit::ActivateDotGaugeUnit_Implementation()
{
    //@Visibility
    SetVisibility(ESlateVisibility::HitTestInvisible);

    //@TODO: Animation 추가
    SetIsActive(true);
}

void UDotGaugeUnit::DeactivateDotGaugeUnit_Implementation()
{
    //@Visibility
    SetVisibility(ESlateVisibility::Collapsed);

    //@TODO: Animation 추가
    SetIsActive(false);
}

void UDotGaugeUnit::UpdateDotGaugeUnit_Implementation(bool bFilled)
{
    //@bFilled
    if (bIsFilled == bFilled)
    {
        return;
    }

    //@Fill Image
    if (!FillImage)
    {
        UE_LOGFMT(LogDotGaugeUnit, Warning, "채우기 이미지가 유효하지 않습니다.");
        return;
    }

    //@bIsFilled
    bIsFilled = bFilled;
    //@Set Visibility
    FillImage->SetVisibility(bIsFilled ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

    UE_LOGFMT(LogDotGaugeUnit, Log, "게이지 유닛 상태가 업데이트 되었습니다. 채워짐: {0}", bIsFilled);
}

#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion