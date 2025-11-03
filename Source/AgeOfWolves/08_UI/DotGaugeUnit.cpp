#include "DotGaugeUnit.h"
#include "Logging/StructuredLog.h"
#include "Components/Image.h"

DEFINE_LOG_CATEGORY(LogDotGaugeUnit)

//@Defualt Setting
#pragma region Default Setting
UDotGaugeUnit::UDotGaugeUnit(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsFilled = false;
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

    //@Texture2D
    if (!DotGaugeUnitInformation.BackgroundImageTexture || !DotGaugeUnitInformation.FillImageTexture)
    {
        UE_LOGFMT(LogDotGaugeUnit, Warning, "텍스처가 유효하지 않습니다.");
        return;
    }

    //@BG Image
    BackgroundImage->SetBrushFromTexture(DotGaugeUnitInformation.BackgroundImageTexture);

    //@Fill Image
    FillImage->SetBrushFromTexture(DotGaugeUnitInformation.FillImageTexture);
    FillImage->SetVisibility(ESlateVisibility::Collapsed);  // ✅ 초기에 숨김

    //@초기 상태
    bIsFilled = false;

    //@Deactivate
    DeactivateDotGaugeUnit();

    UE_LOGFMT(LogDotGaugeUnit, Log, "게이지 유닛 이미지가 초기화되었습니다.");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UDotGaugeUnit::ActivateDotGaugeUnit_Implementation()
{
    //@Visibility 설정
    SetVisibility(ESlateVisibility::HitTestInvisible);

    //@상태 동기화
    bIsActive = true;

    //@TODO: Animation 추가

    UE_LOGFMT(LogDotGaugeUnit, Log, "게이지 유닛 활성화됨");
}

void UDotGaugeUnit::DeactivateDotGaugeUnit_Implementation()
{
    //@Visibility 설정
    SetVisibility(ESlateVisibility::Collapsed);

    //@상태 동기화
    bIsActive = false;
    bIsFilled = false;  // 비활성화 시 Filled 상태도 초기화

    //@Fill Image도 숨김
    if (FillImage)
    {
        FillImage->SetVisibility(ESlateVisibility::Collapsed);
    }

    //@TODO: Animation 추가

    UE_LOGFMT(LogDotGaugeUnit, Log, "게이지 유닛 비활성화됨");
}

void UDotGaugeUnit::UpdateDotGaugeUnit_Implementation(bool bFilled)
{
    //@이미 같은 상태면 무시
    if (bIsFilled == bFilled)
    {
        return;
    }

    //@Fill Image 체크
    if (!FillImage)
    {
        UE_LOGFMT(LogDotGaugeUnit, Warning, "채우기 이미지가 유효하지 않습니다.");
        return;
    }

    //@Active 상태가 아니면 업데이트 불가
    if (!bIsActive)
    {
        UE_LOGFMT(LogDotGaugeUnit, Warning, "비활성화 상태에서는 Filled 상태를 변경할 수 없습니다.");
        return;
    }

    //@상태 업데이트
    bIsFilled = bFilled;

    //@Visibility 설정
    FillImage->SetVisibility(bIsFilled ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

    //@TODO: Animation 추가

    UE_LOGFMT(LogDotGaugeUnit, Log, "게이지 유닛 상태 업데이트됨. Filled: {0}", bIsFilled);
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