#include "DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "08_UI/DropDownMenuOption.h"

DEFINE_LOG_CATEGORY(LogDropDownMenu)

#pragma region Default Setting
UDropDownMenu::UDropDownMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    DropDownMenuOptionBox = nullptr;
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

void UDropDownMenu::InternalBindToOptions(UDropDownMenuOption* Option)
{
    if (!Option)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "Option이 null입니다. 바인딩을 수행할 수 없습니다.");
        return;
    }

    //@내부 바인딩
    Option->DropDownMenuOptionInitFinished.BindUFunction(this, "OnDropDownMenuOptionInitFinished");
    Option->DropDownMenuOptionSelected.BindUObject(this, &UDropDownMenu::OnDropDownMenuOptionSelected);

    UE_LOGFMT(LogDropDownMenu, Verbose, "Option({0})에 대한 내부 바인딩 완료", *Option->GetName());
}

void UDropDownMenu::InitializeDropDownMenu()
{
    //@Create Options
    CreateDropDownMenuOptions();

    //@초기화 요청 이벤트
    RequestStartInitByDropDownMenu.Broadcast();
}
#pragma endregion

#pragma region Subwidgets
void UDropDownMenu::ResetDropDownMenu()
{
    //@TODO: Drop Donw Menu의 초기 설정에 필요한 동작들...
    
    
    //@가시성 비활성화
    SetVisibility(ESlateVisibility::Collapsed);
}

void UDropDownMenu::CreateDropDownMenuOptions()
{
    if (!DropDownMenuOptionBox)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "DropDownMenuOptionBox가 null입니다. 옵션을 생성할 수 없습니다.");
        return;
    }

    if (!DropDownMenuOptionClass)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "DropDownMenuOptionClass가 설정되지 않았습니다. 에디터에서 DropDownMenuOptionClass를 설정해주세요.");
        return;
    }

    //@기존 옵션 제거
    DropDownMenuOptionBox->ClearChildren();

    for (const FText& OptionText : OptionNames)
    {
        UDropDownMenuOption* Option = CreateWidget<UDropDownMenuOption>(this, DropDownMenuOptionClass);
        if (!Option)
        {
            UE_LOGFMT(LogDropDownMenu, Error, "UDropDownMenuOption 위젯을 생성하지 못했습니다. DropDownMenuOptionClass: {0}", *DropDownMenuOptionClass->GetName());
            continue;
        }

        //@Set Option Name
        Option->SetOptionName(OptionText);

        //@비동기 초기화 바인딩
        RequestStartInitByDropDownMenu.AddUFunction(Option, "InitializeDropDownMenuOption");

        //@내부 바인딩
        InternalBindToOptions(Option);

        //@Add Child To VerticalBox
        UVerticalBoxSlot* VerticalBoxSlot = DropDownMenuOptionBox->AddChildToVerticalBox(Option);
        if (!VerticalBoxSlot)
        {
            continue;
        }
        VerticalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        VerticalBoxSlot->SetVerticalAlignment(VAlign_Fill);

        UE_LOGFMT(LogDropDownMenu, Log, "DropDownMenuOption({0})이 생성되고 이벤트가 바인딩되었습니다.", OptionText.ToString());
    }

}
#pragma endregion

#pragma region Callbacks
void UDropDownMenu::OnDropDownMenuOptionInitFinished()
{
    //@초기화 완료 이벤트
    DropDownMenuInitFinished.ExecuteIfBound();
}

void UDropDownMenu::OnDropDownMenuOptionSelected(const FText& SelectedOptionText)
{
    UE_LOGFMT(LogDropDownMenu, Log, "드롭다운 메뉴 옵션 선택: {0}", SelectedOptionText.ToString());

    //@여기에 선택된 옵션에 대한 처리 로직을 추가할 수 있습니다...


}
#pragma endregion