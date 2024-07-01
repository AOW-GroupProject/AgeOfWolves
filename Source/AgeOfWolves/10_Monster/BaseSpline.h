// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/KismetMathLibrary.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSpline.generated.h"

//BaseMonster는 접근할 수 있지만, 이 클래스에서는 몬스터에 직접 접근하는 일이 없도록
UCLASS()
class AGEOFWOLVES_API ABaseSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Spline
private:

	UPROPERTY(EditDefaultsOnly)
	class USplineComponent* SplineComponent;




public:
	// @설명: 현재 몬스터가 위치한 Spline의 인덱스. 또한 patrol을 시작할 인덱스로 사용되며, 이는 프로퍼티 창에서 편집.
	UPROPERTY(EditAnywhere)
	int SplineIndex = 0;

	// @설명: 소환할 몬스터의 클래스, 레벨에서 수정할 수 있도록 함
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseMonster_Spline> MonsterClass;
	
	UPROPERTY(BlueprintReadOnly)
	//현재 스플라인에 있는 몬스터.
	TObjectPtr<class ABaseMonster_Spline> MyMonster; 

	/*
	* @목적 : 몬스터 소환.
	* @설명 : 현재 몬스터가 empty라면 몬스터를 SplindIndex 위치에 소환한다.
	* @참조 : -
	*/
	void SpawnMonster();


	FORCEINLINE class USplineComponent* GetSplineComponent()
	{
		return SplineComponent;
	}
	
#pragma endregion

	
#pragma region Distance에 따른 Tangent(사용안함)


	/*
	* @목적 : (AI)MoveTo를 사용하여 이동방식을 구현할 때 사용할 함수.
	* @설명 : SplineIndex + 1.
	* @참조 : -
	*/
	UFUNCTION(BlueprintCallable)
	void IncreaseSplineIndex();

	/*
	* @목적 : -
	* @설명 : 현재 SplineIndex의 World Position 반환
	* @참조 : -
	*/
	UFUNCTION()
	FVector GetSplinePointWorldPosition();

	/*
	* @목적 : 인덱스 a와 인덱스 b사이의 거리를 구하는 함수.
	* @설명 : GetDistanceAlong~을 사용하여 차이를 반환.
	* @참조 : -
	*/
	//UFUNCTION()
	//int CalculateDistanceBetweenIndex(int a, int b);
	

#pragma endregion


};
