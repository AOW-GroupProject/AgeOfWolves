
#include "DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "08_UI/DropDownMenuOption.h"

DEFINE_LOG_CATEGORY(LogDropDownMenu)

//@Default Settings
#pragma region Default Setting
UDropDownMenu::UDropDownMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    DropDownMenuOptionBox = nullptr;
    CurrentSelectedOption = FName();
}

void UDropDownMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();

}

void UDropDownMenu::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UDropDownMenu::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDropDownMenu::NativeDestruct()
{
    Super::NativeDestruct();
}

void UDropDownMenu::InternalBindToOptions(UDropDownMenuOption* Option, const FName& OptionName, bool bIsLastOption)
{
    if (!Option)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "Option이 null입니다. 바인딩을 수행할 수 없습니다.");
        return;
    }

    //@마지막 옵션인 경우에만 DropDownMenuOptionInitFinished 이벤트 바인딩
    if (bIsLastOption)
    {
        Option->DropDownMenuOptionInitFinished.BindUFunction(this, "OnDropDownMenuOptionInitFinished");
    }

    Option->DropDownMenuOptionSelected.AddUObject(this, &UDropDownMenu::OnDropDownMenuOptionSelected, OptionName);

    UE_LOGFMT(LogDropDownMenu, Verbose, "Option({0})에 대한 내부 바인딩 완료", *OptionName.ToString());
}

void UDropDownMenu::InitializeDropDownMenu()
{
    //@Create Options
    CreateDropDownMenuOptions();
    //@Close, 게임 시작 시점엔 우선 숨겨줍니다.
    CloseDropDownMenu();
    //@초기화 요청 이벤트
    RequestStartInitByDropDownMenu.Broadcast();
}

void UDropDownMenu::CheckAllUIsInitFinished()
{
    if (bOptionsInitFinished)
    {
        bOptionsInitFinished = false;

        //@초기화 완료 이벤트
        DropDownMenuInitFinished.ExecuteIfBound();
    }
}

#pragma endregion

//@Property/Info...etc
#pragma region Subwidgets
void UDropDownMenu::ResetDropDownMenu()
{
    //@TODO: Drop Donw Menu의 초기 설정에 필요한 동작들...


    //@이전에 선택된 옴션 버튼의 선택 취소 이벤트
    if (!CurrentSelectedOption.IsNone())
    {
        //@이전 선택된 옵션의 버튼 선택 취소 이벤트
        FName PrevSelectedOption = CurrentSelectedOption;
        CancelDropDownMenuOptionButton.Broadcast(PrevSelectedOption);

        //@Current Selected Option
        CurrentSelectedOption = FName();

        UE_LOGFMT(LogDropDownMenu, Log, "이전에 선택된 옵션 취소: {0}", PrevSelectedOption.ToString());
    }

    

}

void UDropDownMenu::CreateDropDownMenuOptions()
{
    //@DropDownMenuOptionBox, DropDownMenuBGImage
    if (!DropDownMenuOptionBox || !DropDownMenuBGImage)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "필요한 위젯 컴포넌트가 null입니다.");
        return;
    }

    //@기존 옵션 제거
    DropDownMenuOptionBox->ClearChildren();

    //@전체 높이 계산을 위한 변수
    float TotalHeight = 0.f;

    //@옵션 추가
    int32 OptionCount = OptionInformations.Num();
    for (int32 i = 0; i < OptionCount; ++i)
    {
        //@Drop Down Menu Option의 블루프린트 클래스
        TSubclassOf<UDropDownMenuOption> OptionClass = OptionInformations[i].GetOptionClass();
        if (!OptionClass)
        {
            UE_LOGFMT(LogDropDownMenu, Error, "Option {0}의 OptionClass가 설정되지 않았습니다.", i);
            continue;
        }
        //@Drop Down Menu Option
        UDropDownMenuOption* Option = CreateWidget<UDropDownMenuOption>(this, OptionClass);
        if (!Option)
        {
            UE_LOGFMT(LogDropDownMenu, Error, "UDropDownMenuOption 위젯을 생성하지 못했습니다. OptionClass: {0}", *OptionClass->GetName());
            continue;
        }

        //@Set Option Name
        Option->SetOptionName(FText::FromName(OptionInformations[i].GetOptionName()));
        //@Set Option Hot Key Text
        Option->SetDropDownMenuOptionHotKeyText(OptionInformations[i].GetOptionHotKeyText());

        //@비동기 초기화 이벤트
        RequestStartInitByDropDownMenu.AddUFunction(Option, "InitializeDropDownMenuOption");

        //@Option Button 선택 취소 이벤트
        CancelDropDownMenuOptionButton.AddUFunction(Option, "DropDownMenuOptionButtonCanceledNotified");

        //@내부 바인딩 (마지막 옵션인 경우에만 DropDownMenuOptionInitFinished 이벤트 바인딩)
        InternalBindToOptions(Option, OptionInformations[i].GetOptionName(), i == OptionCount - 1);

        //@AddChildToVerticalBox
        UVerticalBoxSlot* VerticalBoxSlot = DropDownMenuOptionBox->AddChildToVerticalBox(Option);
        if (VerticalBoxSlot)
        {
            //@Alignment
            VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            VerticalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            VerticalBoxSlot->SetVerticalAlignment(VAlign_Fill);

            //@좌/우 패딩
            float LeftRightPadding = Option->GetLeftRightPadding();

            //@상하 패딩 설정 (첫 번째와 마지막 옵션에 대해 특별 처리)
            float TopPadding = (i == 0) ? Option->GetUpUnderPadding() : 0.0f;
            float BottomPadding = (i == OptionCount - 1) ? Option->GetUpUnderPadding() : 0.0f;

            VerticalBoxSlot->SetPadding(FMargin(LeftRightPadding, TopPadding, LeftRightPadding, BottomPadding));

            //@Vertical Box의 Height 계산
            TotalHeight += Option->GetOptionHeight() + TopPadding + BottomPadding;
        }

        UE_LOGFMT(LogDropDownMenu, Log, "DropDownMenuOption({0})이 생성되고 이벤트가 바인딩되었습니다.", OptionInformations[i].GetOptionName().ToString());
    }

    //@Brush.X
    float BrushWidth = DropDownMenuBGImage->GetBrush().GetImageSize().X;

    //@Set Brush Size
    FVector2D NewSize(BrushWidth, TotalHeight);
    DropDownMenuBGImage->SetDesiredSizeOverride(NewSize);

    //@Set Desired Size In Viewport
    SetDesiredSizeInViewport(NewSize);

    UE_LOGFMT(LogDropDownMenu, Log, "DropDownMenu 옵션들이 생성되었고, 배경 크기가 조정되었습니다. 크기: {0}", NewSize.ToString());
}

