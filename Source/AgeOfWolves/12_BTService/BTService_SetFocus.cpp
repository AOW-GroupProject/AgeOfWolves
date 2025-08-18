#include "BTService_SetFocus.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY(LogBTService_SetFocus)

void UBTService_SetFocus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComponent = AIController ? AIController->GetBlackboardComponent() : nullptr;

	if (!AIController || !BBComponent)
	{
		UE_LOGFMT(LogBTService_SetFocus, Warning, "SetFocus 실패: 유효하지 않은 AIController 또는 BlackboardComponent");
		return;
	}

	if (AIController->GetBrainComponent()->IsPaused())
	{
		AIController->ClearFocus(EAIFocusPriority::LastFocusPriority);
		UE_LOGFMT(LogBTService_SetFocus, Log, "Brain 일시정지 상태 - Focus 해제");
	}
	else
	{
		AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
		if (!!TargetActor)
		{
			AIController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
			UE_LOGFMT(LogBTService_SetFocus, Log, "타겟 Focus 설정: {0}",
				TargetActor ? *TargetActor->GetName() : TEXT("None"));
		}
		else
		{
			AIController->ClearFocus(EAIFocusPriority::LastFocusPriority);
			UE_LOGFMT(LogBTService_SetFocus, Log, "Brain 일시정지 상태 - Focus 해제");
		}
	}

	UpdateAgentCombatState(OwnerComp);
}

void UBTService_SetFocus::UpdateAgentCombatState(UBehaviorTreeComponent& OwnerComp)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BBComponent = AIController ? AIController->GetBlackboardComponent() : nullptr;

	if (!AIController || !BBComponent)
	{
		UE_LOGFMT(LogBTService_SetFocus, Warning, "UpdateAgentCombatState 실패: 유효하지 않은 BaseAIController 또는 BlackboardComponent");
		return;
	}

	EAIState AIState = static_cast<EAIState>(BBComponent->GetValueAsEnum("AIState"));

	if ((uint8)AIState <= -1)
	{
		return;
	}

	AIController->ChangeAgentAIState(AIState);

	UE_LOGFMT(LogBTService_SetFocus, Log, "AI 상태 업데이트: {0}",
		*UEnum::GetValueAsString(AIState));
}