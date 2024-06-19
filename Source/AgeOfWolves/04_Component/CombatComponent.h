// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
	int GetInputCount();

	UFUNCTION(BlueprintCallable)
	void IncrementInputCount();


	UFUNCTION(BlueprintCallable)
	void DecrementInputCount();

	UFUNCTION(BlueprintCallable)
	void ResetInputCount();

	UFUNCTION(BlueprintCallable)
	bool CanComboAttack();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;



	
private:
	int InputCount = 0;

public :
	

};
