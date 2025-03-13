#include "ObjectiveDetectionComponent.h"

UObjectiveDetectionComponent::UObjectiveDetectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UObjectiveDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
}
