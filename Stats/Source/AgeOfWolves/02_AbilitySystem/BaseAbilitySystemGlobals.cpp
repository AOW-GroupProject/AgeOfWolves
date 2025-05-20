// Fill out your copyright notice in the Description page of Project Settings.


#include "02_AbilitySystem/BaseAbilitySystemGlobals.h"
#include "02_AbilitySystem/BaseGameplayEffectContext.h"

FGameplayEffectContext* UBaseAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FBaseGameplayEffectContext();
}
