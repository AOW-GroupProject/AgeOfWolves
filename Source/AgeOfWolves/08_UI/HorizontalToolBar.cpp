#include "HorizontalToolBar.h"
#include "Logging/StructuredLog.h"
#include "Components/HorizontalBox.h"
#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogHorizontalToolBar)

//@Default Setting
#pragma region Default Setting
UHorizontalToolBar::UHorizontalToolBar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , DefaultButtonIndex(0)
    , CurrentSelectedIndex(MAX_uint8)
    , ButtonBox(nullptr)
{
}

void UHorizontalToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UHorizontalToolBar::NativePreConstruct()
{
    Super::NativePreConstruct();
    SetIsFocusable(false);
}

void UHorizontalToolBar::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHorizontalToolBar::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UHorizontalToolBar::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    // 파생 클래스에서 처리하도록 기본 차단
    return FNavigationReply::Explicit(nullptr);
}

void UHorizontalToolBar::InitializeToolBar()
{
    //@버튼 생성
    CreateButtons();

    //@초기화 완료 이벤트
    ToolBarInitFinished.ExecuteIfBound();

    UE_LOGFMT(LogHorizontalToolBar, Log, "HorizontalToolBar 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
void UHorizontalToolBar::MoveLeft()
{
    MoveSelection(-1);
    UE_LOGFMT(LogHorizontalToolBar, Verbose, "왼쪽으로 이동");
}

void UHorizontalToolBar::MoveRight()
{
    MoveSelection(1);
    UE_LOGFMT(LogHorizontalToolBar, Verbose, "오른쪽으로 이동");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UHorizontalToolBar::OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    //@같은 버튼 재선택 무시
    if (CurrentSelectedIndex == ButtonIndex)
    {
        return;
    }

    //@이전 선택 취소
    if (IsValidButtonIndex(CurrentSelectedIndex))
    {
        CancelToolBarButtonSelected(CurrentSelectedIndex);
    }

    //@현재 인덱스 업데이트
    CurrentSelectedIndex = ButtonIndex;

    UE_LOGFMT(LogHorizontalToolBar, Log, "버튼 선택됨: 인덱스 {0}", ButtonIndex);
}

void UHorizontalToolBar::OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    UE_LOGFMT(LogHorizontalToolBar, Verbose, "버튼 호버됨: 인덱스 {0}", ButtonIndex);
}

void UHorizontalToolBar::OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex)
{
    UE_LOGFMT(LogHorizontalToolBar, Verbose, "버튼 언호버됨: 인덱스 {0}", ButtonIndex);
}

void UHorizontalToolBar::CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex)
{
    //@인덱스 유효성 검사
    if (!IsValidButtonIndex(PreviousIndex))
    {
        UE_LOGFMT(LogHorizontalToolBar, Warning, "유효하지 않은 인덱스: {0}", PreviousIndex);
        return;
    }

    //@이전 버튼 가져오기
    UCustomButton* PreviousButton = GetButtonByIndex(PreviousIndex);
    if (!PreviousButton)
    {
        UE_LOGFMT(LogHorizontalToolBar, Error, "버튼을 찾을 수 없음: 인덱스 {0}", PreviousIndex);
        return;
    }

    //@선택 취소
    PreviousButton->CancelSelectedButton();

    UE_LOGFMT(LogHorizontalToolBar, Log, "버튼 선택 취소됨: 인덱스 {0}", PreviousIndex);
}
#pragma endregion