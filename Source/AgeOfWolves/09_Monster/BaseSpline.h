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

	// @����: Spline�� ����Ʈ�� ������ �����Ѵ�.
	int SplinePointCount;

	// @����: ���� ��ġ�� Spline�� ��ġ �ε���
	int SplineIndex = 0;



public:
	class USplineComponent* GetSplineComponent()
	{
		return SplineComponent;
	}
	/// <summary>
	/// MoveTo�� ������ �̵��� ���� �ε��� ī��Ʈ ����
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void IncrementSplineIndex();

	/// <summary>
	/// ���� SplineIndex�� World Position ��ȯ
	/// </summary>
	/// <returns> SplineIndex�� World Position </returns>
	UFUNCTION()
	FVector GetSplinePointWorldPosition();
#pragma endregion

};
