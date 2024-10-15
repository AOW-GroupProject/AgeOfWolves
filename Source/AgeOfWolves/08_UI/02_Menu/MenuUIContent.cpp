#include "MenuUIContent.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogMenuUIContent)

//@Defualt Setting
#pragma region Default Setting
UMenuUIContent::UMenuUIContent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//@하위 클래스는 Menu Category를 여기서 설정해줍니다...
}

void UMenuUIContent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UMenuUIContent::NativePreConstruct()
{
	Super::NativePreConstruct();

	SetIsFocusable(false);

}

void UMenuUIContent::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMenuUIContent::NativeDestruct()
{
	Super::NativeDestruct();

}

FNavigationReply UMenuUIContent::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	return FNavigationReply::Explicit(nullptr);
}

FReply UMenuUIContent::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	// 모든 포커스 시도를 로깅
	UE_LOGFMT(LogMenuUIContent, Log, "포커스 시도: 위젯: {0}, 원인: {1}",
		*GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

	// SetDirectly만 허용하고 나머지는 거부
	if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
	{
		return FReply::Unhandled();
	}

	return FReply::Handled();
}

void UMenuUIContent::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	// SetDirectly가 아닌 경우 포커스 유지
	if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
	{
		UE_LOGFMT(LogMenuUIContent, Log, "포커스 유지: 위젯: {0}, 원인: {1}",
			*GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
		return;
	}

	Super::NativeOnFocusLost(InFocusEvent);

	UE_LOGFMT(LogMenuUIContent, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
		*GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
}

FReply UMenuUIContent::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Unhandled();
}

FReply UMenuUIContent::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UMenuUIContent::InitializeMenuUIContent()
{

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

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UMenuUIContent::OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType)
{
	//@가시성 변화 시 수행해야할 동작들 아래에서 작성...

	//@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
