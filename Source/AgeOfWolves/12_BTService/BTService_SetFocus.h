#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

#include "BTService_SetFocus.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTService_SetFocus, Log, All)

/**
 *	@UBTService_SetFocus
 * 
 *	Blackboard에서 설정한 Target에 대하여 시선을 고정합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBTService_SetFocus : public UBTService_BlackboardBase
{

	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void UpdateAgentCombatState(UBehaviorTreeComponent& OwnerComp);
	
};
