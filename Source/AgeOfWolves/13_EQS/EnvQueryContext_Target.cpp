#include "EnvQueryContext_Target.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Target::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AAIController* AIController = Cast<AAIController>((Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController()));

	if (!AIController)
	{
		return;
	}

	AActor* Target = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!Target)
	{
		return;
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, Target);
}
