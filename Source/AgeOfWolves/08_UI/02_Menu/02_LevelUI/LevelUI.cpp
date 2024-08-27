#include "LevelUI.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogLevelUI)

ULevelUI::ULevelUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    MenuCategory = EMenuCategory::Level;
}

void ULevelUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // 추가적인 초기화 작업
}

void ULevelUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void ULevelUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void ULevelUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void ULevelUI::InitializeMenuUIContent()
{
    Super::InitializeMenuUIContent();
    // LevelUI 특화 초기화 작업 수행

    CheckMenuUIContentInitFinished();
}

void ULevelUI::CheckMenuUIContentInitFinished()
{
    Super::CheckMenuUIContentInitFinished();
    // 추가적인 초기화 완료 체크 로직

}