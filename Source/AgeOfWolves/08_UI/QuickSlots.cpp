// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlots.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "08_UI/QuickSlot.h"

DEFINE_LOG_CATEGORY(LogQuickSlots)

UQuickSlots::UQuickSlots(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PotionSlotList = nullptr;
	ToolSlotList = nullptr;
}

void UQuickSlots::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(QuickSlotClass);

	// @설명 : Potion Slot List에 ToolSlotNum 크기 만큼의 Quick Slot을 추가합니다.
	for (uint8 i = 0; i < PotionSlotNum; ++i)
	{
		UQuickSlot* QuickSlot = NewObject<UQuickSlot>(this, QuickSlotClass);
		if (IsValid(QuickSlot))
		{
			if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(PotionSlotList->AddChildToVerticalBox(QuickSlot)))
			{
				VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				if (i < PotionSlotNum - 1)
				{
					VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 20.f));
				}
			}
		}
	}

	// @설명 : Tool Slot List에 ToolSlotNum 크기 만큼의 Quick Slot을 추가합니다.
	for (uint8 i = 0; i < ToolSlotNum; ++i)
	{
		UQuickSlot* QuickSlot = NewObject<UQuickSlot>(this, QuickSlotClass);
		if (IsValid(QuickSlot))
		{
			if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(ToolSlotList->AddChildToVerticalBox(QuickSlot)))
			{
				VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				if (i < ToolSlotNum - 1)
				{
					VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 40.f));
					UE_LOGFMT(LogQuickSlots, Error, "Padding");
				}
			}
		}
	}

}

void UQuickSlots::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UQuickSlots::NativeConstruct()
{
	Super::NativeConstruct();
}

void UQuickSlots::NativeDestruct()
{
	Super::NativeDestruct();
}
