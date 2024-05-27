// Fill out your copyright notice in the Description page of Project Settings.


#include "09_Monster/BTTask_MoveAlongSplinePoint.h"
#include "MonsterInterface_Spline.h"
#include "BaseMonster_Spline.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_MoveAlongSplinePoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (!Cast<IMonsterInterface_Spline>(OwnerComp.GetAIOwner()->GetPawn())) return EBTNodeResult::Failed;


	bNotifyTick = true;
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveAlongSplinePoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (Cast<IMonsterInterface_Spline>(OwnerComp.GetAIOwner()->GetPawn())->WhenMoveToSplinePointReturnTrue())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
