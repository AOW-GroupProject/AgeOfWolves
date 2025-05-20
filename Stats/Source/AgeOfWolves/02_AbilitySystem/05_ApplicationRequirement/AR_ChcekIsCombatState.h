#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_ChcekIsCombatState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsCombatState, Log, All);

/**
 *	@UAR_ChcekIsCombatState
 * 
 *	Combat State가 1이면 참을 반환하는 AR을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_ChcekIsCombatState : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec, 
		UAbilitySystemComponent* Source) const override;
};
