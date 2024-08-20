#include "MapUI.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogMapUI)

UMapUI::UMapUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UMapUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // 추가적인 초기화 작업
}

void UMapUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UMapUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMapUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void UMapUI::InitializeMenuUIContent(EMenuCategory Category)
{
    Super::InitializeMenuUIContent(Category);
    // MapUI 특화 초기화 작업 수행

    CheckMenuUIContentInitFinished();
}

void UMapUI::CheckMenuUIContentInitFinished() const
{
    Super::CheckMenuUIContentInitFinished();
    // 추가적인 초기화 완료 체크 로직
}