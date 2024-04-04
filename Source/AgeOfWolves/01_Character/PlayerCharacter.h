// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"

#include "PlayerCharacter.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(LogPlayer, Log, All)

class UCurveFloat;

UCLASS()
class AGEOFWOLVES_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
#pragma region Default Member Function

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	//~AActor interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~End Of AActor interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	//~End Of APawn interface

	//~APawn Interface.
	virtual void PawnClientRestart() override;
	//~ End Of APawn Interface.


#pragma endregion

#pragma region Control Rotaion

protected:
	// ~Custom
	void AdjustControllerRotation(float DeltaSeconds);

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement|Control Rotation")
		UCurveFloat* DirectionCurve;
	// ~End Of Custom

#pragma endregion

#pragma region Default Components

protected:
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

#pragma endregion

};
