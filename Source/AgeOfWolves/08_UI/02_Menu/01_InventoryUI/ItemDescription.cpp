// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDescription.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogItemDescription)


UItemDescription::UItemDescription(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UItemDescription::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UItemDescription::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UItemDescription::NativeConstruct()
{
	Super::NativeConstruct();

}

void UItemDescription::NativeDestruct()
{
	Super::NativeDestruct();

}

void UItemDescription::InitializeItemDescription()
{

	//@�ʱ�ȭ �Ϸ� �̺�Ʈ
	//ItemDescriptionInitFinished.ExecuteIfBound();
}
