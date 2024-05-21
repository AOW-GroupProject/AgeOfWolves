// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::OnRegister()
{
	Super::OnRegister();
}


void UInventoryComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// @TODO: PS의 HUD에 Quick Slot Item 연동
}

void UInventoryComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

//void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}
