// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Monster/BaseMonster.h"
#include "MonsterInterface_Spline.h"

#include "BaseMonster_Spline.generated.h"


/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API ABaseMonster_Spline : public ABaseMonster, public IMonsterInterface_Spline
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	ABaseMonster_Spline();

protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Interface
public:
	/// <summary>
	/// 목표로 다가감
	/// </summary>
	/// <returns>가까우면 true 반환</returns>
	virtual bool WhenMoveToSplinePointReturnTrue() override;
#pragma endregion

#pragma region Spline
public:

	class USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBlueprint* SplineBP;
	class ABaseSpline* SplineActor;

#pragma endregion
	
};
