#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_IsInDanger.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogIsInDanger, Log, All);

/**
*	@UAR_IsInDanger
*
*	캐릭터가 위험 상황에 처해 있는지 확인하는 AR을 정의합니다.
*	AlertState가 1 이상일 경우 캐릭터가 위험한 상황에 놓여있다고 판단합니다.
*/
UCLASS()
class AGEOFWOLVES_API UAR_IsInDanger : public UGameplayEffectCustomApplicationRequirement
{
    GENERATED_BODY()

public:
    virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
        const FGameplayEffectSpec& Spec,
        UAbilitySystemComponent* Source) const override;
};