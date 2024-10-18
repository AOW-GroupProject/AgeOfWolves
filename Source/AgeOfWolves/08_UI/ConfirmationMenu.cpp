#include "ConfirmationMenu.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/MultiLineEditableTextBox.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogConfirmationMenu)

//@Default Settings
#pragma region Default Setting
UConfirmationMenu::UConfirmationMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    CurrentHoveredConfirmationButton = nullptr;
    CurrentSelectedConfirmationButton = nullptr;
}

void UConfirmationMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UConfirmationMenu::NativePreConstruct()
{
    Super::NativePreConstruct();

    //@Keyboard Focus 가능 여부
    SetIsFocusable(true);
}

void UConfirmationMenu::NativeConstruct()
{
    Super::NativeConstruct();
}

void UConfirmationMenu::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UConfirmationMenu::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);
}

FReply UConfirmationMenu::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    //@Set Directly(SetFocus())를 통한 포커스 시도 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return FReply::Handled().ClearUserFocus();
    }

    UE_LOGFMT(LogConfirmationMenu, Log, "포커스 : 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

    return FReply::Handled();
}

void UConfirmationMenu::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    //@SetDirectly(SetFocus()) 혹은 Mouse를 통한 포커스 소실 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        //@Set Focus
        SetFocus();

        return;
    }

    Super::NativeOnFocusLost(InFocusEvent);

    UE_LOGFMT(LogConfirmationMenu, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
}

FReply UConfirmationMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogConfirmationMenu, Log, "키 입력 감지됨: {0}", Key.ToString());

    //@방향키 조작
    if (Key == EKeys::Left || Key == EKeys::Right)
    {
        //@좌우 방향키 조작 처리
        HandleHorizontalDirectionalInput(Key == EKeys::Left ? -1 : 1);

        return FReply::Handled();
    }
    else if (Key == EKeys::Enter)
    {
        //@Current Hovered Option 선택
        SelectCurrentHoveredOptionByKeyboard();

        return FReply::Handled();
    }

    UE_LOGFMT(LogConfirmationMenu, Log, "확정 메뉴에서 처리하지 않는 키 입력: {0}", Key.ToString());

    //@이외 키 입력은 처리되지 않았고, 다른 UI에서 키 입력 처리를 막습니다.
    return FReply::Handled();
}

void UConfirmationMenu::InternalBindToButton(UCustomButton* Button, FName MenuButtonName)
{
    if (!Button)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "Button이 유효하지 않습니다.");
        return;
    }

    Button->ButtonSelected.AddUObject(this, &UConfirmationMenu::OnConfirmationMenuButtonClicked, MenuButtonName);
    Button->ButtonHovered.AddUObject(this, &UConfirmationMenu::OnConfirmationMenuButtonHovered, MenuButtonName);
    Button->ButtonUnhovered.AddUObject(this, &UConfirmationMenu::OnConfirmationMenuButtonUnhovered, MenuButtonName);
}

