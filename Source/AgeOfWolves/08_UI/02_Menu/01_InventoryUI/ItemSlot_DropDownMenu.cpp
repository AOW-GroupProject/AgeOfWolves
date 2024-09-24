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
#pragma endregion

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
    //@Ok?
    if (OkOrCancel == "OK")
    {

        return;
    }
    //@Cancel?
    if (OkOrCancel == "CANCEL")
    {

        return;
    }

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
