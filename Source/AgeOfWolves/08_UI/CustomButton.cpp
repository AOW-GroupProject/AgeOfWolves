
#include "CustomButton.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Animation/WidgetAnimation.h" 

DEFINE_LOG_CATEGORY(LogCustomButton);

//@Defualt Setting
#pragma region Default Setting
UCustomButton::UCustomButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ButtonOverlay = nullptr;
    Button = nullptr;
    ButtonImage = nullptr;
}

void UCustomButton::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@자체 바인딩
    if (Button)
    {
        Button->OnHovered.AddDynamic(this, &UCustomButton::OnButtonHovered);
        Button->OnUnhovered.AddDynamic(this, &UCustomButton::OnButtonUnhovered);
        Button->OnPressed.AddDynamic(this, &UCustomButton::OnButtonPressed);
        Button->OnClicked.AddDynamic(this, &UCustomButton::OnButtonClicked);
    }

    //@애니메이션 완료 콜백 바인딩
    if (BlendOutAnimation)
    {
        FWidgetAnimationDynamicEvent AnimFinishedEvent;
        AnimFinishedEvent.BindDynamic(this, &UCustomButton::OnBlendOutAnimationFinished);
        BindToAnimationFinished(BlendOutAnimation, AnimFinishedEvent);
    }

    SetButtonState(EButtonState::Normal);
}

void UCustomButton::NativePreConstruct()
{
    Super::NativePreConstruct();

    SetIsFocusable(true);
}

void UCustomButton::NativeConstruct()
{
    Super::NativeConstruct();
}

