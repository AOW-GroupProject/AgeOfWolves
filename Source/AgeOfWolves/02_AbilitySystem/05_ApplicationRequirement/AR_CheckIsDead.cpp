
#include "AR_CheckIsDead.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsDead)

//@Defualt Setting
#pragma region Default Setting
bool UAR_CheckIsDead::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsDead, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsDead, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float Health = AttributeSet->GetHealth();
    const bool bResult = (Health <= 0.0f);

    UE_LOGFMT(LogCheckIsDead, Log, "현재 Health: {0}, 체크 결과: {1}", Health, bResult);
    return bResult;
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion