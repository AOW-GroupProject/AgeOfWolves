// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BaseCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override; 
	//End UActorComponent Interface
#pragma endregion

#pragma region Move Speed
protected:
	UFUNCTION()
		void MoveSpeedChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);

public:
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
		void ChangeMoveSpeed(float InOldVal, float InNewVal);

private:
	bool IsSprinting;
private:
	const float WalkAcceleration = 500.f;
	const float SprintAcceleration = 700.f;
#pragma endregion

};
