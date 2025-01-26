#include "ANS_AllowChainAction.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogANS_AllowChainAction)

UANS_AllowChainAction::UANS_AllowChainAction()
{
    BaseASCRef.Reset();
}

void UANS_AllowChainAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    auto Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 Owner");
        return;
    }

    auto Character = Cast<ACharacterBase>(Owner);
    if (!Character)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 소유자가 CharacterBase가 아님");
        return;
    }

    auto BaseASC = Cast<UBaseAbilitySystemComponent>(Character->GetAbilitySystemComponent());
    if (!BaseASC)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - BaseASC가 아님");
        return;
    }

    auto AnimatingGA = Cast<UBaseGameplayAbility>(BaseASC->GetAnimatingAbility());
    if (!AnimatingGA)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 실행 중인 어빌리티가 없음");
        return;
    }

    if (auto AbilityTag = AnimatingGA->GetAbilityTag(); AbilityTag.IsValid())
    {
        if (!TagToChain.IsValid())
        {
            UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 체인 액션 태그가 설정되지 않음");
            return;
        }

        BaseASCRef = BaseASC;
        BaseASCRef->StartChainWindowWithTag(AbilityTag, TagToChain);

        UE_LOGFMT(LogANS_AllowChainAction, Log, "체인 윈도우 시작 성공 - 어빌리티: {0} | 체인 액션 태그: {1}",
            *AnimatingGA->GetName(),
            *TagToChain.ToString());
    }
    else
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 어빌리티 태그");
    }
}

void UANS_AllowChainAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@Base ASC Ref
    if (!BaseASCRef.IsValid())
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 종료 실패 - 유효하지 않은 AbilitySystemComponent 레퍼런스");
        return;
    }

    UE_LOGFMT(LogANS_AllowChainAction, Log, "체인 윈도우 종료");

    //@Chain Window 종료
    BaseASCRef->EndChainWindow();

    //@Reset
    BaseASCRef.Reset();
}