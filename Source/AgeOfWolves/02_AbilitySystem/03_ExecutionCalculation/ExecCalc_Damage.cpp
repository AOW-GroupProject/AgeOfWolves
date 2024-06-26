// Fill out your copyright notice in the Description page of Project Settings.


#include "02_AbilitySystem/03_ExecutionCalculation/ExecCalc_Damage.h"


struct AOWDamageStatics
{
	AOWDamageStatics()
	{

	}
};

static const AOWDamageStatics& DamageStatics()
{
	static AOWDamageStatics DStatics;
	return DStatics;
}




UExecCalc_Damage::UExecCalc_Damage()
{
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
}
