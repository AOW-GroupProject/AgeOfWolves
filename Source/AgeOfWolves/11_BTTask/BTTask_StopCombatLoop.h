//BTTask_StopCombatLoop.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopCombatLoop.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_StopCombatLoop, Log, All);

/**
 * @class UBTTask_StopCombatLoop
 * @brief 전투 패턴을 종료하는 Task입니다.
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_StopCombatLoop : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_StopCombatLoop(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
    //@Exit Block 완료 콜백
    void OnCombatPatternExitCompleteNotified(UBehaviorTreeComponent* OwnerComp);

protected:
    //@Task 완료 대기 상태
    bool bIsWaitingForExitBlock;
};