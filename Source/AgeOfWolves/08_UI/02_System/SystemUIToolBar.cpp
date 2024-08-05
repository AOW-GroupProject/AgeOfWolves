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
	//@TODO: Exeternal Binding to Input Comp(키보드 입력)


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
	//@TODO: 초기화 작업

	//@Delegate: 초기화 완료 이벤트
	ToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion
