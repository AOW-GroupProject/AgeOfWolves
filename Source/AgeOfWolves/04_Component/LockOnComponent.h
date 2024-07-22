// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULockOnComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:

	// @목적 : 사용자의 LockOn IA에 대응되는 Lock On 기능을 정의하는 콜백 함수 
	void Input_LockOn();

	// @목적 : LockOn을 시작하는 함수
	void StartLockOn();
	// @목적 : LockOn을 취소하는 함수
	void CancelLockOn();

	// @목적 : 사용자의 ChageLockOnTarget IA에 대응되는 Lock On Target 변경을 정의하는 콜백 함수 (사용하지 않음)
	void Input_ChangeLockOnTarget(const FInputActionValue& Value);


	// @목적 : TargetEnemy 방향으로 Controller Rotation을 설정하는 함수
	void SetControllerRotationTowardTarget();
	// @목적 : 주변 Enemy를 찾고 TargetEnemy를 설정하는 함수
	// @설명 : TargetEnemy를 찾았으면 true를 반환한다.
	bool FindTargetEnemy();

	// @설명 : 케릭터가 LockOn시 적을 찾는 감지 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Action | Native Input Action")
	float MaxDetectRadius = 1000.f;
	// @설명 : 케릭터가 LockOn을 유지하는 최대 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Action | Native Input Action")
	float  MaxLockOnDistance = 2000.f;

	// @설명 : Input_LockOn 함수 실행 시 찾은 적 Actor 담는 Array
	UPROPERTY()
	TArray<AActor*> NearByEnemies;
	// @설명 : nput_LockOn 함수 실행 시 계산한 내적값과 찾은 적 Actor를 담는 Map
	UPROPERTY()
	TMap<float, AActor*> EnemyMap;
	// @설명 : 현재 플레이어가 LockOn 중인 적 Actor
	UPROPERTY()
	AActor* TargetEnemy;
	// @설명 : 현재 플레이어가 LockOn 중인지 나타내는 bool 변수
	bool bLockOn = false;
	// @설명 : 현재 플레이어의 입력 벡터
	FVector2D InputVector;

	UPROPERTY(BlueprintReadWrite)
	int32 InputCount;

	void AdjustCameraTransform(float DeltaTime);

	
private:

	class APlayerCharacter* PlayerCharacter;
	class UBaseAnimInstance* BaseAnimInstance;
	class USpringArmComponent* SpringArmComponent;
	class UCameraComponent* FollowCameraComponent;
	class UBaseInputComponent* BaseInputComponent;

public:
	FORCEINLINE bool GetbLockOn() { return bLockOn; }
};
