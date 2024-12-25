// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SpawnProjectile.generated.h"

class AProjectile;

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()


#pragma region Default Setting
public:
	UAnimNotify_SpawnProjectile();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#pragma endregion


#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileFactory;

	UPROPERTY(EditAnywhere)
	FName BoneSocket;

	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerGameplayTag;

#pragma endregion
};
