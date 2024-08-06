#include "MapUI.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogMapUI)


UMapUI::UMapUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UMapUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@TODO: 외부 바인딩 수행
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

void UMapUI::InitializeMapUI()
{
    //@TODO: 초기화 작업 수행 

    MapUIInitFinished.ExecuteIfBound();
}
