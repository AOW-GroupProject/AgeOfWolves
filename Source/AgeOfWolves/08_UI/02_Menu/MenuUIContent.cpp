#include "MenuUIContent.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogMenuUIContent)
#pragma region Default Setting

UMenuUIContent::UMenuUIContent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//@하위 클래스는 Menu Category를 여기서 설정해줍니다...
}

void UMenuUIContent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@가시성 변화 이벤트에 바인딩
	OnVisibilityChanged.AddDynamic(this, &UMenuUIContent::OnUIVisibilityChanged);
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

void UMenuUIContent::CheckMenuUIContentInitFinished()
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
#pragma endregion



