// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSpline.generated.h"

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

	// @설명: Spline의 포인트의 개수를 저장한다.
	int SplinePointCount;

	// @설명: 현재 위치한 Spline의 위치 인덱스
	int SplineIndex = 0;



public:
	class USplineComponent* GetSplineComponent()
	{
		return SplineComponent;
	}
	/// <summary>
	/// MoveTo가 끝나면 이동할 다음 인덱스 카운트 증가
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void IncrementSplineIndex();

	/// <summary>
	/// 현재 SplineIndex의 World Position 반환
	/// </summary>
	/// <returns> SplineIndex의 World Position </returns>
	UFUNCTION()
	FVector GetSplinePointWorldPosition();
#pragma endregion

};
