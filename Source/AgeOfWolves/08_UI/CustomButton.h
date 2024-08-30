#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CustomButton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCustomButton, Log, All);

class UOverlay;
class UImage;
class UButton;

#pragma region Enums
/**
* EButtonState
* 
* Button의 상태를 나타내는 열거형
*/
UENUM(BlueprintType)
enum class EButtonState : uint8
{
    Normal = 0      UMETA(DisplayName = "Normal"),
    Hovered         UMETA(DisplayName = "Hovered"),
    Pressed         UMETA(DisplayName = "Pressed"),
    Selected        UMETA(DisplayName = "Selected"),
    Disabled        UMETA(DisplayName = "Disabled"),
    MAX,
};
#pragma endregion

#pragma region Structs
/**
* FButtonStateInformation
* 
* Button 상태와 이와 관련된 Texture를 담고 있는 구조체
*/
USTRUCT(BlueprintType)
struct FButtonStateInformation
{
    GENERATED_BODY()
    //@Texture2D에 대응되는 버튼 상태
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
        EButtonState State;
    //@버튼 상태에 대응되는 Texture
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
        TSoftObjectPtr<UTexture2D> Texture;
};
#pragma endregion

#pragma region Delegates
DECLARE_MULTICAST_DELEGATE(FButtonHovered);
DECLARE_MULTICAST_DELEGATE(FButtonUnhovered);
DECLARE_MULTICAST_DELEGATE(FButtonSelected);
DECLARE_MULTICAST_DELEGATE(FButtonDisabled);
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UCustomButton : public UUserWidget
{
    GENERATED_BODY()
        //@Friend Class 설정
        friend class UInteractableItemSlot;

#pragma region Default Setting
public:
    UCustomButton(const FObjectInitializer& ObjectInitializer);

protected:
    //~Interface Of UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~End Of UUserWidget
#pragma endregion

#pragma region SubWidgets
protected:
    //@Overlay
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
        UOverlay* ButtonOverlay;
    //@Button
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
        UButton* Button;
    //@Image
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
        UImage* ButtonImage;

private:
    //@Button State를 설정합니다.
    void SetButtonState(EButtonState NewState);
protected:
    //@현재 버튼의 상태를 나타냅니다.
    UPROPERTY(BlueprintReadOnly, Category = "Button")
        EButtonState CurrentButtonState;
    //@버튼 상태별 정보를 담고 있는 구조체 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
        TArray<FButtonStateInformation> ButtonStateInfos;

protected:
    //@버튼 이미지를 업데이트합니다.
    UFUNCTION(BlueprintCallable, Category = "Button")
        void UpdateButtonImage();

protected:
    //@버튼의 상호작용 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Button")
        void ActivateButton();
    //@버트느이 상호작용 비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Button")
        void DeactivateButton(bool bIsClicked = false);
#pragma endregion

#pragma region Callbacks
public:
    //@버튼의 Hovered 이벤트 구독
    UFUNCTION(BlueprintNativeEvent, Category = "Button")
        void OnButtonHovered();
    virtual void OnButtonHovered_Implementation();
    //@버튼의 Unohoverd 이벤트 구독
    UFUNCTION(BlueprintNativeEvent, Category = "Button")
        void OnButtonUnhovered();
    virtual void OnButtonUnhovered_Implementation();
    //@버튼의 Pressed 이벤트 구독
    UFUNCTION(BlueprintNativeEvent, Category = "Button")
        void OnButtonPressed();
    virtual void OnButtonPressed_Implementation();
    //@버튼의 Clicked 이벤트 구독
    UFUNCTION(BlueprintNativeEvent, Category = "Button")
        void OnButtonClicked();
    virtual void OnButtonClicked_Implementation();
    //@버튼의 Canceled 이벤트 구독
    //@주의: 이를 활용하는 곳에서 Cancel 관련 이벤트 정의 및 아래 콜백과 바인딩 수행
    UFUNCTION(BlueprintNativeEvent, Category = "Button")
        void ButtonCanceledNotified();
    virtual void ButtonCanceledNotified_Implementation();
#pragma endregion

#pragma region
public:
    //@Button의 Hover 이벤트
    FButtonHovered ButtonHovered;
    //@Button의 Clicked 이벤트
    FButtonSelected ButtonSelected;
    //@Button의 Unhovered 이벤트
    FButtonUnhovered ButtonUnhovered;
    //@Button의 Disabled 이벤트
    FButtonDisabled ButtonDisabled;
#pragma endregion

#pragma region Callbacks
public:
    FORCEINLINE UButton* GetButton() { return Button; }
#pragma endregion
};