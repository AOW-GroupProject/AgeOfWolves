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
//@Option 버튼의 선택/호버 취소 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FCancelOptionButton, const FName&)
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
    friend class UItemSlots;
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
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    // 내부 바인딩 함수
    virtual void InternalBindToOptions(UDropDownMenuOption* Option, const FName& OptionName, bool bIsLastOption) override;

public:
    //@초기화
    virtual void InitializeDropDownMenu() override;
#pragma endregion

//@Property/Info...etc
#pragma region Subwidgets
protected:
    virtual void ResetDropDownMenu() override;

protected:
    virtual void CreateDropDownMenuOptions() override;


protected:
    FName CurrentHoveredOptionName;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@Option 버튼의 선택/호버 취소 이벤트
    FCancelOptionButton CancelOptionButton;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@가시성 변화 이벤트 구독
    virtual void OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType) override;

protected:
    //@옵션 버튼의 Hover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnDropDownMenuOptionButtonHovered(FName OptionName, EInteractionMethod InteractionMethodType);
    virtual void OnDropDownMenuOptionButtonHovered_Implementation(FName OptionName, EInteractionMethod InteractionMethodType);

    //@옵션 버튼의 Unhover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnDropDownMenuOptionButtonUnhovered(FName OptionName);
    virtual void OnDropDownMenuOptionButtonUnhovered_Implementation(FName OptionName);

    //@옵션 버튼의 Click 이벤트 구독
    virtual void OnDropDownMenuOptionSelected_Implementation(FName SelectedOptionName, EInteractionMethod InteractionMethodType) override;
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};