void UDropDownMenu::OpenDropDownMenu_Implementation()
{
    //@Self Hit Test Invisible
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    UE_LOGFMT(LogDropDownMenu, Log, "Drop Down Menu 가 열렸습니다.");

    //@TODO: Animation 관련 작업 수행 시 오버라이딩...

}

void UDropDownMenu::CloseDropDownMenu_Implementation()
{
    //@Collapsed
    SetVisibility(ESlateVisibility::Collapsed);

    //@Reset
    ResetDropDownMenu();

    UE_LOGFMT(LogDropDownMenu, Log, "Drop Down Menu 가 닫혔습니다.");

    //@TODO: Animation 관련 작업 수행 시 오버라이딩...

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UDropDownMenu::OnDropDownMenuOptionInitFinished()
{

    bOptionsInitFinished = true;

    //@초기화 완료 체크
    CheckAllUIsInitFinished();
}

void UDropDownMenu::OnDropDownMenuOptionSelected(FName SelectedOptionName)
{
    UE_LOGFMT(LogDropDownMenu, Log, "드롭다운 메뉴 옵션 선택됨: {0}", SelectedOptionName.ToString());

    //@이미 선택된 옵션을 한번 더 선택했을 경우, 무시
    if (CurrentSelectedOption == SelectedOptionName)
    {
        UE_LOGFMT(LogDropDownMenu, Log, "이미 선택된 옵션입니다. 처리를 무시합니다: {0}", SelectedOptionName.ToString());
        return;
    }

    //@이전에 선택된 옴션 버튼의 선택 취소 이벤트
    if (!CurrentSelectedOption.IsNone())
    {
        FName PrevSelectedOption = CurrentSelectedOption;
        CancelDropDownMenuOptionButton.Broadcast(PrevSelectedOption);

        UE_LOGFMT(LogDropDownMenu, Log, "이전에 선택된 옵션 취소: {0}", PrevSelectedOption.ToString());
    }

    //@Option Informations
    FDropDownMenuOptionInformation* SelectedOptionInformation = OptionInformations.FindByPredicate([&](const FDropDownMenuOptionInformation& Option) {
        return Option.GetOptionName() == SelectedOptionName;
        });

    //@FDropDownMenuOptionInformation
    if (!SelectedOptionInformation)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "선택된 옵션을 찾을 수 없습니다: {0}", SelectedOptionName.ToString());
        return;
    }

    //@Current Selected Option
    CurrentSelectedOption = SelectedOptionName;
    UE_LOGFMT(LogDropDownMenu, Log, "새로운 옵션이 선택됨: {0}", SelectedOptionName.ToString());

    //@옵션 버튼 선택 이벤트 호출
    DropDownMenuOptionButtonClicked.Broadcast(SelectedOptionName);

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FORCEINLINE const FText UDropDownMenu::GetConfirmationMenuDialogueText(const FName& Name) const
{
    const FDropDownMenuOptionInformation* FoundOption = OptionInformations.FindByPredicate([&Name](const FDropDownMenuOptionInformation& Option) {
        return Option.CompareOptionName(Name);
        });

    if (FoundOption)
    {
        return FoundOption->GetConfirmationMenuDialogueText();
    }

    return FText::GetEmpty();
}
#pragma endregion
