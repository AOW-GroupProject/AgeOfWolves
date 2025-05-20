#include "BTTask_StopCombatLoop.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "AbilitySystemComponent.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "GameplayTagContainer.h"

DEFINE_LOG_CATEGORY(LogBTTask_StopCombatLoop)

UBTTask_StopCombatLoop::UBTTask_StopCombatLoop(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsWaitingForExitBlock(false)
{
    bCreateNodeInstance = true;
    NodeName = "Stop Combat Loop";
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_StopCombatLoop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //@Base AI Controller
    ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        UE_LOGFMT(LogBTTask_StopCombatLoop, Warning, "실행 실패 - 사유: AI 컨트롤러가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@델리게이트 바인딩 체크
    if (!AIController->RequestEndCombatPattern.IsBound())
    {
        UE_LOGFMT(LogBTTask_StopCombatLoop, Warning, "실행 실패 - 사유: 전투 패턴 종료 요청 이벤트가 바인딩되지 않음");
        return EBTNodeResult::Failed;
    }

    //@전투 패턴 종료 요청
    const bool bNeedWaitExitBlock = AIController->RequestEndCombatPattern.Execute();

    //@종료 즉시 완료되는 경우
    if (!bNeedWaitExitBlock)
    {
        UE_LOGFMT(LogBTTask_StopCombatLoop, Log, "전투 패턴 종료 요청 성공. 즉시 완료");
        return EBTNodeResult::Succeeded;
    }

    //@종료 완료 콜백 바인딩
    AIController->NotifyCombatPatternExitComplete.BindLambda([this, &OwnerComp]() -> bool
        {
            OnCombatPatternExitCompleteNotified(&OwnerComp);
            return true;
        });

    //@대기 상태로 전환
    bIsWaitingForExitBlock = true;

    UE_LOGFMT(LogBTTask_StopCombatLoop, Log, "전투 패턴 종료 요청 성공. Exit Block 완료 대기 중...");
    return EBTNodeResult::InProgress;
}

void UBTTask_StopCombatLoop::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    //@대기 중이 아니면 틱 처리하지 않음
    if (!bIsWaitingForExitBlock)
    {
        return;
    }
}

void UBTTask_StopCombatLoop::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

    //@AI Controller
    ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (AIController)
    {
        //@바인딩 해제
        AIController->NotifyCombatPatternExitComplete.Unbind();
    }
}

void UBTTask_StopCombatLoop::OnCombatPatternExitCompleteNotified(UBehaviorTreeComponent* OwnerComp)
{
    if (!OwnerComp)
    {
        return;
    }

    //@대기 상태 해제
    bIsWaitingForExitBlock = false;

    UE_LOGFMT(LogBTTask_StopCombatLoop, Log, "Exit Block 실행 완료. Task 종료");

    //@Task 완료
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}