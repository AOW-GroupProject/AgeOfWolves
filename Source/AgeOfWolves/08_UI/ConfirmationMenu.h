#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ConfirmationMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogConfirmationMenu, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UHorizontalBox;
class UVerticalBox;
class UImage;
class UMultiLineEditableTextBox;
class UCustomButton;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FConfirmationMenuInitFinished);

//@Confirmation Menu 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FConfirmationMenuButtonClicked, FName);
//@확정 메뉴  버튼 호버 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FConfirmationMenuButtonHovered, FName)
//@확정 메뉴  버튼 언호버 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FConfirmationMenuButtonUnhovered, FName)
#pragma endregion

/**
 * @UConfirmationMenu
 * 
 * 사용자 지시의 최종 확정을 위한 UI
 */
UCLASS()
class AGEOFWOLVES_API UConfirmationMenu : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
    friend class UItemSlot_DropDownMenu;
#pragma endregion

	GENERATED_BODY()
	
//@Defualt Setting
#pragma region Default Setting
public:
    UConfirmationMenu(const FObjectInitializer& ObjectInitializer);

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
    //@내부 바인딩
    void InternalBindToButton(UCustomButton* Button, FName OptionName);

protected:
    //@초기화
    UFUNCTION()
        virtual void InitializeConfirmationMenu();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@Rest
    void ResetConfirmationMenu();
    //@Custom Button 생성
    virtual void CreateButton();

protected:
    //@확정 메뉴 창을 엽니다.
    UFUNCTION(BlueprintNativeEvent, Category = "Confirmation Menu")
        void OpenConfirmationMenu();
    virtual void OpenConfirmationMenu_Implementation();

    //@확정 메뉴 창을 닫습니다.
    UFUNCTION(BlueprintNativeEvent, Category = "Confirmation Menu")
        void CloseConfirmationMenu();
    virtual void CloseConfirmationMenu_Implementation();

protected:
    //@Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ConfirmationMenuOverlay;

protected:
    //@배경 이미지
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* ConfirmationMenuBGImage;

protected:
    //@세로 박스
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UVerticalBox* ConfirmationMenuBox;

protected:
    //@텍스트 박스
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UMultiLineEditableTextBox* ConfirmationMenuDialogue;

protected:
    //@가로 박스
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* ConfirmationMenuButtonBox;

protected:
    //@확정 메뉴의 Ok 버튼 Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ConfirmationMenuOkButtonOverlay;
    //@확정 메뉴의 Ok 버튼 이미지
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* ConfirmationMenuOkButtonImage;


protected:
    //@확정 메뉴의 Cancel 버튼 Overlay;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ConfirmationMenuCancelButtonOverlay;
    //@확정 메뉴의 Cancel 버튼 이미지
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* ConfirmationMenuCancelButtonImage;

protected:
    //@현재 선택된 확정 옵션(OK or CANCEL)
    TWeakObjectPtr<UCustomButton> CurrentSelectedConfirmationButton;
    //@TMap
    TMap<FName, TObjectPtr<UCustomButton>> MConfirmationMenuButtons;
    //@Custom Button Class-1
    UPROPERTY(EditDefaultsOnly, category = "Coonfirmation Menu | Button")
        TSubclassOf<UCustomButton> ConfirmationMenuOkButtonClass;
    //@Custom Button Class-2
    UPROPERTY(EditDefaultsOnly, category = "Coonfirmation Menu | Button")
        TSubclassOf<UCustomButton> ConfirmationMenuCancelButtonClass;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 완료 이벤트
    FConfirmationMenuInitFinished ConfirmationMenuInitFinished;

public:
    //@확정 메뉴 버튼 Click 이벤트
    FConfirmationMenuButtonClicked ConfirmationMenuButtonClicked;
    //@확정 메뉴 버튼  Hoer 이벤트
    FConfirmationMenuButtonHovered ConfirmationMenuButtonHovered;
    //@확정 메뉴 버튼 Unhover 이벤트
    FConfirmationMenuButtonUnhovered ConfirmationMenuButtonUnhovered;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Inventory Tool Bar 버튼 클릭 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnConfirmationMenuButtonClicked(FName OptionName);
    virtual void OnConfirmationMenuButtonClicked_Implementation(FName OptionName);
    //@Inventory Tool Bar 버튼 Hover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnConfirmationMenuButtonHovered(FName OptionName);
    virtual void OnConfirmationMenuButtonHovered_Implementation(FName OptionName);
    //@Inventory Tool Bar 버튼 Unhover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnConfirmationMenuButtonUnhovered(FName OptionName);
    virtual void OnConfirmationMenuButtonUnhovered_Implementation(FName OptionName);

protected:
    //@Inventory Tool Bar 버튼 선택 취소 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void CancelConfirmationMenuButtonSelected(FName PreviousOptionName);
    virtual void CancelConfirmationMenuButtonSelected_Implementation(FName PreviousOptionName);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    FText GetConfirmationMenuDialogueText() const;
    void SetConfirmationMenuDialogueText(FText Text);

protected:
    //@Multi Line Text Box에서 '.'기준 줄바꿈 정리
    FText ArrangeDialogueText(FText Text);
#pragma endregion

};
