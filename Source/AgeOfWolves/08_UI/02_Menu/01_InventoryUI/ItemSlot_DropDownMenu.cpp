
// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot_DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "08_UI/DropDownMenuOption.h"
#include "08_UI/ConfirmationMenu.h"
#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogItemSlot_DropDownMenu)

//@Default Settings
#pragma region Default Setting
UItemSlot_DropDownMenu::UItemSlot_DropDownMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    CurrentHoveredOptionName = FName();
}

void UItemSlot_DropDownMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    OnVisibilityChanged.AddDynamic(this, &UItemSlot_DropDownMenu::OnUIVisibilityChanged);
}

void UItemSlot_DropDownMenu::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UItemSlot_DropDownMenu::NativeConstruct()
{
    Super::NativeConstruct();
}

void UItemSlot_DropDownMenu::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UItemSlot_DropDownMenu::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
}

FReply UItemSlot_DropDownMenu::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UItemSlot_DropDownMenu::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusLost(InFocusEvent);
}

FReply UItemSlot_DropDownMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "키 입력 감지됨: {0}", Key.ToString());

    //@방향키 조작
    if (Key == EKeys::Down || Key == EKeys::Up)
    {
        //@위, 아래 방향키 조작 처리
        HandleVerticalDirectionalInput(Key == EKeys::Up ? -1 : 1);

        return FReply::Handled();
    }
    //@옵션 선택
    else if (Key == EKeys::Enter)
    {
        SelectOptionByHotKey(CurrentHoveredOptionName);

        return FReply::Handled();
    }
    //@ESC키(옵션 창 닫기)
    else if (Key == EKeys::Escape)
    {
        CloseDropDownMenu();

        return FReply::Handled();
    }
    //@옵션 선택 단축키
    else
    {
        for (const auto& OptionInfo : OptionInformations)
        {
            EHotKey OptionHotKey = OptionInfo.GetHotKey();
            if (CompareKeyWithHotKey(Key, OptionHotKey))
            {
                SelectOptionByHotKey(OptionInfo.GetOptionName());
                return FReply::Handled();
            }
        }
    }

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "Drop Down Menu에서 처리하지 않는 키 입력: {0}", Key.ToString());

    //@이외 키 입력은 처리되지 않았고, 다른 UI에서 키 입력 처리를 막습니다.
    return FReply::Handled();
}

void UItemSlot_DropDownMenu::InternalBindToOptions(UDropDownMenuOption* Option, const FName& OptionName, bool bIsLastOption)
{

    //@내부 바인딩
    if (!Option)
    {
        return;
    }

    Super::InternalBindToOptions(Option, OptionName, bIsLastOption);

    Option->DropDownMenuOptionHovered.AddUFunction(this, "OnDropDownMenuOptionButtonHovered");
    Option->DropDownMenuOptionUnhovered.AddUFunction(this, "OnDropDownMenuOptionButtonUnhovered");
    Option->DropDownMenuOptionSelected.AddUFunction(this, "OnDropDownMenuOptionSelected");

}

void UItemSlot_DropDownMenu::InitializeDropDownMenu()
{
    Super::InitializeDropDownMenu();

}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UItemSlot_DropDownMenu::ResetDropDownMenu()
{
    Super::ResetDropDownMenu();

    //@Prev Hovered Option의 선택/호버 상태 취소
    if (!CurrentHoveredOptionName.IsNone())
    {
        FName PrevHoveredOption = CurrentHoveredOptionName;
        CancelOptionButton.Broadcast(PrevHoveredOption);
        CurrentHoveredOptionName = FName();
        UE_LOGFMT(LogDropDownMenu, Log, "이전에 선택된 옵션 취소: {0}", PrevHoveredOption.ToString());
    }

    //@첫 번째 Drop Down Menu Option
    UDropDownMenuOption* FirstDropDownMenuOption = GetFirstDropDownMenuOption();
    if (!FirstDropDownMenuOption)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "첫 번째 드롭다운 메뉴 옵션을 찾을 수 없습니다.");
        return;
    }

    //@Button
    UCustomButton* Button = FirstDropDownMenuOption->GetDropDownMenuOptionButton();
    if (!Button)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "첫 번째 옵션의 버튼을 찾을 수 없습니다.");
        return;
    }

    //@Set Button Hovered By Keyboard
    if (!Button->SetButtonHoveredByKeyboard())
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "{0} 옵션을 Hover 상태로 설정하는 데 실패했습니다.", FirstDropDownMenuOption->GetOptionName());
        return;
    }

    //@Current Hovered Option Name
    CurrentHoveredOptionName = FirstDropDownMenuOption->GetOptionName();
    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "{0} 옵션이 Hover 상태로 설정되었습니다.", CurrentHoveredOptionName.ToString());

}

