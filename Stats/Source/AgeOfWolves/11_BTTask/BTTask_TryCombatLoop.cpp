#include "BTTask_TryCombatLoop.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "AbilitySystemComponent.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "GameplayTagContainer.h"

DEFINE_LOG_CATEGORY(LogBTTask_TryCombatLoop)

UBTTask_TryCombatLoop::UBTTask_TryCombatLoop(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Running Combat Loop";
}

EBTNodeResult::Type UBTTask_TryCombatLoop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //@Base AI Controller
    ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        UE_LOGFMT(LogBTTask_TryCombatLoop, Warning, "실행 실패 - 사유: AI 컨트롤러가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@델리게이트 바인딩 체크
    if (!AIController->RequestStartCombatPattern.IsBound())
    {
        UE_LOGFMT(LogBTTask_TryCombatLoop, Warning, "실행 실패 - 사유: 전투 패턴 시작 요청 이벤트가 바인딩되지 않음");
        return EBTNodeResult::Failed;
    }

    //@전투 패턴 시작 요청
    if (!AIController->RequestStartCombatPattern.Execute())
    {
        UE_LOGFMT(LogBTTask_TryCombatLoop, Warning, "실행 실패 - 사유: 전투 패턴 시작 요청 실패");
        return EBTNodeResult::Failed;
    }

    UE_LOGFMT(LogBTTask_TryCombatLoop, Log, "전투 패턴 시작 요청 성공");
    return EBTNodeResult::Succeeded;
}