#include "AR_IsInDanger.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogIsInDanger)

bool UAR_IsInDanger::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogIsInDanger, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogIsInDanger, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float AlertState = AttributeSet->GetAlertLevel();
    const bool bResult = (AlertState >= 1.0f);

    UE_LOGFMT(LogIsInDanger, Log, "현재 AlertState: {0}, 위험 상황 여부: {1}", AlertState, bResult);

    return bResult;
}