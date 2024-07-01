// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BaseAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimInstance, Log, All)

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle = 0	UMETA(DisplayName = "Idle"),
	Walk		UMETA(DisplayName = "Walk"),
	Run			UMETA(DisplayName = "Run"),
	Pivoting	UMETA(DisplayName = "Pivoting"),
	MAX			UMETA(DisplayName = "MAX"),
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Fwd = 0		UMETA(DisplayName = "Fwd"),
	Bwd			UMETA(DisplayName = "Bwd"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	MAX			UMETA(DisplayName = "MAX"),
};

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
#pragma region Default Setting

protected:
	/*
	* [설명] : Owner의 Beginplay() 함수 호출 시 호출되는 함수
	*/
	virtual void NativeBeginPlay() override;
	/*
	* [설명] : 초기화 함수
	*/
	virtual void NativeInitializeAnimation() override;
	/*
	* [설명] : 매 Frame마다 호출되는 Tick 함수
	*/
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/*
	* [설명] : Anim Instance의 대상이 되는 캐릭터 오너에 대한 약한 참조
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess ="true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBase;

protected:
	/* 
	*  @목적 : 현재 캐릭터의 이동 상태(정지, 걷기, 뛰기...etc)를 찾습니다.
	*  @설명 : 현재 캐릭터의 Anim Instance에서 각 이동 상태에 대응되는 각각의 Animation을 적절히 재생하기 위함입니다.
	*  @참조 : ABP_AkaOni_Base, ABP_AkaOni_BaseLayer
	*/
	UFUNCTION(BlueprintCallable)
		void FindMovementState();

	/*
	* @목적: 이전 MovementState
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState LastMovementState = EMovementState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::Idle;
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

protected:

	/*
	*  @목적 : 현재 캐릭터의 이동 방향을 찾습니다.
	*  @설명 : 이동 방향은 총 4가지로, EMovementDirection 유형의 변수로 나타냅니다.
	*  @참조 : ABP_AkaOni_Base, ABP_AkaOni_BaseLayer
	*/
	UFUNCTION(BlueprintCallable)
		void FindMovementDirection();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Direction", meta = (AlloPrivateAccess = "true"))
		EMovementDirection MovementDirection = EMovementDirection::Fwd;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE EMovementDirection GetMovementDirection() const { return MovementDirection; }

#pragma endregion

#pragma region Movement
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bFalling;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bShouldMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector Velocity;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Speed;

	// @목적 : 현재 플레이어가 LockOn 중인지 나타내는 bool 변수
	bool bLockOn = false;

#pragma endregion

#pragma region IK
protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bModifyBoneTransform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BoneTransformLerpSpeed = 10.f;

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ChangeBoneTransform(float DeltaTime);
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) { }

	UFUNCTION(BlueprintNativeEvent)
		void ClearBoneTransform(float DeltaTime);
	virtual void ClearBoneTransform_Implementation(float DeltaTime) { }
#pragma endregion

public:
	FORCEINLINE void SetbLockOn(bool LockOn) { bLockOn = LockOn; }
};
