#include "AR_CheckIsGuardCombat.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsGuardCombat)

bool UAR_CheckIsGuardCombat::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsGuardCombat, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsGuardCombat, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float CombatState = AttributeSet->GetCombatState();
    const bool bResult = (CombatState >= 3.0f);

    UE_LOGFMT(LogCheckIsGuardCombat, Log, "현재 CombatState: {0}, 체크 결과: {1}", CombatState, bResult);
    return bResult;
}