void UItemSlot_DropDownMenu::CreateDropDownMenuOptions()
{
    Super::CreateDropDownMenuOptions();

    //@바인딩
    for (auto Option : DropDownMenuOptions)
    {
        if (!Option)
        {
            continue;
        }

        CancelOptionButton.AddUFunction(Option, "DropDownMenuOptionButtonCanceledNotified");
    }

}

void UItemSlot_DropDownMenu::HandleVerticalDirectionalInput(int32 Direction)
{
    //@Drop Down Menu Options
    if (DropDownMenuOptions.Num() == 0)
    {
        return;
    }

    //@Current Hovered Option Name
    if (CurrentHoveredOptionName.IsNone())
    {
        return;
    }

    //@Current Hovered Option의 Index
    int32 CurrentIndex = -1;

    CurrentIndex = DropDownMenuOptions.IndexOfByPredicate([this](const UDropDownMenuOption* Option) {
        return Option && Option->GetOptionName() == CurrentHoveredOptionName;
        });

    //@다음 Option의 Index;
    int32 NewIndex = CurrentIndex + Direction;
    if (NewIndex < 0)
    {
        NewIndex = DropDownMenuOptions.Num() - 1;
    }
    else if (NewIndex >= DropDownMenuOptions.Num())
    {
        NewIndex = 0;
    }

    //@Next Option
    UDropDownMenuOption* NewOption = DropDownMenuOptions[NewIndex];
    if (!NewOption)
    {
        
        return;
    }

    //@Set Button Hovered By Keyboard
    UCustomButton* Button = NewOption->GetDropDownMenuOptionButton();
    if (Button && Button->SetButtonHoveredByKeyboard())
    {
        CurrentHoveredOptionName = NewOption->GetOptionName();
        UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "새로운 옵션이 Hover 상태로 설정됨: {0}", *CurrentHoveredOptionName.ToString());
    }

}

void UItemSlot_DropDownMenu::SelectOptionByHotKey(const FName& OptionName)
{
    //@단축키에 대응되는 Option
    UDropDownMenuOption* OptionToSelect = GetDropDownMenuOptionByName(OptionName);
    if (!OptionToSelect)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "선택할 옵션을 찾을 수 없습니다: {0}", OptionName.ToString());
        return;
    }

    //@Button
    UCustomButton* SelectedButton = OptionToSelect->GetDropDownMenuOptionButton();
    if (!SelectedButton)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "선택된 옵션의 버튼을 찾을 수 없습니다: {0}", OptionName.ToString());
        return;
    }

    //@현재 Hover 된 Option과 선택하고자 하는 Option이 다르다면 이전 Hover 상태 취소
    if (!CurrentHoveredOptionName.IsNone() && CurrentHoveredOptionName != OptionName)
    {
        CancelDropDownMenuOptionButton.Broadcast(CurrentHoveredOptionName);
    }

    //@Set Button Selected By Keyboard
    if (!SelectedButton->SetButtonSelectedByKeyboard())
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "옵션 선택 실패: {0}", OptionName.ToString());
        return;
    }

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "옵션이 성공적으로 선택되었습니다: {0}", OptionName.ToString());
}

void UItemSlot_DropDownMenu::ResetSelectedOptionToHovered()
{
    //@Current Selected Option Name
    if (CurrentSelectedOptionName.IsNone())
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "현재 선택된 옵션이 없습니다.");
        return;
    }

    UDropDownMenuOption* SelectedOption = GetDropDownMenuOptionByName(CurrentSelectedOptionName);
    if (!SelectedOption)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "선택된 옵션을 찾을 수 없습니다: {0}", CurrentSelectedOptionName.ToString());
        return;
    }

    UCustomButton* Button = SelectedOption->GetDropDownMenuOptionButton();
    if (!Button)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "선택된 옵션의 버튼을 찾을 수 없습니다: {0}", CurrentSelectedOptionName.ToString());
        return;
    }

    // 현재 선택된 옵션의 상태를 취소
    CancelOptionButton.Broadcast(CurrentSelectedOptionName);

    // 선택된 옵션을 Hover 상태로 다시 설정
    if (!Button->SetButtonHoveredByKeyboard())
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "현재 선택된 옵션을 Hover 상태로 설정하는데 실패했습니다: {0}", CurrentSelectedOptionName.ToString());
        return;
    }

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "현재 선택된 옵션이 Hover 상태로 재설정되었습니다: {0}", CurrentSelectedOptionName.ToString());
}

