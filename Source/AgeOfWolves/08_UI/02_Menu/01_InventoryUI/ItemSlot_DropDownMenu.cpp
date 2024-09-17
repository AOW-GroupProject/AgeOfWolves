// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot_DropDownMenu.h"

UItemSlot_DropDownMenu::UItemSlot_DropDownMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    //@Option Names
    OptionNames.Add(FText::FromString("USE"));
    OptionNames.Add(FText::FromString("LEAVE"));
    OptionNames.Add(FText::FromString("DISCARD"));
    OptionNames.Add(FText::FromString("BACK"));
    OptionNames.Add(FText::FromString("HELP"));
}

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
