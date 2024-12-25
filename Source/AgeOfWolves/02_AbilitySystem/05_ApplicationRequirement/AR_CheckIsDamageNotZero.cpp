#include "AR_CheckIsDamageNotZero.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsDamageNotZero)

bool UAR_CheckIsDamageNotZero::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsDamageNotZero, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsDamageNotZero, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float Damage = AttributeSet->GetDamage();
    const bool bResult = (Damage > 0.0f);

    UE_LOGFMT(LogCheckIsDamageNotZero, Log, "현재 Damage: {0}, 체크 결과: {1}", Damage, bResult);
    return bResult;
}
