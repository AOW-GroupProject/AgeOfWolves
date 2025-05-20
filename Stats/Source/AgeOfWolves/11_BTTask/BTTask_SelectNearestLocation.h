#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"

#include "BTTask_SelectNearestLocation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_SelectNearestLocation, Log, All)

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
 *	@UBTTask_SelectNearestLocation
 * 
 *	Env Query를 통해 생긴 좌표들 중 현재 위치와 가장 가까운 Query를 찾습니다.
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_SelectNearestLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

//@친추 클래스
#pragma region Friend Class
#pragma endregion

//@Defualt Setting
#pragma region Default Setting
public:
	UBTTask_SelectNearestLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	void EnemySeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result);

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
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	TObjectPtr<AAIController> AIControllerRef;
#pragma endregion
};
