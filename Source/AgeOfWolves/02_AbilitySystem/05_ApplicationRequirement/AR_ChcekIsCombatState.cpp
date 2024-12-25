// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ChcekIsCombatState.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsCombatState)

bool UAR_ChcekIsCombatState::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsCombatState, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsCombatState, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float CombatState = AttributeSet->GetCombatState();
    const bool bResult = (CombatState >= 1.0f);

    UE_LOGFMT(LogCheckIsCombatState, Log, "현재 CombatState: {0}, 체크 결과: {1}", CombatState, bResult);
    return bResult;
}