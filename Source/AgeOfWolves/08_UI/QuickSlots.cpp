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

	// #1. Potion Slot List 초기화
	{
		// @설명 : Potion Slot List에 PotionSlotNum 크기 만큼의 Quick Slot을 추가합니다.
		for (uint8 i = 0; i < PotionSlotNum; ++i)
		{
			UQuickSlot* QuickSlot = NewObject<UQuickSlot>(this, QuickSlotClass);

			if (IsValid(QuickSlot))
			{
				// @TODO: 각 Quick Slot의 구성 요소들에 대한 초기화 작업이 필요합니다.
				// @TODO: Inventory Component로 부터 아이템 정보를 가져와야 합니다.
				// #1. Quick Slot 초기화
				QuickSlot->SetIsStackable(true);

				// #2. Quick Slot List 초기화
				if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(PotionSlotList->AddChildToVerticalBox(QuickSlot)))
				{
					// Size 설정
					VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
					if (i < PotionSlotNum - 1)
					{
						// Padding 설정
						VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 20.f));
					}
				}
			}
		}
	}

	// #2. Tool Slot List 초기화
	{
		// @설명 : Tool Slot List에 ToolSlotNum 크기 만큼의 Quick Slot을 추가합니다.
		for (uint8 i = 0; i < ToolSlotNum; ++i)
		{
			UQuickSlot* QuickSlot = NewObject<UQuickSlot>(this, QuickSlotClass);
			if (IsValid(QuickSlot))
			{
				// @TODO : 각 Quick Slot의 구성 요소들에 대한 초기화 작업이 필요합니다.
				// @TODO: Inventory Component로 부터 아이템 정보를 가져와야 합니다.
				// #1. Quick Slot 초기화

				// #2. Tool Slot List 초기화
				if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(ToolSlotList->AddChildToVerticalBox(QuickSlot)))
				{
					// #2. Size 설정
					VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
					if (i < ToolSlotNum - 1)
					{
						// #3. Padding 설정
						VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 40.f));
					}
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