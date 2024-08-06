
#include "SystemUI.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogSystemUI)


USystemUI::USystemUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void USystemUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@TODO: 외부 바인딩 수행 장소
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

void USystemUI::InitializeSystemUI()
{
    //@TODO: 초기화 작업 수행

    SystemUIInitFinished.ExecuteIfBound();
}
