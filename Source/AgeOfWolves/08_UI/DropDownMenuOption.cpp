// Fill out your copyright notice in the Description page of Project Settings.

#include "DropDownMenuOption.h"
#include "Logging/StructuredLog.h"

#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogDropDownMenuOption)

#pragma region Default Setting
UDropDownMenuOption::UDropDownMenuOption(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UDropDownMenuOption::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UDropDownMenuOption::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UDropDownMenuOption::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDropDownMenuOption::NativeDestruct()
{
    Super::NativeDestruct();
}

void UDropDownMenuOption::InternalBindToOptionButton(UCustomButton* DropDownMenuOptionButton)
{
    if (!DropDownMenuOptionButton)
    {
        UE_LOGFMT(LogDropDownMenuOption, Error, "Button이 null입니다. 바인딩을 수행할 수 없습니다.");
        return;
    }
    //@내부 바인딩
    DropDownMenuOptionButton->ButtonSelected.AddUObject(this, &UDropDownMenuOption::OnDropDownMenuOptionButtonClicked);
    DropDownMenuOptionButton->ButtonHovered.AddUObject(this, &UDropDownMenuOption::OnDropDownMenuOptionButtonHovered);
    DropDownMenuOptionButton->ButtonUnhovered.AddUObject(this, &UDropDownMenuOption::OnDropDownMenuOptionButtonUnhovered);

    UE_LOGFMT(LogDropDownMenuOption, Verbose, "CustomButton 이벤트가 성공적으로 바인딩되었습니다.");
}

void UDropDownMenuOption::InitializeDropDownMenuOption()
{
    //@CustomButton
    CreateButton();

    //@초기화 완료 이벤트
    DropDownMenuOptionInitFinished.ExecuteIfBound();
}

#pragma endregion

#pragma region Subwidgets
void UDropDownMenuOption::CreateButton()
{
    //@Overlay
    if (!DropDownMenuOptionOverlay)
    {
        UE_LOGFMT(LogDropDownMenuOption, Error, "RootWidget이 Overlay가 아니거나 null입니다. 위젯 블루프린트에서 올바르게 설정되었는지 확인하세요.");
        return;
    }

    //@DropDownMenuOptionButtonClass
    if (!DropDownMenuOptionButtonClass || !DropDownMenuOptionButtonOverlay)
    {
        UE_LOGFMT(LogDropDownMenuOption, Error, "DropDownMenuOptionButtonClass가 설정되지 않았습니다. 에디터에서 DropDownMenuOptionButtonClass를 설정해주세요.");
        return;
    }

    //@Create Widget
    UCustomButton* OptionButton = CreateWidget<UCustomButton>(this, DropDownMenuOptionButtonClass);
    if (!OptionButton)
    {
        UE_LOGFMT(LogDropDownMenuOption, Error, "UCustomButton 위젯을 생성하지 못했습니다. DropDownMenuOptionButtonClass: {0}", *DropDownMenuOptionButtonClass->GetName());
        return;
    }

    //@옵션 버튼 취소 이벤트 바인딩
    NotifyDropDownMenuOptionCanceled.AddUFunction(OptionButton, "CancelSelectedButton");

    //@내부 바인딩
    InternalBindToOptionButton(OptionButton);

    //@Add CustomButton To Overlay
    UOverlaySlot* ButtonOverlaySlot = DropDownMenuOptionButtonOverlay->AddChildToOverlay(OptionButton);
    if (!ButtonOverlaySlot)
    {
        UE_LOGFMT(LogDropDownMenuOption, Error, "CustomButton을 Overlay에 추가하지 못했습니다.");
        return;
    }

    //@Alignment
    ButtonOverlaySlot->SetHorizontalAlignment(HAlign_Fill);
    ButtonOverlaySlot->SetVerticalAlignment(VAlign_Fill);

}
#pragma endregion

#pragma region Callbacks
void UDropDownMenuOption::OnDropDownMenuOptionButtonClicked()
{
    //@DropDownMenuOptionText
    if (!DropDownMenuOptionText)
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼이 클릭되었지만, DropDownMenuOptionText가 유효하지 않습니다.");
        return;
    }

    //@FText
    FText OptionText = DropDownMenuOptionText->GetText();
    if (OptionText.IsEmpty())
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼이 클릭되었지만, DropDownMenuOptionText가 비어있습니다.");
        return;
    }

    //@Selected 이벤트
    DropDownMenuOptionSelected.ExecuteIfBound(OptionText);

    UE_LOGFMT(LogDropDownMenuOption, Log, "드롭다운 메뉴 옵션 버튼이 클릭되었습니다. 옵션: {0}", OptionText.ToString());
}

