#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DropDownMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDropDownMenu, Log, All)

#pragma region Forward Declaration
class UVerticalBox;
class UImage;
class USizeBox;
class UOverlay;
class UDropDownMenuOption;
#pragma endregion

#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByDropDownMenu)
//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FDropDownMenuInitFinished);
#pragma endregion

/**
 * @UDropDownMenu
 *
 * Drop Down Menu를 구현합니다.
 */
UCLASS()
class AGEOFWOLVES_API UDropDownMenu : public UUserWidget
{
    //@friend class
    friend class UInteractableItemSlot;

    GENERATED_BODY()

#pragma region Default Setting
public:
    UDropDownMenu(const FObjectInitializer& ObjectInitializer);

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
    void InternalBindToOptions(UDropDownMenuOption* Option);

public:
    //@초기화
    UFUNCTION()
        virtual void InitializeDropDownMenu();

#pragma endregion

#pragma region Subwidgets
protected:
    //@Reset
    void ResetDropDownMenu();
    //@Drop Down Menu Option 생성
    void CreateDropDownMenuOptions();

protected:
    //@Open Drop Down Menu
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        void OpenDropDownMenu();
    //@Close Drop Down Menu
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        void CloseDropDownMenu();

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* DropDownMenuBGImage;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UVerticalBox* DropDownMenuOptionBox;

protected:
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu | Options")
        TArray<FText> OptionNames;
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu | Options")
        TSubclassOf<UDropDownMenuOption> DropDownMenuOptionClass;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByDropDownMenu RequestStartInitByDropDownMenu;
    //@초기화 완료 이벤트
    FDropDownMenuInitFinished DropDownMenuInitFinished;
#pragma endregion

#pragma region Callbacks;
protected:
    UFUNCTION()
        void OnDropDownMenuOptionInitFinished();

protected:
    //@Option 선택 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnDropDownMenuOptionSelected(const FText& SelectedOptionText);

#pragma endregion

};