#pragma endregion

//@Callbacks
#pragma region Callbacks
void UItemSlot_DropDownMenu::OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType)
{
    Super::OnUIVisibilityChanged_Implementation(VisibilityType);

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "UI 가시성 변경: {0}", *UEnum::GetValueAsString(VisibilityType));

    if (VisibilityType == ESlateVisibility::SelfHitTestInvisible)
    {
        //@포커스
        SetFocus();
    }

}

void UItemSlot_DropDownMenu::OnDropDownMenuOptionButtonHovered_Implementation(FName OptionName, EInteractionMethod InteractionMethodType)
{
    //@호버된 Drop Down Menu Option 가져오기
    UDropDownMenuOption* HoveredOption = GetDropDownMenuOptionByName(OptionName);
    if (!HoveredOption)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "호버된 옵션을 찾을 수 없습니다: {0}", OptionName.ToString());
        return;
    }

    //@현재 호버된 옵션과 새로 호버된 옵션이 같은 경우 처리 중단
    if (!CurrentHoveredOptionName.IsNone() && CurrentHoveredOptionName == OptionName)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Verbose, "이미 호버된 옵션입니다. 처리를 무시합니다: {0}", OptionName.ToString());
        return;
    }

    //@이전 호버 상태 취소
    if (!CurrentHoveredOptionName.IsNone())
    {
        CancelOptionButton.Broadcast(CurrentHoveredOptionName);
        UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "이전에 호버된 옵션 취소: {0}", CurrentHoveredOptionName.ToString());
    }

    if (CurrentSelectedOptionName == OptionName)
    {
        CurrentSelectedOptionName = NAME_None;
    }

    //@새로운 호버 옵션 설정
    CurrentHoveredOptionName = OptionName;

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "새로운 옵션이 호버됨: {0}", OptionName.ToString());

    //@TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 호버된 옵션의 시각적 상태 변경, 옵션 정보 표시 등

}

void UItemSlot_DropDownMenu::OnDropDownMenuOptionButtonUnhovered_Implementation(FName OptionName)
{
    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "드롭다운 메뉴 옵션 버튼 언호버됨: 옵션 {0}", OptionName.ToString());

    //@Current Hovered Option
    if (!CurrentHoveredOptionName.IsNone() && CurrentHoveredOptionName != OptionName)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Verbose, "언호버된 옵션이 현재 호버된 옵션과 일치하지 않음: 옵션 {0}", OptionName.ToString());
        return;
    }

    if (!CurrentHoveredOptionName.IsNone() && CurrentHoveredOptionName == OptionName)
    {
        CurrentHoveredOptionName = NAME_None;

        UE_LOGFMT(LogItemSlot_DropDownMenu, Verbose, "현재 호버된 옵션 리셋됨: 옵션 {0}", OptionName.ToString());
    }

}

void UItemSlot_DropDownMenu::OnDropDownMenuOptionSelected_Implementation(FName SelectedOptionName, EInteractionMethod InteractionMethodType)
{
    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "드롭다운 메뉴 옵션 선택됨: {0}", SelectedOptionName.ToString());

    Super::OnDropDownMenuOptionSelected_Implementation(SelectedOptionName, InteractionMethodType);

}

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UItemSlot_DropDownMenu::CompareKeyWithHotKey(const FKey& Key, EHotKey HotKey)
{
    switch (HotKey)
    {
    case EHotKey::Q: return Key == EKeys::Q;
    case EHotKey::W: return Key == EKeys::W;
    case EHotKey::E: return Key == EKeys::E;
    case EHotKey::R: return Key == EKeys::R;
    case EHotKey::T: return Key == EKeys::T;
    default: return false;
    }
}
#pragma endregion
