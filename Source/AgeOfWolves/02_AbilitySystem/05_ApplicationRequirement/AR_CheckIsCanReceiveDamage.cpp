// Fill out your copyright notice in the Description page of Project Settings.

#include "AR_CheckIsCanReceiveDamage.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCheckIsCanReceiveDamage)

bool UAR_CheckIsCanReceiveDamage::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* Source) const
{
    if (!Source)
    {
        UE_LOGFMT(LogCheckIsCanReceiveDamage, Error, "Source가 유효하지 않습니다");
        return false;
    }

    const UBaseAttributeSet* AttributeSet = Source->GetSet<UBaseAttributeSet>();
    if (!AttributeSet)
    {
        UE_LOGFMT(LogCheckIsCanReceiveDamage, Error, "AttributeSet이 유효하지 않습니다");
        return false;
    }

    const float Health = AttributeSet->GetHealth();
    const bool bResult = (Health >= 1.0f);

    UE_LOGFMT(LogCheckIsCanReceiveDamage, Log, "현재 Health: {0}, 체크 결과: {1}", Health, bResult);
    return bResult;
}