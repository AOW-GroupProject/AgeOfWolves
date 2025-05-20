#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsNonCombatState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsNonCombatState, Log, All);

/**
 *	@UAR_CheckIsNonCombatState
 * 
 *	Combat State가 0이면 참을 반환하는 AR을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsNonCombatState : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
};
