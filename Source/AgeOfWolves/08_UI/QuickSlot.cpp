// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlot.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/EditableText.h"

DEFINE_LOG_CATEGORY(LogQuickSlot)


UQuickSlot::UQuickSlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SlotOverlay = nullptr;
	SlotImage = nullptr;
	SlotItemNum = nullptr;
	bUnique = true;
}

void UQuickSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UQuickSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UQuickSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void UQuickSlot::NativeDestruct()
{
	Super::NativeDestruct();

}
