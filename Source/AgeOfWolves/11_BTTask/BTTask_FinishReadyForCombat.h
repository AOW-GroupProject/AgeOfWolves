#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_FinishReadyForCombat.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_FinishReadyForCombat, Log, All)

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
 *	@UBTTask_FinishReadyForCombat
 * 
 *	AI의 전투 준비 종료 동작을 정의하는 BT Task
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_FinishReadyForCombat : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FinishReadyForCombat(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
