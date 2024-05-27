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
	
};
