// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseSpline.h"
#include "01_Character/BaseMonster_Spline.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	//시작 인덱스가 최대 인덱스보다 크다면 조정
	if (SplineIndex >= SplineComponent->GetNumberOfSplinePoints())
	{
		SplineIndex = SplineComponent->GetNumberOfSplinePoints() - 1;
	}
	SpawnMonster();
}

// Called every frame
void ABaseSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABaseSpline::SpawnMonster()
{
	if (!MonsterClass) return;
	if (!MyMonster)
	{
		FRotator rot = UKismetMathLibrary::MakeRotFromX(SplineComponent->GetTangentAtSplinePoint(SplineIndex, ESplineCoordinateSpace::World));
		FVector loc = SplineComponent->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::World);
		FTransform trans = FTransform(rot, loc, FVector(1,1,1));

		MyMonster = GetWorld()->SpawnActorDeferred<ABaseMonster_Spline>(MonsterClass, trans); //MonsterClass->StaticClass() 사용하면 오류남
		
		if (MyMonster)
		{
			//FinishSpawning 전에 몬스터에게 본인 객체 전달
			MyMonster->SetSplineActor(this);
			MyMonster->FinishSpawning(trans);
		}
		
	}
}


void ABaseSpline::IncreaseSplineIndex()
{
	
	SplineIndex += 1;
	if (SplineIndex >= SplineComponent->GetNumberOfSplinePoints() - 1)
	{
		SplineIndex = 0;
	}
}


FVector ABaseSpline::GetSplinePointWorldPosition()
{
	return SplineComponent->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::World);
}
/*

int ABaseSpline::CalculateDistanceBetweenIndex(int a, int b)
{ 
	//최대 인덱스가 넘어갔다면 0으로 바꿔줌.
	if (b >= SplineComponent->GetNumberOfSplinePoints()) b = 0;
	float A;
	float B;
	if (b > a)
	{
		B = SplineComponent->GetDistanceAlongSplineAtSplinePoint(b);
		A = SplineComponent->GetDistanceAlongSplineAtSplinePoint(a);
		return B - A;
	}
	else
	{
		B = SplineComponent->GetSplineLength();
		A = SplineComponent->GetDistanceAlongSplineAtSplinePoint(a);
		return B - A;
	}
}
*/

