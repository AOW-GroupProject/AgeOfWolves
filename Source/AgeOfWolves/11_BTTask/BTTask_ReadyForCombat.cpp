#include "BTTask_ReadyForCombat.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"

DEFINE_LOG_CATEGORY(LogBTTask_ReadyForCombat)

UBTTask_ReadyForCombat::UBTTask_ReadyForCombat(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;

	NodeName = TEXT("전투 준비 동작 수행");
}

EBTNodeResult::Type UBTTask_ReadyForCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //@Base AI Controller
    ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        UE_LOGFMT(LogBTTask_ReadyForCombat, Warning, "실행 실패 - 사유: AI 컨트롤러가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@델리게이트 바인딩 체크
    if (!AIController->RequestReadyToCombat.IsBound())
    {
        UE_LOGFMT(LogBTTask_ReadyForCombat, Warning, "실행 실패 - 사유: 전투 패턴 시작 요청 이벤트가 바인딩되지 않음");
        return EBTNodeResult::Failed;
    }

    //@전투 패턴 시작 요청
    if (!AIController->RequestReadyToCombat.Execute())
    {
        UE_LOGFMT(LogBTTask_ReadyForCombat, Warning, "실행 실패 - 사유: 전투 패턴 시작 요청 실패");
        return EBTNodeResult::Failed;
    }

    UE_LOGFMT(LogBTTask_ReadyForCombat, Log, "전투 준비 동작 요청 수행 완료");
    return EBTNodeResult::Succeeded;
}
