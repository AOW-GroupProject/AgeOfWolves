#include "HorizontalToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogHorizontalToolBar)

//@Default Setting
#pragma region Default Setting
UHorizontalToolBar::UHorizontalToolBar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , DefaultButtonIndex(0)
    , CurrentSelectedIndex(MAX_uint8)
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
    return FNavigationReply::Explicit(nullptr);
}

void UHorizontalToolBar::InitializeToolBar()
{
    //@Create Buttons
    CreateButtons();

    //@초기화 완료 이벤트
    ToolBarInitFinished.ExecuteIfBound();
}

void UHorizontalToolBar::CheckToolBarInitialization()
{
    //@파생 클래스에서 구현
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets


void UHorizontalToolBar::CreateButtons()
{
    if (!ButtonBox)
    {
        UE_LOGFMT(LogHorizontalToolBar, Error, "ButtonBox가 유효하지 않습니다.");
        return;
    }

    ButtonBox->ClearChildren();
    MButtons.Empty();

    //@파생 클래스에서 구현
}

void UHorizontalToolBar::MoveLeft()
{
    MoveSelection(-1);
}

void UHorizontalToolBar::MoveRight()
{
    MoveSelection(1);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UHorizontalToolBar::OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    if (CurrentSelectedIndex == ButtonIndex) return;

    //@선택된 버튼 취소
    CancelToolBarButtonSelected(CurrentSelectedIndex);

    //@Current Index
    CurrentSelectedIndex = ButtonIndex;

    //@파생 클래스에서 추가 처리 필요
}

void UHorizontalToolBar::OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    //@파생 클래스에서 구현
}

void UHorizontalToolBar::OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex)
{
    //@파생 클래스에서 구현
}

void UHorizontalToolBar::CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex)
{
    UE_LOGFMT(LogHorizontalToolBar, Log, "{0}", PreviousIndex);

    auto PreviousButton = MButtons.FindRef(PreviousIndex);
    if (!PreviousButton)
    {
        UE_LOGFMT(LogHorizontalToolBar, Error, "Button이 유효하지 않습니다!");
        return;
    }

    PreviousButton->CancelSelectedButton();

    UE_LOGFMT(LogHorizontalToolBar, Log, "{0} 인덱스의 버튼이 취소되었습니다.", PreviousIndex);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
int32 UHorizontalToolBar::GetCurrentButtonIndex() const
{
    TArray<uint8> ButtonIndices;
    MButtons.GetKeys(ButtonIndices);
    return ButtonIndices.IndexOfByKey(CurrentSelectedIndex);
}
#pragma endregion