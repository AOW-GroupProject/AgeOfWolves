#include "AR_CheckIsSprinting.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsSprinting)


bool UAR_CheckIsSprinting::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsSprinting, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsSprinting, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float MoveSpeed = AttributeSet->GetMoveSpeed();
    const bool bResult = (MoveSpeed >= 650.f);

    UE_LOGFMT(LogCheckIsSprinting, Log, "현재 Move Speed: {0}, 체크 결과: {1}", MoveSpeed, bResult);
    return bResult;
}
