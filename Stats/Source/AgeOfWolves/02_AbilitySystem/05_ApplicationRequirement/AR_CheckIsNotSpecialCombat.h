#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsNotSpecialCombat.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsNotSpecialCombat, Log, All);

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAR_CheckIsNotSpecialCombat
 * 
 *	현재 캐릭터의 전투 모드가 '2'가 아닌지 체크하는 AR
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsNotSpecialCombat : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
	
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
};
