// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_GameplayCue.generated.h"

class UAbilitySystemComponent;
class USkeletalMeshComponent;
class AActor;

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UANS_GameplayCue : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FGameplayTag BeginGameplayCueTagTrigger;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FGameplayTag EndGameplayCueTagTrigger;


	// 이후 End시 이벤트 다시 호출해 나이아가라 삭제
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ProjectileEffectFactory;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSet<FGameplayTag> TrackedGameplayCues;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FName SocketName;

	UPROPERTY()
	AActor* EffectActor;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bAttachToWeapon = false;

	//UPROPERTY(EditAnywhere)
	//TSubclassOf<UGameplayEffect> NotifyCueEffect;


	//FActiveGameplayEffectHandle GameplayEffectHandle;

private:
	void Init(AActor* Owner, USkeletalMeshComponent* MeshComp, UAbilitySystemComponent* ASC);
	void Reset(UAbilitySystemComponent* ASC);

};
