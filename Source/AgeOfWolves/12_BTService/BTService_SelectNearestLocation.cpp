#include "BTService_SelectNearestLocation.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "AIController.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogBTService_SelectNearestLocation)

void UBTService_SelectNearestLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AIControllerRef = Cast<AAIController>(OwnerComp.GetAIOwner());
	ACharacterBase* PawnObject = Cast<ACharacterBase>(AIControllerRef->GetPawn());

	if (!AIControllerRef || !PawnObject || !LocationSeekerQuery) return;

	LocationSeekerQueryRequest = FEnvQueryRequest(LocationSeekerQuery, PawnObject);
	LocationSeekerQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UBTService_SelectNearestLocation::EnemySeekerQueryFinished);

}

void UBTService_SelectNearestLocation::EnemySeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result)
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