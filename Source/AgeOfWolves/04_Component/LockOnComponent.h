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

	// @���� : ������� LockOn IA�� �����Ǵ� Lock On ����� �����ϴ� �ݹ� �Լ� 
	void Input_LockOn();

	// @���� : LockOn�� �����ϴ� �Լ�
	void StartLockOn();
	// @���� : LockOn�� ����ϴ� �Լ�
	void CancelLockOn();

	// @���� : ������� ChageLockOnTarget IA�� �����Ǵ� Lock On Target ������ �����ϴ� �ݹ� �Լ� (������� ����)
	void Input_ChangeLockOnTarget(const FInputActionValue& Value);


	// @���� : TargetEnemy �������� Controller Rotation�� �����ϴ� �Լ�
	void SetControllerRotationTowardTarget();
	// @���� : �ֺ� Enemy�� ã�� TargetEnemy�� �����ϴ� �Լ�
	// @���� : TargetEnemy�� ã������ true�� ��ȯ�Ѵ�.
	bool FindTargetEnemy();

	// @���� : �ɸ��Ͱ� LockOn�� ���� ã�� ���� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Action | Native Input Action")
	float MaxDetectRadius = 1000.f;
	// @���� : �ɸ��Ͱ� LockOn�� �����ϴ� �ִ� �Ÿ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Action | Native Input Action")
	float  MaxLockOnDistance = 2000.f;

	// @���� : Input_LockOn �Լ� ���� �� ã�� �� Actor ��� Array
	UPROPERTY()
	TArray<AActor*> NearByEnemies;
	// @���� : nput_LockOn �Լ� ���� �� ����� �������� ã�� �� Actor�� ��� Map
	UPROPERTY()
	TMap<float, AActor*> EnemyMap;
	// @���� : ���� �÷��̾ LockOn ���� �� Actor
	UPROPERTY()
	AActor* TargetEnemy;
	// @���� : ���� �÷��̾ LockOn ������ ��Ÿ���� bool ����
	bool bLockOn = false;
	// @���� : ���� �÷��̾��� �Է� ����
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
