// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BaseAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimInstance, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EMovementState
* 
*	현재 이동 상태 관련 열거형
*/
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

/*
*	@EMovementDirection
* 
*	이동 방향 관련 열거형
*/
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Fwd = 0		UMETA(DisplayName = "Fwd"),
	Bwd			UMETA(DisplayName = "Bwd"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	MAX			UMETA(DisplayName = "MAX"),
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UBaseAnimInstance
 * 
 *	Anim Instance의 기본 유형을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBaseAnimInstance : public UAnimInstance
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseAnimInstance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UFUNCTION(BlueprintCallable)
		void FindMovementState();

	UFUNCTION(BlueprintCallable)
		void FindMovementDirection();

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ChangeBoneTransform(float DeltaTime);
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) { }

	UFUNCTION(BlueprintNativeEvent)
		void ClearBoneTransform(float DeltaTime);
	virtual void ClearBoneTransform_Implementation(float DeltaTime) { }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState LastMovementState = EMovementState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Direction", meta = (AlloPrivateAccess = "true"))
		EMovementDirection MovementDirection = EMovementDirection::Fwd;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bFalling;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bShouldMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector Velocity;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Speed;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float DirectionAngle;

	bool bLockOn = false;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bModifyBoneTransform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BoneTransformLerpSpeed = 10.f;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBase;

public:
	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
		FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
		FORCEINLINE EMovementState GetLastMovementState() const { return LastMovementState; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
		FORCEINLINE float GetDirectionAngle() const { return DirectionAngle; }

	UFUNCTION(BlueprintCallable)
		FORCEINLINE EMovementDirection GetMovementDirection() const { return MovementDirection; }

	FORCEINLINE void SetbLockOn(bool LockOn) { bLockOn = LockOn; }
#pragma endregion

};
