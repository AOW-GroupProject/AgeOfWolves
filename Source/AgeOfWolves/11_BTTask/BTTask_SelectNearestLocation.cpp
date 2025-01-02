#include "BTTask_SelectNearestLocation.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "AIController.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogBTTask_SelectNearestLocation)

UBTTask_SelectNearestLocation::UBTTask_SelectNearestLocation()
{
    bCreateNodeInstance = true;
    NodeName = "Select Nearest Location";
}

EBTNodeResult::Type UBTTask_SelectNearestLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIControllerRef = Cast<AAIController>(OwnerComp.GetAIOwner());
	ACharacterBase* PawnObject = Cast<ACharacterBase>(AIControllerRef->GetPawn());

	if (!AIControllerRef || !PawnObject || !LocationSeekerQuery) return EBTNodeResult::Failed;

	LocationSeekerQueryRequest = FEnvQueryRequest(LocationSeekerQuery, PawnObject);
	LocationSeekerQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UBTTask_SelectNearestLocation::EnemySeekerQueryFinished);

	return EBTNodeResult::Succeeded;
}

void UBTTask_SelectNearestLocation::EnemySeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	uint8 Index = 0;
	float CurrentBestScore = -1.f;
	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);

	for (uint8 i = 0; i < Locations.Num(); i++)
	{
		if (Result->GetItemScore(i) > CurrentBestScore)
		{
			NearestLocation = Locations[i];
			CurrentBestScore = Result->GetItemScore(i);
		}
	}

	AIControllerRef->GetBlackboardComponent()->SetValueAsVector("MoveToLocation", NearestLocation);
}
