#include "MenuUIContent.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogMenuUIContent)

UMenuUIContent::UMenuUIContent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UMenuUIContent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UMenuUIContent::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UMenuUIContent::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMenuUIContent::NativeDestruct()
{
	Super::NativeDestruct();

}

void UMenuUIContent::InitializeMenuUIContent(EMenuCategory Category)
{
	//@Menu Category
	MenuCategory = Category;

	//@TODO: 내부 Subwidget 생성
	
	//@TODO: 초기화 요청 이벤트 호출
	
	//@TOOD: 내부 멤버들 결정 및 내부 바인딩 수행하면 지워줍니다.
	//CheckMenuUIContentInitFinished();
}

void UMenuUIContent::CheckMenuUIContentInitFinished() const
{
	//@TODO: Internal Binding 수행 후 Callback들로 부터 체크된 항목들 모두 참인지 확인

	//@초기화 완료 이벤트 호출
	if (MenuCategory == EMenuCategory::MAX)
	{
		UE_LOGFMT(LogMenuUIContent, Warning, "할당 받은 Menu Category가 유효하지 않습니다.");
		return;
	}

	//@초기화 완료 이벤트 호출
	MenuUIContentInitFinished.ExecuteIfBound(MenuCategory);
}
