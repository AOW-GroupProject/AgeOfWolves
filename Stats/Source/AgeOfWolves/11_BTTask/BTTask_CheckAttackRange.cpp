#include "BTTask_CheckAttackRange.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "10_AI/BaseAIController.h"

#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogBTTask_CheckAttackRange)

UBTTask_CheckAttackRange::UBTTask_CheckAttackRange(const FObjectInitializer& ObjectInitializer)
{
    bCreateNodeInstance = true;
    NodeName = "Check Attack Range";
}

EBTNodeResult::Type UBTTask_CheckAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	APawn* OwningPawn = AIController->GetPawn();
	UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));

	if (!AIController || !OwningPawn || !BBComponent || !TargetActor) return EBTNodeResult::Failed;

	float Min_AttackRange = AIController->GetMinAttackRange();
	float Max_AttackRange = AIController->GetMaxAttackRange();

	float DistanceBetween = FMath::Abs(UKismetMathLibrary::Distance2D(FVector2D(TargetActor->GetActorLocation()), FVector2D(OwningPawn->GetActorLocation())));

	if (DistanceBetween > Max_AttackRange || DistanceBetween < Min_AttackRange)
	{
		BBComponent->SetValueAsBool("InAttackRange", false);
	}
	else
	{
		BBComponent->SetValueAsBool("InAttackRange", true);
	}

	return EBTNodeResult::Succeeded;
}
