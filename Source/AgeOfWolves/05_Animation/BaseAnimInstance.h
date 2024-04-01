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
	* [설명] : Anim Instance의 대상이 되는 캐릭터 오너에 대한 약한 참조
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess ="true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBase;
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
