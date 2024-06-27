// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/BaseMonster_Spline.h"
#include "Components/SplineComponent.h"
#include "10_Monster/BaseSpline.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Math/UnrealMathUtility.h"


ABaseMonster_Spline::ABaseMonster_Spline()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//SplineComponent = Cast<ABaseSpline>(SplineBP)->GetSplineComponent();

}

void ABaseMonster_Spline::BeginPlay()
{
	Super::BeginPlay();
	//SplineActor = Cast<ABaseSpline>(GetWorld()->SpawnActor<AActor>(SplineBP->GeneratedClass, GetActorLocation(), GetActorRotation()));
	CurrentState = EMonsterState::Patrol;

	if (SplineActor)
	{
		SplineActor->GetSplineComponent()->Duration = MaxTime;
		//시작지점이 0이 아닌경우일 수 있으므로 시작 Distance 초기화.
		Distance = SplineActor->GetSplineComponent()->GetDistanceAlongSplineAtSplinePoint(SplineActor->SplineIndex);
		//현재 인덱스와 다음 인덱스까지의 거리 차이를 저장하는 변수 초기화.
		//SplineIndexDistance = SplineActor->CalculateDistanceBetweenIndex(SplineActor->SplineIndex, SplineActor->SplineIndex + 1);
	}

	
	
}

void ABaseMonster_Spline::Tick(float DeltaTime)
{
	ControllRotation();
}

void ABaseMonster_Spline::ControllRotation()
{
	if (CurrentState == EMonsterState::Patrol)
	{
		if (GetVelocity().Size() > 0)
		{
			
			Rotation = FMath::RInterpTo(Rotation, GetControlRotation(), GetWorld()->DeltaTimeSeconds, 0.1);
			//GetController()->SetControlRotation(Rotation);
			//SetActorRotation(UKismetMathLibrary::MakeRotFromX(GetVelocity())); //이거 변경해야됨
		}
		
	}
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
			SplineActor->IncreaseSplineIndex();
			return true;
		}
		return false;
	}
	return false;
}


void ABaseMonster_Spline::MoveAlongSplinePoint(float delta)
{
	if (!SplineActor) return;
	if (CurrentState == EMonsterState::Patrol)
	{
		FVector tang = SplineActor->GetSplineComponent()->FindTangentClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
		GetCharacterMovement()->Velocity = tang.GetSafeNormal() * GetCharacterMovement()->MaxWalkSpeed;
#pragma region Distance에 따른 Tangent 활용해보기(나중에)

		//수정하고 싶은 점: 만약 거리가 굉장히 가까운 두 점이 있다면 경로를 건너뛰는 오류를 막기 위해 Distance를 사용하기

		//CurrentTime += delta;
		//Distance = FMath::Lerp(0, SplineActor->GetSplineComponent()->GetSplineLength(), CurrentTime / MaxTime);
		//BetweenDistance += delta;
		//Distance += GetCharacterMovement()->MaxWalkSpeed * delta; //1초에 200미터 이동
		////현재 진행거리가 다음 인덱스까지의 거리를 넘었다면
		//if (BetweenDistance > SplineIndexDistance)
		//{
		//	BetweenDistance = 0;
		//	SplineActor->IncreaseSplineIndex();
		//	SplineIndexDistance = SplineActor->CalculateDistanceBetweenIndex(SplineActor->SplineIndex, SplineActor->SplineIndex + 1);
		//}
		
		//FVector tang = SplineActor->GetSplineComponent()->GetTangentAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + tang, FColor::Green);

		
		
		
		/*if (CurrentTime >= MaxTime)
		{
			CurrentTime = 0;
		}*/
		/*if (Distance > SplineActor->GetSplineComponent()->GetSplineLength())
		{
			Distance = 0;
		}*/
#pragma endregion
	}
}

void ABaseMonster_Spline::SetSplineActor(ABaseSpline* spline)
{
	SplineActor = spline;
}

