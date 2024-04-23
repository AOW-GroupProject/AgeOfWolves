// Fill out your copyright notice in the Description page of Project Settings.


#include "TestWidget.h"

#include "03_Player/PlayerStateBase.h"


UTestWidget::UTestWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UTestWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (const auto PS = GetOwningPlayerState<APlayerStateBase>())
	{
		PS->OnAnyAttributeValueChanged.AddDynamic(this, &UTestWidget::OnAttributeValueChanged);
	}

}

void UTestWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UTestWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UTestWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UTestWidget::OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
	if (Attribute.IsValid())
	{
		if (Attribute.AttributeName == "Health")
		{
			UE_LOG(LogTemp, Log, TEXT("Health : %f"), NewValue);
		}
		else if (Attribute.AttributeName == "Mana")
		{
			UE_LOG(LogTemp, Log, TEXT("Mana : %f"), NewValue);
		}
		else if (Attribute.AttributeName == "Stamina")
		{
			UE_LOG(LogTemp, Log, TEXT("Stamina : %f"), NewValue);
		}
	}
}




