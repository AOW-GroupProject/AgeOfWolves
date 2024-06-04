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

	//인터페이스: 몬스터마다 움직이는 방식이 다른 경우 Task에서 캐스팅을 전부 다 하지 않기 위해
	
	// 점에서 점으로 움직임
	virtual bool WhenMoveToSplinePointReturnTrue() = 0;

	// 자연스러운 움직임
	virtual void MoveAlongSplinePoint(float delta) = 0;

	
};
