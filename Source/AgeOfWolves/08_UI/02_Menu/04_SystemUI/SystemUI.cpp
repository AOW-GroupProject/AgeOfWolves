#include "SystemUI.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogSystemUI)

USystemUI::USystemUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void USystemUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    // �߰����� �ʱ�ȭ �۾�
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

void USystemUI::InitializeMenuUIContent(EMenuCategory Category)
{
    Super::InitializeMenuUIContent(Category);
    // SystemUI Ưȭ �ʱ�ȭ �۾� ����

    CheckMenuUIContentInitFinished();
}

void USystemUI::CheckMenuUIContentInitFinished() const
{
    Super::CheckMenuUIContentInitFinished();
    // �߰����� �ʱ�ȭ �Ϸ� üũ ����
}