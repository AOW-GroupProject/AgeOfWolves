// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BaseAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimInstance, Log, All)

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle = 0    UMETA(DisplayName = "Idle"),
	Walking     UMETA(DisplayName = "Walking"),
	WalkStop    UMETA(DisplayName = "WalkStop"),
	Sprinting   UMETA(DisplayName = "Sprinting"),
	SprintStop  UMETA(DisplayName = "SprintStop"),
	MAX         UMETA(DisplayName = "MAX"),
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
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess ="true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBase;

protected:
	UFUNCTION(BlueprintCallable)
		void FindMovementState();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState LastMovementState = EMovementState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::Idle;

public:
	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
		FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
		FORCEINLINE EMovementState GetLastMovementState() const { return LastMovementState; }

protected:
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
