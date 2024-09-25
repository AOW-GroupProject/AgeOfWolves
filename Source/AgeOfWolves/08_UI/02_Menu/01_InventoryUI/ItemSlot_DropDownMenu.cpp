// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot_DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "08_UI/DropDownMenuOption.h"
#include "08_UI/ConfirmationMenu.h"

DEFINE_LOG_CATEGORY(LogItemSlot_DropDownMenu)

//@Default Settings
#pragma region Default Setting
UItemSlot_DropDownMenu::UItemSlot_DropDownMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UItemSlot_DropDownMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
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

void UItemSlot_DropDownMenu::InitializeDropDownMenu()
{
    Super::InitializeDropDownMenu();

}
void UItemSlot_DropDownMenu::ActivateUseOption()
{
}
void UItemSlot_DropDownMenu::ActivateLeaveOption()
{
}
void UItemSlot_DropDownMenu::ActivateDiscardOption()
{
}
void UItemSlot_DropDownMenu::ActivateBackOption()
{
}
void UItemSlot_DropDownMenu::ActivateHelpOption()
{
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UItemSlot_DropDownMenu::OnDropDownMenuOptionSelected(FName SelectedOptionName)
{
    //@Super
    Super::OnDropDownMenuOptionSelected(SelectedOptionName);

    //@선택된 옵션에 대응하는 Confirmation Menu 찾기
    UConfirmationMenu* SelectedConfirmationMenu = OptionConfirmationMenus.FindRef(SelectedOptionName);
    if (!SelectedConfirmationMenu)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "선택된 옵션 '{0}'에 대한 Confirmation Menu를 찾을 수 없습니다.", *SelectedOptionName.ToString());
        return;
    }

    UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "선택된 옵션 '{0}'에 대한 Confirmation Menu를 찾았습니다.", *SelectedOptionName.ToString());

    //@TODO: Selected Optio Name에 해당되는 Confirmation Menu를 화면에 표시합니다.
    SelectedConfirmationMenu->OpenConfirmationMenu();
}

void UItemSlot_DropDownMenu::OnConfirmationMenuOptionSelected(FName OkOrCancel)
{
    //@Confirmation Menu
    UConfirmationMenu* SelectedConfirmationMenu = OptionConfirmationMenus.FindRef(CurrentSelectedOption);
    if (!SelectedConfirmationMenu)
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Error, "현재 선택된 옵션 '{0}'에 대한 Confirmation Menu를 찾을 수 없습니다.", *CurrentSelectedOption.ToString());
        return;
    }

    //@Ok?
    if (OkOrCancel == "OK")
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "'{0}' 옵션에 대해 'OK'가 선택되었습니다.", *CurrentSelectedOption.ToString());
        //@TODO: 'OK' 선택 시 수행할 작업 구현
        return;
    }
    //@Cancel?
    if (OkOrCancel == "CANCEL")
    {
        UE_LOGFMT(LogItemSlot_DropDownMenu, Log, "'{0}' 옵션에 대해 'CANCEL'이 선택되었습니다.", *CurrentSelectedOption.ToString());

        //@Close Confirmation Menu
        SelectedConfirmationMenu->CloseConfirmationMenu();

        UE_LOGFMT(LogItemSlot_DropDownMenu, Verbose, "'{0}' 옵션의 Confirmation Menu가 닫혔습니다.", *CurrentSelectedOption.ToString());
        return;
    }

    UE_LOGFMT(LogItemSlot_DropDownMenu, Warning, "알 수 없는 선택: {0}", *OkOrCancel.ToString());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
