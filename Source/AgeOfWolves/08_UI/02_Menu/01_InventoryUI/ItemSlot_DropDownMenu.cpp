
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
    //CurrentHoveredOptionName = FName();
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

FReply UItemSlot_DropDownMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "키 입력 감지됨: {0}", Key.ToString());

    //@방향키 조작
    if (Key == EKeys::Down)
    {
        //@아래 옵션으로 이동

        return FReply::Handled();
    }
    else if (Key == EKeys::Up)
    {
        //@위 옵션으로 이동

        return FReply::Handled();
    }
    //@ESC키(옵션 창 닫기) 버튼은 열어줍니다.
    else if (Key == EKeys::Escape)
    {

        return FReply::Unhandled();
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

    //@이전에 선택된 옴션 버튼의 선택 취소 이벤트
    if (!CurrentHoveredOptionName.IsNone())
    {
        //@이전 선택된 옵션의 버튼 선택 취소 이벤트
        FName PrevHoveredOption = CurrentHoveredOptionName;
        CancelOptionButton.Broadcast(PrevHoveredOption);

        //@Current Selected Option
        CurrentHoveredOptionName = FName();

        UE_LOGFMT(LogDropDownMenu, Log, "이전에 선택된 옵션 취소: {0}", PrevHoveredOption.ToString());
    }
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
        UE_LOGFMT(LogItemSlot_DropDownMenu, Verbose, "드롭다운 메뉴에 포커스 설정됨");

        //@첫 번째 Option 버튼의 Hover 상태 전환
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

        //@Keyboard 조작에 의한 Hover 상태로 전환
        if (Button->SetButtonHoveredByKeyboard())
        {
            UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "{0} 옵션이 Hover 상태로 설정되었습니다.", FirstDropDownMenuOption->GetOptionName());
        }
        else
        {
            UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "{0} 옵션을 Hover 상태로 설정하는 데 실패했습니다.", FirstDropDownMenuOption->GetOptionName());
        }
    }
    else if (VisibilityType == ESlateVisibility::Collapsed)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "드롭다운 메뉴가 접혀졌습니다.");
        //@TODO: 드롭다운 메뉴가 접혔을 때 수행할 추가 작업이 있다면 여기에 구현
    }
    else
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Verbose, "처리되지 않은 가시성 상태: {0}", *UEnum::GetValueAsString(VisibilityType));
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
#pragma endregion
