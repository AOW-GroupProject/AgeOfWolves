#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsSprinting.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsSprinting, Log, All);

/**
 *	@UAR_CheckIsSprinting
 * 
 *	현재 캐릭터가 달리고 있는지 체크하는 AR을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsSprinting : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
};
