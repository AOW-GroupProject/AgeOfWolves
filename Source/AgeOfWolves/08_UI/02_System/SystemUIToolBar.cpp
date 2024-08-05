#include "SystemUIToolBar.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogToolBar)

#pragma region Default Setting
USystemUIToolBar::USystemUIToolBar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void USystemUIToolBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@External Binding
	//@TODO: Exeternal Binding to Input Comp(Ű���� �Է�)


}

void USystemUIToolBar::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void USystemUIToolBar::NativeConstruct()
{
	Super::NativeConstruct();

}

void USystemUIToolBar::NativeDestruct()
{
	Super::NativeDestruct();

}

void USystemUIToolBar::InitializeToolBar()
{
	//@TODO: �ʱ�ȭ �۾�

	//@Delegate: �ʱ�ȭ �Ϸ� �̺�Ʈ
	ToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion
