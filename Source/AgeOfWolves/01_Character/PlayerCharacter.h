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
	
#pragma region Default Setting
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
	virtual void PawnClientRestart() override;
	//~End Of APawn Interface.

	//~Components
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
	//~End Of Components
#pragma endregion

#pragma region Controller Rotaion
protected:
	/*
	* @목적 : 캐릭터 객체의 회전 값을 Controller의 회전 값에 따른 보간 작업을 수행합니다.
	* @설명 : 사용자 카메라가 바라보는 방향과 캐릭터 객체가 바라보는 방향의 조정 작업을 수행합니다.
	* @참조 : -
	*/ 
	void AdjustControllerRotation(float DeltaSeconds);

	/*
	* @목적 : 사용자의 카메라 회전에 따른 캐릭터 객체의 회전 보간 작업
	* @설명 : Float Curve를 통해 카메라(Controller) 회전 값과 캐릭터 이동 방향의 회전 값
	* @참조 : APlayerCharacter::AdjustControllerRotation
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement|Control Rotation")
		UCurveFloat* DirectionCurve;
#pragma endregion

};