void UDropDownMenuOption::OnDropDownMenuOptionButtonHovered()
{
    //@DropDownMenuOptionText
    if (!DropDownMenuOptionText)
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼에 마우스가 올라갔지만, DropDownMenuOptionText가 유효하지 않습니다.");
        return;
    }

    //@FText
    FText OptionText = DropDownMenuOptionText->GetText();
    if (OptionText.IsEmpty())
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼에 마우스가 올라갔지만, DropDownMenuOptionText가 비어있습니다.");
        return;
    }

    //@Hover 이벤트
    DropDownMenuOptionHovered.ExecuteIfBound(OptionText);

    UE_LOGFMT(LogDropDownMenuOption, Log, "드롭다운 메뉴 옵션 버튼에 마우스가 올라갔습니다. 옵션: {0}", OptionText.ToString());

}

void UDropDownMenuOption::OnDropDownMenuOptionButtonUnhovered()
{
    //@DropDownMenuOptionText
    if (!DropDownMenuOptionText)
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼에서 마우스가 벗어났지만, DropDownMenuOptionText가 유효하지 않습니다.");
        return;
    }

    //@FText
    FText OptionText = DropDownMenuOptionText->GetText();
    if (OptionText.IsEmpty())
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼에서 마우스가 벗어났지만, DropDownMenuOptionText가 비어있습니다.");
        return;
    }

    //@Unhover 이벤트
    DropDownMenuOptionUnhovered.ExecuteIfBound(OptionText);

    UE_LOGFMT(LogDropDownMenuOption, Log, "드롭다운 메뉴 옵션 버튼에서 마우스가 벗어났습니다. 옵션: {0}", OptionText.ToString());
}

void UDropDownMenuOption::DropDownMenuOptionButtonCanceledNotified(const FText& OptionName)
{
    //@DropDownMenuOptionText
    if (!DropDownMenuOptionText)
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼 선택이 취소되었지만, DropDownMenuOptionText가 유효하지 않습니다.");
        return;
    }
    //@FText
    const FText CurrentOptionText = DropDownMenuOptionText->GetText();
    if (!CurrentOptionText.CompareTo(OptionName))
    {
        return;
    }

    if (CurrentOptionText.IsEmpty())
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "드롭다운 메뉴 옵션 버튼 선택이 취소되었지만, DropDownMenuOptionText가 비어있습니다.");
        return;
    }

    //@옵션 선택 취소 이벤트
    NotifyDropDownMenuOptionCanceled.Broadcast();

    UE_LOGFMT(LogDropDownMenuOption, Log, "드롭다운 메뉴 옵션 버튼 선택이 취소되었습니다. 옵션: {0}", CurrentOptionText.ToString());
}
#pragma endregion

#pragma region Utilities
void UDropDownMenuOption::SetOptionName(FText Text)
{
    if (!DropDownMenuOptionText)
    {
        return;
    }

    //@Set Text
    DropDownMenuOptionText->SetText(Text);
}

void UDropDownMenuOption::SetDropDownMenuOptionHotKeyInfoImage(UTexture2D* Texture)
{
    if (!Texture || !Texture->IsValidLowLevel())
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "Texture가 유효하지 않습니다!");
        return;
    }

    if (DropDownMenuOptionHotKeyInfoImage)
    {
        //@Set Brush From Texture
        DropDownMenuOptionHotKeyInfoImage->SetBrushFromTexture(Texture);
    }
    else
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "DropDownMenuOptionHotKeyInfoImage가 유효하지 않습니다!");
    }
}

UImage* UDropDownMenuOption::GetDropDownMenuOptionHotKeyInfoImage()
{
    if (!DropDownMenuOptionHotKeyInfoImage)
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "DropDownMenuOptionHotKeyInfoImage가 유효하지 않습니다!");
        return nullptr;
    }

    return DropDownMenuOptionHotKeyInfoImage;
}

void UDropDownMenuOption::SetDropDownMenuOptionHotKeyText(const FText& Text)
{
    if (DropDownMenuOptionHotKeyText)
    {
        DropDownMenuOptionHotKeyText->SetText(Text);
    }
    else
    {
        UE_LOGFMT(LogDropDownMenuOption, Warning, "DropDownMenuOptionHotKeyText가 유효하지 않습니다!");
    }
}

FText UDropDownMenuOption::GetDropDownMenuOptionHotKeyText() const
{
    if (DropDownMenuOptionHotKeyText)
    {
        return DropDownMenuOptionHotKeyText->GetText();
    }
    return FText::GetEmpty();
}
#pragma endregion