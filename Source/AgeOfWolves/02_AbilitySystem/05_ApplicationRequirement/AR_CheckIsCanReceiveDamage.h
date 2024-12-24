// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsCanReceiveDamage.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsCanReceiveDamage, Log, All);

/**
 *	@UAR_CheckIsCanReceiveDamage
 * 
 *	Damage 전달을 받을 수 있는 상태인지 체크하는 AR
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsCanReceiveDamage : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()

public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
	
};
