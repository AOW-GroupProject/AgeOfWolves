// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"

#include "ExecCalc_DamageToHealth.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExecCalc_DamageToHealth, Log, All);

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
 *	@UExecCalc_DamageToHealth
 *	
 *	Damage를 Health에 적용하는 Execution Calculation
 */
UCLASS()
class AGEOFWOLVES_API UExecCalc_DamageToHealth : public UGameplayEffectExecutionCalculation
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UExecCalc_DamageToHealth();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
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