void UCustomButton::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UCustomButton::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);

}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UCustomButton::SetButtonState(EButtonState NewState)
{
    if (CurrentButtonState != NewState)
    {
        UE_LOGFMT(LogCustomButton, Log, "{0}의 상태가 {1}에서 {2}로 변경됨", *GetName(),
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
            UTexture2D* LoadedTexture = StateInfo.Texture.LoadSynchronous();
            if (!LoadedTexture)
            {
                //@Normal 상태일 경우 ButtonImage를 비우고 tint를 0로 설정
                ButtonImage->SetBrushFromTexture(nullptr);
                ButtonImage->SetBrushTintColor(FLinearColor::Transparent);
                UE_LOGFMT(LogCustomButton, Log, "{0}의 이미지가 {1} 상태로 초기화됨", *GetName(), StaticEnum<EButtonState>()->GetNameStringByValue((int64)CurrentButtonState));
            }
            else
            {
                //@다른 상태의 경우 해당 Texture 적용
                ButtonImage->SetBrushFromTexture(LoadedTexture);
                ButtonImage->SetBrushTintColor(FLinearColor::White);
                ButtonImage->SetColorAndOpacity(FLinearColor::White);
                UE_LOGFMT(LogCustomButton, Log, "{0}의 이미지가 {1} 상태로 업데이트됨", *GetName(), StaticEnum<EButtonState>()->GetNameStringByValue((int64)CurrentButtonState));
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
    //@Button
    if (!Button)
    {
        UE_LOGFMT(LogCustomButton, Log, "버튼이 유효하지 않습니다.");
        return;
    }

    //@bClick
    if (!bIsClicked)
    {
        UE_LOGFMT(LogCustomButton, Error, "Decati");
        //@Button State
        SetButtonState(EButtonState::Disabled);
    }
    else
    {
        //@Button State
        SetButtonState(EButtonState::Selected);
    }

    //@Button의 상호 작용 비활성화
    Button->SetIsEnabled(false);

    //@Disabled 이벤트
    ButtonDisabled.Broadcast();
}

bool UCustomButton::SetButtonHoveredByKeyboard_Implementation()
{
    if (CurrentButtonState == EButtonState::Disabled
        || CurrentButtonState == EButtonState::Selected
        || CurrentButtonState == EButtonState::Hovered)
    {
        return false;
    }

    if (BlendInAndOutAnimation)
    {
        PlayAnimation(BlendInAndOutAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward);  
    }

    //@Button State를 Hovered 상태로 전환
    SetButtonState(EButtonState::Hovered);

    //@Button의 호버 이벤트
    ButtonHovered.Broadcast(EInteractionMethod::Keyboard);

    UE_LOGFMT(LogCustomButton, Error, "Hovered");

    return true;
}

bool UCustomButton::SetButtonSelectedByKeyboard_Implementation()
{
    if (CurrentButtonState == EButtonState::Disabled)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 비활성화 상태입니다. Click 무시.");
        return false;
    }

    //@Animation
    if (BlendInAndOutAnimation && IsAnimationPlaying(BlendInAndOutAnimation))
    {
        StopAnimation(BlendInAndOutAnimation);
    }

    //@Clicke에 의한 비활성화
    DeactivateButton(true);

    //@Clicked/Selected 이벤트
    ButtonSelected.Broadcast(EInteractionMethod::Keyboard);

    UE_LOGFMT(LogCustomButton, Log, "버튼이 선택되었습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
    return true;

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UCustomButton::OnButtonHovered_Implementation()
{
    if (CurrentButtonState == EButtonState::Disabled
        || CurrentButtonState == EButtonState::Selected
        || CurrentButtonState == EButtonState::Hovered)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 비활성화 혹은 이미 선택된 상태입니다. Hover 무시.");
        return;
    }

    if (BlendInAndOutAnimation)
    {
        PlayAnimation(BlendInAndOutAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward);
    }


    SetButtonState(EButtonState::Hovered);

    ButtonHovered.Broadcast(EInteractionMethod::Mouse);

    UE_LOGFMT(LogCustomButton, Log, "버튼에 마우스가 올라갔습니다.");
}

void UCustomButton::OnButtonUnhovered_Implementation()
{
    if (CurrentButtonState != EButtonState::Hovered)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 Hovered 상태가 아닙니다. Unhover 무시.");
        return;
    }

    if (!bLockAsHovered)
    {
        if (BlendInAndOutAnimation && IsAnimationPlaying(BlendInAndOutAnimation))
        {
            StopAnimation(BlendInAndOutAnimation);
        }

        if (BlendOutAnimation && !IsAnimationPlaying(BlendInAndOutAnimation))
        {
            PlayAnimation(BlendOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
        }

        SetButtonState(EButtonState::Normal);

        ButtonUnhovered.Broadcast();
    }

    UE_LOGFMT(LogCustomButton, Log, "버튼에서 마우스가 벗어났습니다.");
}

void UCustomButton::OnButtonPressed_Implementation()
{
    //@Button이 Hovered 상태가 아니면 무시
    if (CurrentButtonState != EButtonState::Hovered)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 Hovered 상태가 아닙니다. Press 무시.");
        return;
    }

    //@Set Button State
    SetButtonState(EButtonState::Pressed);

    //@즉시 클릭 동작 수행
    OnButtonClicked();

    UE_LOGFMT(LogCustomButton, Log, "버튼이 눌렸고 클릭되었습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}

void UCustomButton::OnButtonClicked_Implementation()
{
    if (CurrentButtonState != EButtonState::Pressed)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 비활성화 상태입니다. Click 무시.");
        return;
    }

    // 애니메이션 정지
    if (BlendInAndOutAnimation && IsAnimationPlaying(BlendInAndOutAnimation))
    {
        StopAnimation(BlendInAndOutAnimation);
    }

    DeactivateButton(true);

    ButtonSelected.Broadcast(EInteractionMethod::Mouse);

    UE_LOGFMT(LogCustomButton, Log, "버튼이 선택되었습니다.");
}

void UCustomButton::CancelSelectedButton_Implementation()
{
    if (CurrentButtonState != EButtonState::Hovered
        && CurrentButtonState != EButtonState::Selected)
    {
        UE_LOGFMT(LogCustomButton, Verbose, "버튼이 Hovered 혹은 Selected 상태가 아닙니다. Cancel 무시.");
        return;
    }

    if (BlendOutAnimation && !IsAnimationPlaying(BlendInAndOutAnimation))
    {
        PlayAnimation(BlendOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
    }

    //@Button 상호작용 활성화
    ActivateButton();

    UE_LOGFMT(LogCustomButton, Log, "버튼 선택이 취소되었습니다.");

    //@블루프린트에서 가져와 오버라이딩 합니다...
    //@eg. 애니메이션
}

void UCustomButton::OnBlendOutAnimationFinished()
{
    if (ButtonImage)
    {
        ButtonImage->SetRenderOpacity(1.f);
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion