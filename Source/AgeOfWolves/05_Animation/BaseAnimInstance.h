// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BaseAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimInstance, Log, All)

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
	* [����] : Anim Instance�� ����� �Ǵ� ĳ���� ���ʿ� ���� ���� ����
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess ="true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBase;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Direction;


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

};
