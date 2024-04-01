// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

UBaseGameplayAbility::UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
}
