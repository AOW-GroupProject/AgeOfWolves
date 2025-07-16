#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsOnlyCombatState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsOnlyCombatState, Log, All);


/**
 *	@UAR_CheckIsOnlyCombatState
 * 
 *	Combat State가 정확히 1이면 참을 반환하는 AR을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsOnlyCombatState : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()

public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
	
};
