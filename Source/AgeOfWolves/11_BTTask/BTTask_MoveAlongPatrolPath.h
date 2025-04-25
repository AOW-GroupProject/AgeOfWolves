#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_MoveAlongPatrolPath.generated.h"

//@전방 선언
#pragma region Forward Declaration
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
* @UBTTask_MoveAlongPatrolPath 
*
* AI가 속한 그룹의 Spline 기반 순찰 경로를 따라 이동하는 Task
*/
UCLASS()
class AGEOFWOLVES_API UBTTask_MoveAlongPatrolPath : public UBTTask_BlackboardBase
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBTTask_MoveAlongPatrolPath();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@현재 순찰 포인트 인덱스
	int32 CurrentPointIndex = 0;

	//@순찰 방식 (순차적(FALSE) 또는 랜덤(TRUE))
	UPROPERTY(EditAnywhere, Category = "Patrol")
	bool bRandomPatrol = false;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
};