void UConfirmationMenu::InitializeConfirmationMenu()
{
    //@Button
    CreateButton();
    //@Reset
    CloseConfirmationMenu();
    //@초기화 완료 이벤트
    ConfirmationMenuInitFinished.ExecuteIfBound();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UConfirmationMenu::ResetConfirmationMenu()
{
    //@Current Selected Confirmation Button
    if (CurrentSelectedConfirmationButton.Get())
    {
        FName PreviousMenuButtonName;
        for (const auto& Pair : MConfirmationMenuButtons)
        {
            if (Pair.Value == CurrentSelectedConfirmationButton)
            {
                PreviousMenuButtonName = Pair.Key;
                break;
            }
        }
        CancelConfirmationMenuButtonSelected(PreviousMenuButtonName);
    }

    //@Current Hovered Confirmation Button
    if (CurrentHoveredConfirmationButton.Get())
    {
        //@Cancel Hovered Menu Button Selected
        FName PreviousMenuButtonName;
        for (const auto& Pair : MConfirmationMenuButtons)
        {
            if (Pair.Value == CurrentHoveredConfirmationButton)
            {
                PreviousMenuButtonName = Pair.Key;
                break;
            }
        }
        CancelConfirmationMenuButtonSelected(PreviousMenuButtonName);
        
    }

}

void UConfirmationMenu::CreateButton()
{
    //@필수 컴포넌트 확인
    if (!ConfirmationMenuOverlay || !ConfirmationMenuOkButtonOverlay || !ConfirmationMenuCancelButtonOverlay)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "필요한 UI 컴포넌트가 null입니다. 위젯 블루프린트에서 올바르게 설정되었는지 확인하세요.");
        return;
    }

    //@버튼 클래스 확인
    if (!ConfirmationMenuOkButtonClass || !ConfirmationMenuCancelButtonClass)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "ConfirmationMenuButtonClass가 설정되지 않았습니다. 에디터에서 ConfirmationMenuButtonClass를 설정해주세요.");
        return;
    }

    //@기존 이미지 제거 및 저장
    UWidget* OkButtonImage = ConfirmationMenuOkButtonOverlay->GetChildAt(0);
    UWidget* CancelButtonImage = ConfirmationMenuCancelButtonOverlay->GetChildAt(0);
    ConfirmationMenuOkButtonOverlay->RemoveChildAt(0);
    ConfirmationMenuCancelButtonOverlay->RemoveChildAt(0);

    //@버튼 키 배열
    TArray<FName> ConfirmationMenuButtonNames = { FName("OK"), FName("CANCEL") };

    //@버튼 생성 및 저장
    for (const FName& ConfirmationMenuButtonName : ConfirmationMenuButtonNames)
    {
        //@Create Widget
        UCustomButton* ConfirmationMenuButton = CreateWidget<UCustomButton>(this, ConfirmationMenuButtonName == "OK" ? ConfirmationMenuOkButtonClass : ConfirmationMenuCancelButtonClass);
        if (!ConfirmationMenuButton)
        {
            UE_LOGFMT(LogConfirmationMenu, Error, "UCustomButton 위젯을 생성하지 못했습니다. ButtonName: {0}", *ConfirmationMenuButtonName.ToString());
            continue;
        }

        //@내부 바인딩
        InternalBindToButton(ConfirmationMenuButton, ConfirmationMenuButtonName);

        //@Add CustomButton To Overlay
        UOverlaySlot* ButtonOverlaySlot = ConfirmationMenuButtonName == "OK"
            ? ConfirmationMenuOkButtonOverlay->AddChildToOverlay(ConfirmationMenuButton)
            : ConfirmationMenuCancelButtonOverlay->AddChildToOverlay(ConfirmationMenuButton);

        if (!ButtonOverlaySlot)
        {
            UE_LOGFMT(LogConfirmationMenu, Error, "CustomButton을 Overlay에 추가하지 못했습니다. ConfirmationMenuButton: {0}", *ConfirmationMenuButtonName.ToString());
            continue;
        }

        //@Alignment
        if (ButtonOverlaySlot)
        {
            ButtonOverlaySlot->SetHorizontalAlignment(HAlign_Fill);
            ButtonOverlaySlot->SetVerticalAlignment(VAlign_Fill);
            ButtonOverlaySlot->SetPadding(FMargin(0.0f));
        }

        //@버튼 저장
        MConfirmationMenuButtons.Add(ConfirmationMenuButtonName, ConfirmationMenuButton);

        UE_LOGFMT(LogConfirmationMenu, Log, "버튼 생성 및 저장 완료. ConfirmationMenuButton: {0}", *ConfirmationMenuButtonName.ToString());
    }

    //@이미지 다시 추가
    if (OkButtonImage)
    {
        UOverlaySlot* OkImageSlot = ConfirmationMenuOkButtonOverlay->AddChildToOverlay(OkButtonImage);
        if (OkImageSlot)
        {
            OkImageSlot->SetHorizontalAlignment(HAlign_Fill);
            OkImageSlot->SetVerticalAlignment(VAlign_Fill);
        }
    }
    if (CancelButtonImage)
    {
        UOverlaySlot* CancelImageSlot = ConfirmationMenuCancelButtonOverlay->AddChildToOverlay(CancelButtonImage);
        if (CancelImageSlot)
        {
            CancelImageSlot->SetHorizontalAlignment(HAlign_Fill);
            CancelImageSlot->SetVerticalAlignment(VAlign_Fill);
        }
    }

    UE_LOGFMT(LogConfirmationMenu, Log, "버튼 이미지가 다시 추가되었습니다.");
}

