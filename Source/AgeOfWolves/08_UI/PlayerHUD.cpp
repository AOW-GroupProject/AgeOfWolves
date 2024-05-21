// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Logging/StructuredLog.h"	

#include "08_UI/QuickSlots.h"
#include "08_UI/StateBars.h"

#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"

DEFINE_LOG_CATEGORY(LogHUD)


UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// #1. HUD 위젯의 구성 요소 중 State Bars 위젯을 초기화합니다.
	{
		if (IsValid(StateBarsBox))
		{
			check(StateBarsClass);

			UStateBars* StateBars = NewObject<UStateBars>(this, StateBarsClass);
			if (IsValid(StateBars))
			{
				// @설명: Scale Box 내부 State Bars 위젯 추가 및 편집
				if (UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(StateBarsBox->AddChild(StateBars)))
				{
					// 정렬 방식
					ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
					ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
				}
			}
		}
	}

	// #2. HUD 위젯의 구성 요소 중 Quick Slots 위젯을 초기화합니다.
	{
		if (IsValid(QuickSlotsBox))
		{
			check(QuickSlotsClass);

			UQuickSlots* QuickSlots = NewObject<UQuickSlots>(this, QuickSlotsClass);
			if (IsValid(QuickSlots))
			{
				// @설명: Scale Box 내부 Quicks Slots 위젯 추가 및 편집
				if (UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(QuickSlotsBox->AddChild(QuickSlots)))
				{
					// 정렬 방식
					ScaleBoxSlot->SetHorizontalAlignment(HAlign_Right);
					ScaleBoxSlot->SetVerticalAlignment(VAlign_Bottom);
				}
			}
		}
	}

}

void UPlayerHUD::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

}

void UPlayerHUD::NativeDestruct()
{
	Super::NativeConstruct();

}

