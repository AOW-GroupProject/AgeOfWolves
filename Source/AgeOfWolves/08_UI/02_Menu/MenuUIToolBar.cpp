#include "MenuUIToolBar.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogToolBar)

#pragma region Default Setting
UMenuUIToolBar::UMenuUIToolBar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UMenuUIToolBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@External Binding
	//@TODO: Exeternal Binding to Input Comp(키보드 입력)


}

void UMenuUIToolBar::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UMenuUIToolBar::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMenuUIToolBar::NativeDestruct()
{
	Super::NativeDestruct();

}

void UMenuUIToolBar::InitializeToolBar()
{
	//@TODO: 초기화 작업

	//@Delegate: 초기화 완료 이벤트
	ToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion
