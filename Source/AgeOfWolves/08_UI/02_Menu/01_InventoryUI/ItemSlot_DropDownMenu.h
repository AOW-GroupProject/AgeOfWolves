// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "08_UI/DropDownMenu.h"
#include "Engine/DataTable.h"

#include "ItemSlot_DropDownMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlot_DropDownMenu, Log, All)

#pragma region Forward Declaration
class UHorizontalBox;
class UBaseGameplayAbility;
#pragma endregion

#pragma region Delegates

#pragma endregion

/**
 * @UItemSlot_DropDownMenu
 * 
 * Item Slot 클릭 시 나타낼 Drop Down Menu UI
 */
UCLASS()
class AGEOFWOLVES_API UItemSlot_DropDownMenu : public UDropDownMenu
{
	GENERATED_BODY()

#pragma region Default Setting
public:
    UItemSlot_DropDownMenu(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    // 내부 바인딩 함수

public:
    //@초기화
    virtual void InitializeDropDownMenu() override;
#pragma endregion
	
#pragma region Subwidgets
protected:
    //@Drop Down Menu Option 생성 함수
    virtual void CreateDropDownMenuOptions() override;
#pragma endregion

#pragma region Callbacks
protected:
    //@Option 선택 이벤트에 등록되는 콜백
    virtual void OnDropDownMenuOptionSelected(const FText& SelectedOptionText) override;
#pragma endregion

};
