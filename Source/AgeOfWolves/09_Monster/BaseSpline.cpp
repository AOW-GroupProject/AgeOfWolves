// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Monster/BaseSpline.h"
#include "Components/SplineComponent.h"

// Sets default values
ABaseSpline::ABaseSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SplinePointCount = SplineComponent->GetNumberOfSplinePoints();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("%d"), SplineIndex));
}

void ABaseSpline::IncrementSplineIndex()
{
	
	SplineIndex += 1;
	if (SplineIndex >= SplinePointCount - 1)
	{
		SplineIndex = 0;
	}
}

FVector ABaseSpline::GetSplinePointWorldPosition()
{
	return SplineComponent->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::World);
}

