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
	Pivoting	UMETA(DisplayName = "Pivoting")
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Fwd = 0		UMETA(DisplayName = "Fwd"),
	Bwd			UMETA(DisplayName = "Bwd"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right")
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
	* [����] : Owner�� Beginplay() �Լ� ȣ�� �� ȣ��Ǵ� �Լ�
	*/
	virtual void NativeBeginPlay() override;
	/*
	* [����] : �ʱ�ȭ �Լ�
	*/
	virtual void NativeInitializeAnimation() override;
	/*
	* [����] : �� Frame���� ȣ��Ǵ� Tick �Լ�
	*/
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/*
	* [����] : Anim Instance�� ����� �Ǵ� ĳ���� ���ʿ� ���� ���� ����
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess ="true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBase;

protected:
	/* 
	*  @���� : ���� ĳ������ �̵� ����(����, �ȱ�, �ٱ�...etc)�� ã���ϴ�.
	*  @���� : ���� ĳ������ Anim Instance���� �� �̵� ���¿� �����Ǵ� ������ Animation�� ������ ����ϱ� �����Դϴ�.
	*  @���� : ABP_AkaOni_Base, ABP_AkaOni_BaseLayer
	*/
	UFUNCTION(BlueprintCallable)
		void FindMovementState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::Idle;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

	/*
	*  @���� : ���� ĳ������ �̵� ������ ã���ϴ�.
	*  @���� : �̵� ������ �� 4������, EMovementDirection ������ ������ ��Ÿ���ϴ�.
	*  @���� : ABP_AkaOni_Base, ABP_AkaOni_BaseLayer
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

#pragma endregion

#pragma region IK

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

};
