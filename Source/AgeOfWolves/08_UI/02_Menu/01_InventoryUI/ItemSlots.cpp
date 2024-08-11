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

	//@외부 바인딩
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
	//@초기화 완료 이벤트
	//ItemSlotsInitFinished.ExecuteIfBound();
}
