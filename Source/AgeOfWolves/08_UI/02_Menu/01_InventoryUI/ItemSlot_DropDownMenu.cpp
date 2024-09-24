// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot_DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "08_UI/DropDownMenuOption.h"

#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogItemSlot_DropDownMenu)

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

void UItemSlot_DropDownMenu::CreateDropDownMenuOptions()
{
    Super::CreateDropDownMenuOptions();

}

void UItemSlot_DropDownMenu::OnDropDownMenuOptionSelected(FName SelectedOptionName)
{
    //@공통 작업 수행
    Super::OnDropDownMenuOptionSelected(SelectedOptionName);

    // 선택된 옵션 정보 찾기
    FDropDownMenuOptionInformation* SelectedOption = OptionInformations.FindByPredicate([&](const FDropDownMenuOptionInformation& Option) {
        return Option.GetOptionName() == SelectedOptionName;
        });

    if (!SelectedOption)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "선택된 옵션을 찾을 수 없습니다: {0}", SelectedOptionName.ToString());
        return;
    }

    // 현재 선택된 옵션 업데이트
    CurrentSelectedOption = SelectedOptionName;
    UE_LOGFMT(LogDropDownMenu, Log, "새로운 옵션이 선택됨: {0}", SelectedOptionName.ToString());

    // 선택된 옵션에 대한 추가 처리
    if (SelectedOption->IsHavingGA())
    {
        UBaseGameplayAbility* Ability = SelectedOption->GetOptionAbility();
        if (Ability)
        {
            //@TODO: GA를 갖는 Option일 경우, 아래에서 GA관련 처리 작업 수행...

            UE_LOGFMT(LogDropDownMenu, Log, "옵션 '{0}'에 연결된 GA를 활성화합니다.", SelectedOptionName.ToString());
        }
        else
        {
            UE_LOGFMT(LogDropDownMenu, Warning, "옵션 '{0}'에 연결된 GA가 null입니다.", SelectedOptionName.ToString());
            return;
        }
    }
    else
    {
        // TODO: GA 없는 옵션에 대한 추가 처리 로직
        UE_LOGFMT(LogDropDownMenu, Log, "옵션 '{0}'이(가) 선택되었습니다. GA 없음.", SelectedOptionName.ToString());
    }

}
