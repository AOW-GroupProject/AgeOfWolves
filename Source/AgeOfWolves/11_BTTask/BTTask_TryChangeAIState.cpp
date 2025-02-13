#include "BTTask_TryChangeAIState.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"

DEFINE_LOG_CATEGORY(LogBTTask_TryChangeAIState)


UBTTask_TryChangeAIState::UBTTask_TryChangeAIState(const FObjectInitializer& ObjectInitializer)
{
    bCreateNodeInstance = true;
    NodeName = "AIState Override";
}

EBTNodeResult::Type UBTTask_TryChangeAIState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* Controller = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComponent = Controller->GetBlackboardComponent();
	
	if (Controller == nullptr || BBComponent == nullptr) return EBTNodeResult::Failed;

	// #0 Target Actor + Target Location 
	AActor* TargetActor;
	FVector TargetLocation;

	switch (RunMode)
	{
	case ERunMode::E_ThisAgent:
		BBComponent->SetValue<UBlackboardKeyType_Enum>("AIState", (uint8)DesireState);
		break;
	case ERunMode::E_AllAgent:
		TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector("MoveToLocation");

		//@TODO: 단체 활동 지시
		//Controller->GetAIManager()->NotifyAIState(DesireState, TargetActor, TargetLocation);
		break;
	}

	return EBTNodeResult::Succeeded;
}
