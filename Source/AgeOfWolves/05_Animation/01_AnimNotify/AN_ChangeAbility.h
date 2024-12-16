// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_ChangeAbility.generated.h"



#pragma region Forward Declaration
class AProjectile;
#pragma endregion


/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UAN_ChangeAbility : public UAnimNotify
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	UAN_ChangeAbility();

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#pragma endregion


#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerGameplayTag;
#pragma endregion

};