void UConfirmationMenu::OpenConfirmationMenu_Implementation()
{

    UE_LOGFMT(LogConfirmationMenu, Verbose, "Confirmation Menu 열기 완료");

    //@첫 번째 버튼(OK 버튼)의 Hover 상태 전환
    UCustomButton* OkButton = MConfirmationMenuButtons.FindRef(FName("OK"));
    if (!OkButton)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "OK 버튼을 찾을 수 없습니다.");
        return;
    }

    //@Keyboard 조작에 의한 Hover 상태로 전환
    if (!OkButton->SetButtonHoveredByKeyboard())
    {
        UE_LOGFMT(LogConfirmationMenu, Warning, "OK 버튼을 Hover 상태로 설정하는 데 실패했습니다.");
    }

    //@화면에 Confirmation Menu를 표시
    AddToViewport(200);

    //@Set Foucs
    SetFocus();

    UE_LOGFMT(LogConfirmationMenu, Log, "OK 버튼이 Hover 상태로 설정되었습니다.");

    //@Animation 작업 등 블루프린트에서 오버라이딩...

    UE_LOGFMT(LogConfirmationMenu, Verbose, "Confirmation Menu 열기 및 초기 설정 완료. 추가 애니메이션은 블루프린트에서 처리될 수 있습니다.");
}

void UConfirmationMenu::CloseConfirmationMenu_Implementation()
{
    //@화면에서 Confirmation Menu를 제거
    RemoveFromParent();

    //@Reset
    ResetConfirmationMenu();

    //@Animation 작업 등 블루프린트에서 오버라이딩...

    UE_LOGFMT(LogConfirmationMenu, Verbose, "Confirmation Menu 닫기 완료. 추가 애니메이션은 블루프린트에서 처리될 수 있습니다.");

}

void UConfirmationMenu::HandleHorizontalDirectionalInput(int32 Direction)
{
    TArray<FName> ButtonNames = { FName("OK"), FName("CANCEL") };
    int32 CurrentIndex = ButtonNames.IndexOfByPredicate([this](const FName& Name) {
        return MConfirmationMenuButtons[Name] == CurrentHoveredConfirmationButton.Get();
        });

    int32 NewIndex = (CurrentIndex + Direction + ButtonNames.Num()) % ButtonNames.Num();
    FName NewButtonName = ButtonNames[NewIndex];

    //@이전 Hover 상태 취소
    if (CurrentHoveredConfirmationButton.IsValid())
    {
        CurrentHoveredConfirmationButton->CancelSelectedButton();
    }

    //@새로운 버튼 Hover 상태로 설정
    UCustomButton* NewButton = MConfirmationMenuButtons[NewButtonName];
    if (!NewButton)
    {

        return;
    }

    if (!NewButton->SetButtonHoveredByKeyboard())
    {

        return;
    }

    //@Current Hovered Confirmation Button 업데이트
    CurrentHoveredConfirmationButton = NewButton;
}

