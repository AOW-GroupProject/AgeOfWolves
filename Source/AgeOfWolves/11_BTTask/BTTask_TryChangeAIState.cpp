#include "BTTask_TryChangeAIState.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"

DEFINE_LOG_CATEGORY(LogBTTask_TryChangeAIState)


UBTTask_TryChangeAIState::UBTTask_TryChangeAIState(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = TEXT("AI 상태 변경");
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
		break;
	}

	return EBTNodeResult::Succeeded;
}

FString UBTTask_TryChangeAIState::GetStaticDescription() const
{
	FString StateName;
	switch (DesireState)
	{
	case EAIState::Idle:
		StateName = TEXT("비전투");
		break;
	case EAIState::Attack:
		StateName = TEXT("전투");
		break;
	case EAIState::Investigate:
		StateName = TEXT("순찰");
		break;
	case EAIState::CoverFire:
		StateName = TEXT("전투 지원");
		break;
	default:
		StateName = TEXT("알 수 없음");
		break;
	}

	return FString::Printf(TEXT("AI 상태 변경: %s"), *StateName);
}