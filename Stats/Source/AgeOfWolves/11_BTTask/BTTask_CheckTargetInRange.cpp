#include "BTTask_CheckTargetInRange.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogBTTask_CheckTargetInRange)

UBTTask_CheckTargetInRange::UBTTask_CheckTargetInRange(const FObjectInitializer& ObjectInitializer)
{
    bCreateNodeInstance = true;
    NodeName = "Check In Range";
}

EBTNodeResult::Type UBTTask_CheckTargetInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	ABaseAIController* EnemyAIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	APawn* OwningPawn = EnemyAIController->GetPawn();
	UBlackboardComponent* BBComponent = EnemyAIController->GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));

	if (!EnemyAIController || !OwningPawn || !BBComponent || !TargetActor) return EBTNodeResult::Failed;

	float Max_SkillRange = EnemyAIController->GetMaxAttackRange();

	float DistanceBetween = FMath::Abs(UKismetMathLibrary::Distance2D(FVector2D(TargetActor->GetActorLocation()), FVector2D(OwningPawn->GetActorLocation())));

	if (DistanceBetween <= Max_SkillRange)
	{
		BBComponent->SetValueAsBool("InRange", true);
	}
	else
	{
		BBComponent->SetValueAsBool("InRange", false);
	}

	return EBTNodeResult::Succeeded;

}