void UConfirmationMenu::SelectCurrentHoveredOptionByKeyboard()
{
    if (!CurrentHoveredConfirmationButton.IsValid())
    {
        return;
    }

    if (!CurrentHoveredConfirmationButton->SetButtonSelectedByKeyboard())
    {
        return;
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UConfirmationMenu::OnConfirmationMenuButtonClicked_Implementation(EInteractionMethod InteractionMethodType, FName MenuButtonName)
{

    //@MConfirmationMenuButtons
    if (!MConfirmationMenuButtons.Contains(MenuButtonName))
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "클릭된 버튼 {0}이(가) MConfirmationMenuButtons에 존재하지 않습니다.", *MenuButtonName.ToString());
        return;
    }

    //@Current Selected Confirmation Button
    CurrentSelectedConfirmationButton = MConfirmationMenuButtons[MenuButtonName];

    //@버튼 클릭 이벤트
    ConfirmationMenuButtonClicked.ExecuteIfBound(MenuButtonName);

    UE_LOGFMT(LogConfirmationMenu, Log, "{0} 버튼이 클릭되었습니다.", *MenuButtonName.ToString());
    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UConfirmationMenu::OnConfirmationMenuButtonHovered_Implementation(EInteractionMethod InteractionMethodType, FName MenuButtonName)
{
    //@호버된 Confirmation Menu Button 가져오기
    UCustomButton* HoveredButton = MConfirmationMenuButtons.FindRef(MenuButtonName);
    if (!HoveredButton)
    {
        UE_LOGFMT(LogConfirmationMenu, Warning, "호버된 버튼을 찾을 수 없습니다: {0}", MenuButtonName.ToString());
        return;
    }

    //@현재 호버된 버튼과 새로 호버된 버튼이 같은 경우 처리 중단
    if (CurrentHoveredConfirmationButton.IsValid() && CurrentHoveredConfirmationButton.Get() == HoveredButton)
    {
        UE_LOGFMT(LogConfirmationMenu, Verbose, "이미 호버된 버튼입니다. 처리를 무시합니다: {0}", MenuButtonName.ToString());
        return;
    }

    //@이전 호버 상태 취소
    if (CurrentHoveredConfirmationButton.IsValid())
    {
        FName PreviousButtonName;
        for (const auto& Pair : MConfirmationMenuButtons)
        {
            if (Pair.Value == CurrentHoveredConfirmationButton)
            {
                PreviousButtonName = Pair.Key;
                break;
            }
        }
        CancelConfirmationMenuButtonSelected(PreviousButtonName);
        UE_LOGFMT(LogConfirmationMenu, Log, "이전에 호버된 버튼 취소: {0}", PreviousButtonName.ToString());
    }

    //@새로운 호버 버튼 설정
    CurrentHoveredConfirmationButton = HoveredButton;

    UE_LOGFMT(LogConfirmationMenu, Log, "새로운 버튼이 호버됨: {0}", MenuButtonName.ToString());

    //@TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 호버된 버튼의 시각적 상태 변경, 버튼 정보 표시 등

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UConfirmationMenu::OnConfirmationMenuButtonUnhovered_Implementation(FName MenuButtonName)
{
    UE_LOGFMT(LogConfirmationMenu, Log, "확정 메뉴 버튼 언호버됨: 버튼 {0}", MenuButtonName.ToString());

    //@Current Hovered Button
    if (CurrentHoveredConfirmationButton.IsValid())
    {
        FName CurrentHoveredButtonName;
        for (const auto& Pair : MConfirmationMenuButtons)
        {
            if (Pair.Value == CurrentHoveredConfirmationButton)
            {
                CurrentHoveredButtonName = Pair.Key;
                break;
            }
        }

        if (CurrentHoveredButtonName != MenuButtonName)
        {
            UE_LOGFMT(LogConfirmationMenu, Verbose, "언호버된 버튼이 현재 호버된 버튼과 일치하지 않음: 버튼 {0}", MenuButtonName.ToString());
            return;
        }

        CurrentHoveredConfirmationButton.Reset();

        UE_LOGFMT(LogConfirmationMenu, Verbose, "현재 호버된 버튼 리셋됨: 버튼 {0}", MenuButtonName.ToString());
    }

    //@TODO: Unhover 상태 처리
    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UConfirmationMenu::CancelConfirmationMenuButtonSelected_Implementation(FName PreviousMenuButtonName)
{
    auto PreviousButton = MConfirmationMenuButtons.FindRef(PreviousMenuButtonName);
    if (!PreviousButton)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "Confirmation Menu Button이 유효하지 않습니다! MenuButtonName: {0}", *PreviousMenuButtonName.ToString());
        return;
    }

    PreviousButton->CancelSelectedButton();

    UE_LOGFMT(LogConfirmationMenu, Log, "{0} 버튼이 취소되었습니다.", *PreviousMenuButtonName.ToString());

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void UConfirmationMenu::SetConfirmationMenuDialogueText(FText Text)
{
    if (!ConfirmationMenuDialogue)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "ConfirmationMenuDialogue가 유효하지 않습니다!");
        return;
    }

    FText ArrangedText = ArrangeDialogueText(Text);
    ConfirmationMenuDialogue->SetText(ArrangedText);

    UE_LOGFMT(LogConfirmationMenu, Verbose, "ConfirmationMenu 대화 텍스트가 설정되었습니다.");
}

FText UConfirmationMenu::GetConfirmationMenuDialogueText() const
{
    if (!ConfirmationMenuDialogue)
    {
        UE_LOGFMT(LogConfirmationMenu, Error, "ConfirmationMenuDialogue가 유효하지 않습니다!");
        return FText::GetEmpty();
    }

    return ConfirmationMenuDialogue->GetText();
}

FText UConfirmationMenu::ArrangeDialogueText(FText Text)
{
    FString String = Text.ToString();

    UE_LOGFMT(LogConfirmationMenu, Log, "새로운 Dialogue Text : {0}", String);

    TArray<FString> Sentences;
    String.ParseIntoArray(Sentences, TEXT("."), true);

    FString FormattedDescription;
    for (int32 i = 0; i < Sentences.Num(); ++i)
    {
        FormattedDescription += Sentences[i].TrimStart().TrimEnd();
        if (i < Sentences.Num() - 1)
        {
            FormattedDescription += TEXT(".\n");
        }
        else if (i == Sentences.Num() - 1 && !Sentences[i].IsEmpty())
        {
            FormattedDescription += TEXT(".");
        }
    }

    if (FormattedDescription.IsEmpty())
    {
        UE_LOGFMT(LogConfirmationMenu, Warning, "ArrangeDialogueText: 포맷된 설명이 비어있습니다.");
    }

    return FText::FromString(FormattedDescription);
}
#pragma endregion
