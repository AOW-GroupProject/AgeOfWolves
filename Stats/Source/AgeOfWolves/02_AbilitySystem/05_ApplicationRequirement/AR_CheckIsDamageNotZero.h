#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"

#include "AR_CheckIsDamageNotZero.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheckIsDamageNotZero, Log, All);

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
 *	@UAR_CheckIsDamageNotZero
 * 
 *	Damage 어트리뷰트가 1이상인지 체크하는 AR
 */
UCLASS()
class AGEOFWOLVES_API UAR_CheckIsDamageNotZero : public UGameplayEffectCustomApplicationRequirement
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec,
		UAbilitySystemComponent* Source) const override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
