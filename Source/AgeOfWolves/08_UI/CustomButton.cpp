#include "CustomButton.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

DEFINE_LOG_CATEGORY(LogCustomButton);

#pragma region Default Setting
UCustomButton::UCustomButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    CurrentButtonState = EButtonState::Normal;
}

void UCustomButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@Update Butotn Image
    UpdateButtonImage();
    //@자체 바인딩
    if (Button)
    {
        Button->OnHovered.AddDynamic(this, &UCustomButton::OnButtonHovered);
        Button->OnUnhovered.AddDynamic(this, &UCustomButton::OnButtonUnhovered);
        Button->OnPressed.AddDynamic(this, &UCustomButton::OnButtonPressed);
        Button->OnClicked.AddDynamic(this, &UCustomButton::OnButtonClicked);
    }
}

void UCustomButton::NativePreConstruct()
{
    Super::NativePreConstruct();


}

void UCustomButton::NativeConstruct()
{
    Super::NativeConstruct();
}

void UCustomButton::NativeDestruct()
{
    Super::NativeDestruct();
}
#pragma endregion

#pragma region SubWidgets
void UCustomButton::SetButtonState(EButtonState NewState)
{
    if (CurrentButtonState != NewState)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "{0}의 상태가 {1}에서 {2}로 변경됨", *GetName(),
            StaticEnum<EButtonState>()->GetNameStringByValue((int64)CurrentButtonState),
            StaticEnum<EButtonState>()->GetNameStringByValue((int64)NewState));
        //@Curretn Button State
        CurrentButtonState = NewState;
        //@Update Button Image
        UpdateButtonImage();
    }
}

void UCustomButton::UpdateButtonImage()
{
    //@Button Image
    if (!ButtonImage)
    {
        UE_LOGFMT(LogCustomButton, Error, "{0}에 ButtonImage가 없음", *GetName());
        return;
    }

    //@FButtonStateInformation
    for (auto& StateInfo : ButtonStateInfos)
    {
        if (StateInfo.State == CurrentButtonState)
        {
            if (CurrentButtonState == EButtonState::Normal
                || CurrentButtonState == EButtonState::Disabled)
            {
                //@Normal 상태일 경우 ButtonImage를 비우고 tint를 0로 설정
                ButtonImage->SetBrushFromTexture(nullptr);
                ButtonImage->SetColorAndOpacity(FLinearColor::Transparent);
                UE_LOGFMT(LogCustomButton, Verbose, "{0}의 이미지가 Normal 상태로 초기화됨", *GetName());
            }
            else
            {
                //@다른 상태의 경우 해당 Texture 적용
                ButtonImage->SetBrushFromTexture(StateInfo.Texture.LoadSynchronous());
                ButtonImage->SetBrushTintColor(FSlateColor(FLinearColor::White));
                ButtonImage->SetColorAndOpacity(FLinearColor::White);
                UE_LOGFMT(LogCustomButton, Verbose, "{0}의 이미지가 {1} 상태로 업데이트됨", *GetName(), StaticEnum<EButtonState>()->GetNameStringByValue((int64)CurrentButtonState));
            }
            break;
        }
    }
}

void UCustomButton::ActivateButton()
{
    if (!Button)
    {
        UE_LOGFMT(LogCustomButton, Log, "버튼이 유효하지 않습니다.");
        return;
    }
    //@Button의 상호 작용 활성화
    Button->SetIsEnabled(true);
    //@Button State
    SetButtonState(EButtonState::Normal);
}

void UCustomButton::DeactivateButton(bool bIsClicked)
{
    if (!Button)
    {
        UE_LOGFMT(LogCustomButton, Log, "버튼이 유효하지 않습니다.");
            return;
    }
    //@Button의 상호 작용 비활성화
    Button->SetIsEnabled(false);
    //@bClick
    if (!bIsClicked)
    {
        //@Button State
        SetButtonState(EButtonState::Disabled);
    }
    else
    {
        //@Button State
        SetButtonState(EButtonState::Selected);
    }
    //@Disabled 이벤트
    ButtonDisabled.Broadcast();
}
#pragma endregion

#pragma region Callbacks
void UCustomButton::OnButtonHovered_Implementation()
{
    if (CurrentButtonState == EButtonState::Disabled
        || CurrentButtonState == EButtonState::Selected)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 비활성화 혹은 이미 선택된 상태입니다. Hover 무시.");
        return;
    }

    //@Button State
    SetButtonState(EButtonState::Hovered);
    //@Delegate
    ButtonHovered.Broadcast();

    UE_LOGFMT(LogCustomButton, Log, "버튼에 마우스가 올라갔습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}

void UCustomButton::OnButtonUnhovered_Implementation()
{
    if (CurrentButtonState != EButtonState::Hovered)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 Hovered 상태가 아닙니다. Unhover 무시.");
        return;
    }

    //@Button State
    SetButtonState(EButtonState::Normal);
    //@Unhovered 이벤트
    ButtonUnhovered.Broadcast();

    UE_LOGFMT(LogCustomButton, Log, "버튼에서 마우스가 벗어났습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}

void UCustomButton::OnButtonPressed_Implementation()
{
    if (CurrentButtonState == EButtonState::Disabled
        || CurrentButtonState == EButtonState::Selected)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 비활성화 혹은 이미 선택된 상태입니다. Press 무시.");
        return;
    }

    //@Set Button State
    SetButtonState(EButtonState::Pressed);

    UE_LOGFMT(LogCustomButton, Log, "버튼이 눌렸습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}

void UCustomButton::OnButtonClicked_Implementation()
{
    if (CurrentButtonState == EButtonState::Disabled)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 비활성화 상태입니다. Click 무시.");
        return;
    }

    //@Clicke에 의한 비활성화
    DeactivateButton(true);

    //@Clicked/Selected 이벤트
    ButtonSelected.Broadcast();

    UE_LOGFMT(LogCustomButton, Log, "버튼이 선택되었습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}

void UCustomButton::ButtonCanceledNotified_Implementation()
{
    if (CurrentButtonState != EButtonState::Selected)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 Selected 상태가 아닙니다. Cancel 무시.");
        return;
    }

    //@Butotn 상호작용 활성화
    ActivateButton();

    UE_LOGFMT(LogCustomButton, Log, "버튼 선택이 취소되었습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}
#pragma endregion