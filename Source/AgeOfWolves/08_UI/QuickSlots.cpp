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

	// #1. Potion Slot List �ʱ�ȭ
	{
		// @���� : Potion Slot List�� PotionSlotNum ũ�� ��ŭ�� Quick Slot�� �߰��մϴ�.
		for (uint8 i = 0; i < PotionSlotNum; ++i)
		{
			UQuickSlot* QuickSlot = NewObject<UQuickSlot>(this, QuickSlotClass);

			if (IsValid(QuickSlot))
			{
				// @TODO: �� Quick Slot�� ���� ��ҵ鿡 ���� �ʱ�ȭ �۾��� �ʿ��մϴ�.
				// @TODO: Inventory Component�� ���� ������ ������ �����;� �մϴ�.
				// #1. Quick Slot �ʱ�ȭ
				QuickSlot->SetIsStackable(true);

				// #2. Quick Slot List �ʱ�ȭ
				if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(PotionSlotList->AddChildToVerticalBox(QuickSlot)))
				{
					// Size ����
					VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
					if (i < PotionSlotNum - 1)
					{
						// Padding ����
						VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 20.f));
					}
				}
			}
		}
	}

	// #2. Tool Slot List �ʱ�ȭ
	{
		// @���� : Tool Slot List�� ToolSlotNum ũ�� ��ŭ�� Quick Slot�� �߰��մϴ�.
		for (uint8 i = 0; i < ToolSlotNum; ++i)
		{
			UQuickSlot* QuickSlot = NewObject<UQuickSlot>(this, QuickSlotClass);
			if (IsValid(QuickSlot))
			{
				// @TODO : �� Quick Slot�� ���� ��ҵ鿡 ���� �ʱ�ȭ �۾��� �ʿ��մϴ�.
				// @TODO: Inventory Component�� ���� ������ ������ �����;� �մϴ�.
				// #1. Quick Slot �ʱ�ȭ

				// #2. Tool Slot List �ʱ�ȭ
				if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(ToolSlotList->AddChildToVerticalBox(QuickSlot)))
				{
					// #2. Size ����
					VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
					if (i < ToolSlotNum - 1)
					{
						// #3. Padding ����
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