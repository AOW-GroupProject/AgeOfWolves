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
// @목적 : UI 관련 테스트 진행을 위해 작성한 임시 코드입니다. 실제 UI 구현을 위해선 아래 코드를 삭제하고 시작해주세요.
class UTestWidget;

UCLASS()
class AGEOFWOLVES_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	//~BeginAActor interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// @TODO: LockOn 기능 Refactoring 시 수정할 곳입니다.
	// @설명: 필요한 순간에만 활용합니다. 우선 주석 처리합니다.
	virtual void Tick(float DeltaSeconds) override;
	//~End AActor interface

	//~Begin APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;
	//~End APawn Interface.

	//~Components
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
	//~End Of Components
#pragma endregion

#pragma region Controller Rotaion
protected:
	// TODO : LockOn 기능 구현 시 활용할 함수, 구현만 해놓은 상태로 직접적인 호출은 아직 없습니다.
	/*
	* @목적 : 매 프레임마다 캐릭터 객체의 회전 값에 대하여 Controller의 회전 값에 따른 보간 작업을 수행합니다.
	* @설명 : 사용자 카메라가 바라보는 방향과 캐릭터 객체가 바라보는 방향의 조정 작업을 수행합니다.
	* @참조 : -
	*/ 
	void AdjustControllerRotation(float DeltaSeconds);

	/*
	* @목적 : 사용자의 카메라 회전에 따른 캐릭터 객체의 회전 보간 작업을 수행합니다.
	* @설명 : Float Curve를 통해 캐릭터 회전 값 보간 작업 과정에 필요한 Offset 값을 가져옵니다.
	* @참조 : APlayerCharacter::AdjustControllerRotation
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement|Control Rotation")
		UCurveFloat* DirectionCurve;
#pragma endregion

#pragma region SpringArm & Camera
	// @TODO: LockOn 기능 관련 Refactoring 진행 시, 수정할 곳
	// @목적 : LockOn 동안 Spring Arm을 통해 카메라 위치를 조정하는 함수
	void AdjustCameraTransform(float DeltaSeconds);

	TObjectPtr<class UBaseInputComponent> BaseInputComponent;
	TObjectPtr<class UBaseAnimInstance> BaseAnimInstance;

public:

};
