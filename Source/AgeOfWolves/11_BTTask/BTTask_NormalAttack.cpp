// BTTask_NormalAttack.cpp
#include "BTTask_NormalAttack.h"
#include "Logging/StructuredLog.h"
#include "10_AI/BaseAIController.h"
#include "AbilitySystemComponent.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "GameplayTagContainer.h"

DEFINE_LOG_CATEGORY(LogBTTask_NormalAttack)

UBTTask_NormalAttack::UBTTask_NormalAttack(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bCreateNodeInstance = true;
    NodeName = "Normal Attack";
}

EBTNodeResult::Type UBTTask_NormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //@Base AI Controller
    ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        UE_LOGFMT(LogBTTask_NormalAttack, Warning, "실행 실패 - 사유: AI 컨트롤러가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@ASC
    UAbilitySystemComponent* ASC = AIController->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogBTTask_NormalAttack, Warning, "실행 실패 - 사유: ASC가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@Tag
    if (!AbilityTag.IsValid())
    {
        UE_LOGFMT(LogBTTask_NormalAttack, Warning, "실행 실패 - 사유: AbilityTag가 유효하지 않음");
        return EBTNodeResult::Failed;
    }

    //@어빌리티 활성화 시도
    FGameplayTagContainer SourceTags;
    FGameplayTagContainer TargetTags;

    if (!ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag), true))
    {
        UE_LOGFMT(LogBTTask_NormalAttack, Warning, "실행 실패 - 사유: 어빌리티 활성화 실패 (Tag: {0})",
            AbilityTag.ToString());
        return EBTNodeResult::Failed;
    }

    UE_LOGFMT(LogBTTask_NormalAttack, Log, "어빌리티 실행 성공 - Tag: {0}", AbilityTag.ToString());
    return EBTNodeResult::Succeeded;
}