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
{}

void UConfirmationMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UConfirmationMenu::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UConfirmationMenu::NativeConstruct()
{
    Super::NativeConstruct();
}

void UConfirmationMenu::NativeDestruct()
{
    Super::NativeDestruct();
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
    if (!CurrentSelectedConfirmationButton.Get())
    {
        return;
    }

    //@Cancel Confirmation Menu Button Selected
    if (CurrentSelectedConfirmationButton.IsValid())
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
    //@화면에 Confirmation Menu를 표시
    AddToViewport(200);

    //@Animation 작업 등 블루프린트에서 오버라이딩...

    UE_LOGFMT(LogConfirmationMenu, Verbose, "Confirmation Menu 열기 완료. 추가 애니메이션은 블루프린트에서 처리될 수 있습니다.");

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
    UE_LOGFMT(LogConfirmationMenu, Log, "{0} 버튼에 마우스가 올라갔습니다.", *MenuButtonName.ToString());
    // TODO: Hover 상태 처리
    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UConfirmationMenu::OnConfirmationMenuButtonUnhovered_Implementation(FName MenuButtonName)
{
    UE_LOGFMT(LogConfirmationMenu, Log, "{0} 버튼에서 마우스가 벗어났습니다.", *MenuButtonName.ToString());
    // TODO: Unhover 상태 처리
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
        UE_LOGFMT(LogConfirmationMenu, Warning, "ConfirmationMenuDialogue가 유효하지 않습니다!");
        return; 
    }
    
    //@Set Text
    ConfirmationMenuDialogue->SetText(ArrangeDialogueText(Text));

}

FText UConfirmationMenu::GetConfirmationMenuDialogueText() const
{
    if (ConfirmationMenuDialogue)
    {
        return ConfirmationMenuDialogue->GetText();
    }
    return FText::GetEmpty();
}

FText UConfirmationMenu::ArrangeDialogueText(FText Text)
{
    //@FText를 FString으로 변환
    FString String = Text.ToString();

    //@'.'을 기준으로 문자열 분리
    TArray<FString> Sentences;
    String.ParseIntoArray(Sentences, TEXT("."), true);

    //@각 문장에 개행 추가 및 재결합
    FString FormattedDescription;
    for (int32 i = 0; i < Sentences.Num(); ++i)
    {
        FormattedDescription += Sentences[i].TrimStart().TrimEnd();
        if (i < Sentences.Num() - 1)  // 마지막 문장이 아니면 '.'과 개행 추가
        {
            FormattedDescription += TEXT(".\n");
        }
        else if (i == Sentences.Num() - 1 && !Sentences[i].IsEmpty())  // 마지막 문장이면서 비어있지 않으면 '.'만 추가
        {
            FormattedDescription += TEXT(".");
        }
    }

    //@FString을 FText로 변환하여 반환
    return FText::FromString(FormattedDescription);
}
#pragma endregion

