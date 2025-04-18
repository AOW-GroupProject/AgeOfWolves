// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropDownMenuOption.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDropDownMenuOption, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UCustomButton;
class UEditableTextBox;
class UHorizontalBox;
class UImage;
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
DECLARE_DELEGATE(FDropDownMenuOptionInitFinished);

//@옵션 선택 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FDropDownMenuOptionSelected, FName, EInteractionMethod);
//@옵션 호버 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FDropDownMenuOptionHovered, FName, EInteractionMethod);
//@옵션 언호버 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FDropDownMenuOptionUnhovered, FName);

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
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
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
    virtual void InternalBindToOptionButton(UCustomButton* DropDownMenuOptionButton);

public:
    // 초기화
    UFUNCTION()
        virtual void InitializeDropDownMenuOption();
#pragma endregion

//@Property/Info...etc
#pragma region Subwidgets
protected:
    //@CustomButton 생성
    virtual void CreateButton();

protected:
    //@Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* DropDownMenuOptionOverlay;

protected:
    //@Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* DropDownMenuOptionButtonOverlay;

    //@Button
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Button")
        TSubclassOf<UCustomButton> DropDownMenuOptionButtonClass;

protected:
    //@Option Text를 담을 Horizontal Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* DropDownMenuOptionTextBox;

protected:
    //@Option Text와 상호작용 가능한 키 정보 이미지
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* DropDownMenuOptionHotKeyInfoOverlay;

    //@Option Text와 상호작용 가능한 키 정보 이미지
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* DropDownMenuOptionHotKeyInfoImage;

    //@Option Text와 상호작용 가능한 키 Text
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UEditableTextBox* DropDownMenuOptionHotKeyText;

protected:
    //@Option에 나타낼 Text
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UEditableTextBox* DropDownMenuOptionText;

protected:
    //@Option 버튼
    TObjectPtr<UCustomButton> Button;

protected:
    //@각 옵션의 Height 값
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Option Text")
        float OptionHeight = 0;
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Option Text")
        float UpUnderPadding = 0;
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu Option | Option Text")
        float LeftRightPadding = 0;
#pragma endregion

//@Delegates
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

//@Callbacks
#pragma region Callbacks
protected:
    //@Button Clicked 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnDropDownMenuOptionButtonClicked(EInteractionMethod InteractionMethodType);
    virtual void OnDropDownMenuOptionButtonClicked_Implementation(EInteractionMethod InteractionMethodType);
    //@Button Hovered 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnDropDownMenuOptionButtonHovered(EInteractionMethod InteractionMethodType);
    virtual void OnDropDownMenuOptionButtonHovered_Implementation(EInteractionMethod InteractionMethodType);

    //@Button Unhovered 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnDropDownMenuOptionButtonUnhovered();
    virtual void OnDropDownMenuOptionButtonUnhovered_Implementation();

protected:
    //@Button의 선택 취소 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void DropDownMenuOptionButtonCanceledNotified(FName OptionName);
    virtual void DropDownMenuOptionButtonCanceledNotified_Implementation(FName OptionName);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utilities
public:
    UFUNCTION(BlueprintCallable)
        FName GetOptionName() const;
    UFUNCTION(BlueprintCallable)
        void SetOptionName(FText Text);

public:
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu Option")
        void SetDropDownMenuOptionHotKeyInfoImage(UTexture2D* Texture);

    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu Option")
        UImage* GetDropDownMenuOptionHotKeyInfoImage();

public:
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu Option")
        void SetDropDownMenuOptionHotKeyText(const FText& Text);

    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu Option")
        FText GetDropDownMenuOptionHotKeyText() const;

public:
    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetOptionHeight() { return OptionHeight; }

    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetUpUnderPadding() { return UpUnderPadding; }

    UFUNCTION(BlueprintCallable)
        FORCEINLINE float GetLeftRightPadding() { return LeftRightPadding; }

public:
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        UCustomButton* GetDropDownMenuOptionButton() const;
#pragma endregion
};