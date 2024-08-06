
#include "SystemUI.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogSystemUI)


USystemUI::USystemUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void USystemUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@TODO: �ܺ� ���ε� ���� ���
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
    //@TODO: �ʱ�ȭ �۾� ����

    SystemUIInitFinished.ExecuteIfBound();
}
