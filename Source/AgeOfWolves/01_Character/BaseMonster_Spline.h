// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/BaseMonster.h"
#include "10_Monster/MonsterInterface_Spline.h"

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

	virtual void Tick(float DeltaTime) override;

	/*
	* @목적 : Rotation을 관리하는 함수입니다.
	* @설명 : MakeRotFromX..등을 사용하여 관리합니다. 추후에 경사면이 생길 경우 수정하여 테스트 해야 함.
	* @참조 : -
	*/
	virtual void ControllRotation() override;

#pragma endregion

#pragma region Interface
public:
	/*
	* @목적 : Spline 특정 포인트로 움직이는 함수입니다.
	* @설명 : SimpleMoveTo로 spline의 포인트로 움직였다면 true를 반환합니다.
	* @참조 : -
	*/
	UFUNCTION(BlueprintCallable)
	virtual bool WhenMoveToSplinePointReturnTrue() override;

	FRotator Rotation;

	/*
	* @목적 : Spline에 맞춰 액터를 움직이는 함수입니다.
	* @설명 : Tick으로 작동하며, FindTangentClosestToWorldLocation 함수를 사용하여 벡터를 구하고 Velocity를 적용시킵니다.
	* @참조 : -
	*/
	UFUNCTION(BlueprintCallable)
	virtual void MoveAlongSplinePoint(float delta) override;

#pragma endregion

#pragma region Spline
public:

	
	class USplineComponent* SplineComponent;

	class ABaseSpline* SplineActor;

	/*
	* @목적 : 초기화 함수.
	* @설명 : Spawn되기 전 BaseSpline에서 SpawnActorDeffered를 사용하여 몬스터가 움직일 spline 정보를 넘겨 받는다.
	* @참조 : -
	*/
	void SetSplineActor(ABaseSpline* spline);
	

	
#pragma region Distance에 따른 Tangent


	// @설명: 현재 인덱스와 다음 인덱스 사이의 Distance거리 저장.
	UPROPERTY(EditAnywhere)
	float SplineIndexDistance;

	// @설명: 인덱스 사이 거리가 아닌 총 진행 거리 저장.
	UPROPERTY(EditAnywhere)
	float Distance = 0;

	// @설명: Spline 진행 시간
	UPROPERTY(EditAnywhere)
	float MaxTime = 10;

	float CurrentTime = 0;

	// @설명: Spline 이동 속도
	float Speed = 200;

	// @설명: SplineIndexDistance거리의 진행상태를 조정하기 위해
	float BetweenDistance = 0;
#pragma endregion

#pragma endregion
	
};
