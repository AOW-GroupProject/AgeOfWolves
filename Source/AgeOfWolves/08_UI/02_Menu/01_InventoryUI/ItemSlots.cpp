// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlots.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogItemSlots)
// UE_LOGFMT(LogItemSlots, Log, "");

UItemSlots::UItemSlots(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UItemSlots::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@�ܺ� ���ε�
}

void UItemSlots::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UItemSlots::NativeConstruct()
{
	Super::NativeConstruct();

}

void UItemSlots::NativeDestruct()
{
	Super::NativeDestruct();

}

void UItemSlots::InitializeItemSlots()
{
	//@�ʱ�ȭ �Ϸ� �̺�Ʈ
	//ItemSlotsInitFinished.ExecuteIfBound();
}
