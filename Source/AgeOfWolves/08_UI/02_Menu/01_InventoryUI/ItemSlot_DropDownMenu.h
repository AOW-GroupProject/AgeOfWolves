// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "08_UI/DropDownMenu.h"
#include "Engine/DataTable.h"

#include "ItemSlot_DropDownMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlot_DropDownMenu, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UHorizontalBox;
class UItemSlots;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
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
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
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

//@Property/Info...etc
#pragma region Subwidgets
#pragma endregion

    //@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
