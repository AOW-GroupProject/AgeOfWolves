#include "AR_CheckIsGroggyStackPresent.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsGroggyStackPresent)


bool UAR_CheckIsGroggyStackPresent::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsGroggyStackPresent, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsGroggyStackPresent, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float Groggy = AttributeSet->GetGroggy();
    const bool bResult = (Groggy >= 1.0f);

    UE_LOGFMT(LogCheckIsGroggyStackPresent, Log, "현재 Groggy: {0}, 체크 결과: {1}", Groggy, bResult);
    return bResult;
}
