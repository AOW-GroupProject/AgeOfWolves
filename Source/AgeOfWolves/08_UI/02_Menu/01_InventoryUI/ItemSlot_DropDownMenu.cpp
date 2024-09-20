// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot_DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "08_UI/DropDownMenuOption.h"

#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogItemSlot_DropDownMenu)

UItemSlot_DropDownMenu::UItemSlot_DropDownMenu(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{

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

void UItemSlot_DropDownMenu::CreateDropDownMenuOptions()
{
    Super::CreateDropDownMenuOptions();



}

void UItemSlot_DropDownMenu::OnDropDownMenuOptionSelected(const FText& SelectedOptionText)
{
    Super::OnDropDownMenuOptionSelected(SelectedOptionText);


}
