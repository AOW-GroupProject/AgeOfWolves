// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/BaseMonster_Spline.h"
#include "10_Monster/BaseMonsterAIController.h"
#include "Components/SplineComponent.h"
#include "10_Monster/BaseSpline.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Math/UnrealMathUtility.h"


ABaseMonster_Spline::ABaseMonster_Spline()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
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
	Super::Tick(DeltaTime);
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

void ABaseMonster_Spline::WhenEndState()
{
	switch (CurrentState)
	{
	case EMonsterState::Patrol:
		SplineVectorBeforeDetectingPlayer = GetActorLocation();
		break;

	case EMonsterState::Strafe:

		break;
	case EMonsterState::Attacking:
		
		break;

	case EMonsterState::DetectingPlayer:

		break;
	case EMonsterState::Stunned:

		break;

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
	}
}

void ABaseMonster_Spline::SetSplineActor(ABaseSpline* spline)
{
	SplineActor = spline;
}

FVector ABaseMonster_Spline::GetSplineVectorToPatrol()
{
	return SplineVectorBeforeDetectingPlayer;
}

void ABaseMonster_Spline::WaitToReactivateDetecting()
{
	ABaseMonsterAIController* MonsterController = Cast<ABaseMonsterAIController>(GetController());
	
	if (MonsterController) MonsterController->SetIsPossibleDetecting(false);

	FTimerHandle GravityTimerHandle;
	float GravityTime = 3;

	GetWorld()->GetTimerManager().SetTimer(GravityTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			// 여기서 위에서 선언했던 MonsterController 변수 그대로 쓰면 null참조 오류남
			ABaseMonsterAIController* MonsterController = Cast<ABaseMonsterAIController>(GetController());
			if(MonsterController) MonsterController->SetIsPossibleDetecting(true);

			// TimerHandle 초기화
			GetWorld()->GetTimerManager().ClearTimer(GravityTimerHandle);
		}), GravityTime, false);
}

