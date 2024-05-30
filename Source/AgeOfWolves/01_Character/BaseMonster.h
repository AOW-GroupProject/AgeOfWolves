// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonster.generated.h"

UENUM()
enum class EMonsterState : uint8
{
	Patrol UMETA(DisplayName = "patrol"),
	Attack UMETA(DisplayName = "attack"),
	DetectingPlayer UMETA(DisplayName = "patrol") //플레이어 감지 중
};

UCLASS()
class AGEOFWOLVES_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ControllRotation();

	//현재 상태 저장
	EMonsterState CurrentState;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
