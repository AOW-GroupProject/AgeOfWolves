#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_TryCombatLoop.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_TryCombatLoop, Log, All)

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
 *	@UBTTask_TryCombatLoop
 *	
 *	AI가 소유하는 Pawn의 전투를 시작합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_TryCombatLoop : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_TryCombatLoop(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
