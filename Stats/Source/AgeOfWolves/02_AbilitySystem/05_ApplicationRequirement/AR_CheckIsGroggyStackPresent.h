#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsGroggyStackPresent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsGroggyStackPresent, Log, All);

/**
 *	@UAR_CheckIsGroggyStackPresent
 * 
 *	현재 그로기 수치가 1이상인지 체크하는 AR을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsGroggyStackPresent : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
};
