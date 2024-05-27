// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Monster/BaseMonster_Spline.h"
#include "Components/SplineComponent.h"
#include "BaseSpline.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"

ABaseMonster_Spline::ABaseMonster_Spline()
{
	PrimaryActorTick.bCanEverTick = true;
	//SplineComponent = Cast<ABaseSpline>(SplineBP)->GetSplineComponent();

}

void ABaseMonster_Spline::BeginPlay()
{
	Super::BeginPlay();
	SplineActor = Cast<ABaseSpline>(GetWorld()->SpawnActor<AActor>(SplineBP->GeneratedClass, GetActorLocation(), GetActorRotation()));
	CurrentState = EMonsterState::Patrol;
	
}

bool ABaseMonster_Spline::WhenMoveToSplinePointReturnTrue()
{
	if (!SplineActor) return false;
	if (CurrentState == EMonsterState::Patrol)
	{
		FVector TargetLocation = FVector(SplineActor->GetSplinePointWorldPosition().X, SplineActor->GetSplinePointWorldPosition().Y, GetActorLocation().Z);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), TargetLocation);
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("%f"), UKismetMathLibrary::Vector_Distance(GetActorLocation(), TargetLocation)));
		if (UKismetMathLibrary::Vector_Distance(GetActorLocation(), TargetLocation) <= 100)
		{
			SplineActor->IncrementSplineIndex();
			return true;
		}
		return false;
	}
	return false;
}
