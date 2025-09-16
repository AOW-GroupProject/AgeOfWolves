#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsBattouCombat.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsBattouCombat, Log, All);

/**
 *	@UAR_CheckIsBattouCombat
 * 
 *	현재 전투 자세가 발도 전투 자세인지 체크합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsBattouCombat : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
};
