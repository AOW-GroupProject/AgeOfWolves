#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"

#include "BTService_SelectNearestLocation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTService_SelectNearestLocation, Log, All)

//@전방 선언
#pragma region Forward Declaration
class AAIController;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UBTService_SelectNearestLocation
 * 
 *	최단 거리 찾기를 지원하는 BT Service
 */
UCLASS()
class AGEOFWOLVES_API UBTService_SelectNearestLocation : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@Env Query
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	UEnvQuery* LocationSeekerQuery;

	FEnvQueryRequest LocationSeekerQueryRequest;

	FVector NearestLocation = FVector::ZeroVector;
#pragma endregion

	//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
	void EnemySeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	TObjectPtr<AAIController> AIControllerRef;
#pragma endregion
};
