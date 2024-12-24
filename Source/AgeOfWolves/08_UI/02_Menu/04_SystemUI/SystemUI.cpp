#include "SystemUI.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogSystemUI)

USystemUI::USystemUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    MenuCategory = EMenuCategory::System;
}

void USystemUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // 추가적인 초기화 작업
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

void USystemUI::InitializeMenuUIContent()
{
    Super::InitializeMenuUIContent();
    // SystemUI 특화 초기화 작업 수행

    CheckMenuUIContentInitFinished();
}

void USystemUI::CheckMenuUIContentInitFinished()
{
    Super::CheckMenuUIContentInitFinished();
    // 추가적인 초기화 완료 체크 로직
}