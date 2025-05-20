#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"

#include "ExecCalc_Groggy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExecCalc_Groggy, Log, All);

/**
 *	@UExecCalc_Groggy
 * 
 *	Source의 Groggy 수치를 Target에게 전달하는 Exec Calc 입니다.
 */
UCLASS()
class AGEOFWOLVES_API UExecCalc_Groggy : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Groggy();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
