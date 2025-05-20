#include "BTService_SetFocus.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY(LogBTService_SetFocus)

void UBTService_SetFocus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();

	if (!AIController || !BBComponent) return;

	if (AIController->GetBrainComponent()->IsPaused())
	{
		AIController->ClearFocus(EAIFocusPriority::LastFocusPriority);
	}
	else
	{
		// #0. AI State
		if ((uint8)BBComponent->GetValueAsEnum("AIState") >= (uint8)EAIState::Attack)
		{
			AIController->ClearFocus(EAIFocusPriority::LastFocusPriority);
		}
		else
		{
			AIController->SetFocus(Cast<AActor>(BBComponent->GetValueAsObject("TargetActor")), EAIFocusPriority::Gameplay);
		}
	}

	// #1. Anim Instance
	UpdateAgentCombatState(OwnerComp);
}

void UBTService_SetFocus::UpdateAgentCombatState(UBehaviorTreeComponent& OwnerComp)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();

	if (!AIController || !BBComponent) return;

	EAIState AIState = static_cast<EAIState>(BBComponent->GetValueAsEnum("AIState"));
	
	if ((uint8)AIState <= -1) return;

	AIController->ChangeAgentAIState(AIState);
}
