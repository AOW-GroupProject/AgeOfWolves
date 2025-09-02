#include "BTTask_FinishReadyForCombat.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"

DEFINE_LOG_CATEGORY(LogBTTask_FinishReadyForCombat)


UBTTask_FinishReadyForCombat::UBTTask_FinishReadyForCombat(const FObjectInitializer& ObjectInitializer)
{
	bCreateNodeInstance = true;

	NodeName = TEXT("전투 준비 동작 종료 수행");
}

EBTNodeResult::Type UBTTask_FinishReadyForCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //@Base AI Controller
    ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        UE_LOGFMT(LogBTTask_FinishReadyForCombat, Warning, "실행 실패 - 사유: AI 컨트롤러가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@델리게이트 바인딩 체크
    if (!AIController->RequestFininshReadyToCombat.IsBound())
    {
        UE_LOGFMT(LogBTTask_FinishReadyForCombat, Warning, "실행 실패 - 사유: 전투 패턴 시작 요청 이벤트가 바인딩되지 않음");
        return EBTNodeResult::Failed;
    }

    //@전투 패턴 시작 요청
    if (!AIController->RequestFininshReadyToCombat.Execute())
    {
        UE_LOGFMT(LogBTTask_FinishReadyForCombat, Warning, "실행 실패 - 사유: 전투 패턴 시작 요청 실패");
        return EBTNodeResult::Failed;
    }

    UE_LOGFMT(LogBTTask_FinishReadyForCombat, Log, "전투 준비 동작 종료 요청 수행 완료");
    return EBTNodeResult::Succeeded;
}
