#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsSpecialCombatState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsSpecialCombatState, Log, All);

/**
 *	@UAR_CheckIsSpecialCombatState
 * 
 *	캐릭터가 현재 발도 모드인지 확인합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsSpecialCombatState : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
};
