﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/*
* @목적 : Damage를 계산하는 Execution Calculation Class
* @설명 : MMC (Modifier Magnitude Calculation)와 공통점 : Attribute들을 캡처하여 사용한다. *차이점 : 둘 이상의 Attribute를 변화시킬 수 있다. 
* @주의 : C++로만 구현가능하다. Instant 또는 Periodic GameplayEffect만 함께 사용가능하다.
*/
// Uses macros from AttributeSet.h
UCLASS()
class AGEOFWOLVES_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()


public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
