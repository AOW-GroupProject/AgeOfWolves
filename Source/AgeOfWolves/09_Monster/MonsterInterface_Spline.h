// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterInterface_Spline.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMonsterInterface_Spline : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGEOFWOLVES_API IMonsterInterface_Spline
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UFUNCTION(BlueprintCallable)
	//virtual class USplineComponent* GetSplineComponent() {}

	/// <summary>
	/// 목표로 다가가면 true 반환
	/// </summary>
	/// <returns></returns>
	virtual bool WhenMoveToSplinePointReturnTrue() = 0;

	
};
