// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseMonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UBaseMonsterAnimInstance();
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	UPROPERTY(BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = true))
	class ABaseMonster* MyPlayer;

	
	UPROPERTY(BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
	float Speed = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
	bool isMoving = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
	float Dir = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
	bool IsFocusingPlayer = false; //현재 GetFocusActor가 존재한다면 true. Strafe Movement를 만드는 몬스터라면 이 변수를 Blend Poses by bool에 사용.


	
};
