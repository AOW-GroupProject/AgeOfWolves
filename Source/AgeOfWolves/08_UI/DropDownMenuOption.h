#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DropDownMenuOption.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDropDownMenuOption, Log, All)

#pragma region Forward Declaration
class UOverlay;
class UCustomButton;
class UEditableTextBox;
#pragma endregion

#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FDropDownMenuOptionInitFinished);

//@옵션 선택 이벤트
DECLARE_DELEGATE_OneParam(FDropDownMenuOptionSelected, const FText&);
//@옵션 호버 이벤트
DECLARE_DELEGATE_OneParam(FDropDownMenuOptionHovered, const FText&);
//@옵션 언호버 이벤트
DECLARE_DELEGATE_OneParam(FDropDownMenuOptionUnhovered, const FText&);

//@옵션 취소 이벤트
DECLARE_MULTICAST_DELEGATE(FNotifyDropDownMenuOptionCanceled);
#pragma endregion

/**
 * @UDropDownMenuOption
 *
 * Drop Down Menu의 개별 옵션을 구현합니다.
 */
UCLASS()
class AGEOFWOLVES_API UDropDownMenuOption : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UDropDownMenuOption(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    // 내부 바인딩 함수
    void InternalBindToOptionButton(UCustomButton* DropDownMenuOptionButton);

public:
    // 초기화
    UFUNCTION()
        void InitializeDropDownMenuOption();
#pragma endregion

#pragma region Subwidgets
protected:
    //@CustomButton 생성
    void CreateButton();

protected:
    //@Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* DropDownMenuOptionOverlay;

protected:
    //@Editable Text Block
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UEditableTextBox* DropDownMenuOptionText;

    //@각 옵션의 Height 값
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Option Text")
        float OptionHeight = 0;
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Option Text")
        float UpUnderPadding = 0;
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Option Text")
        float LeftRightPadding= 0;

protected:
    //@Button
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Button")
        TSubclassOf<UCustomButton> DropDownMenuOptionButtonClass;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 완료 이벤트
    FDropDownMenuOptionInitFinished DropDownMenuOptionInitFinished;

public:
    //@옵션 버튼 Clicked/Selected 이벤트
    FDropDownMenuOptionSelected DropDownMenuOptionSelected;
    //@옵션 버튼 Hover 이벤트
    FDropDownMenuOptionHovered DropDownMenuOptionHovered;
    //@옵션 버튼 Unhover 이벤트
    FDropDownMenuOptionUnhovered DropDownMenuOptionUnhovered;

public:
    FNotifyDropDownMenuOptionCanceled NotifyDropDownMenuOptionCanceled;
#pragma endregion

#pragma region Callbacks
protected:
    //@Button Clicked 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnDropDownMenuOptionButtonClicked();
    //@Button Hovered 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnDropDownMenuOptionButtonHovered();
    //@Button Unhovered 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnDropDownMenuOptionButtonUnhovered();

protected:
    //@Button의 선택 취소 이벤트에 등록되는 콜백
    UFUNCTION()
        void DropDownMenuOptionButtonCanceledNotified(const FText& OptionName);
#pragma endregion

#pragma region Utilities
public:
    UFUNCTION(BlueprintCallable)
        void SetOptionName(FText Text);

public:
    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetOptionHeight() { return OptionHeight; }
    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetUpUnderPadding() { return UpUnderPadding; }
    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetLeftRightPadding() { return LeftRightPadding; }
#pragma endregion
};