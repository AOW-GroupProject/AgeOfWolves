#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "BaseAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	//~UAbilitySystemGlobals interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~End of UAbilitySystemGlobals interface
	
};
