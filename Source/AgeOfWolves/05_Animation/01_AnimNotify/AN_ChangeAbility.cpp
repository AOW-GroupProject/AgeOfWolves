// Fill out your copyright notice in the Description page of Project Settings.


#include "05_Animation/01_AnimNotify/AN_ChangeAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "01_Character/00_Projectile/Projectile_Linear.h"

UAN_ChangeAbility::UAN_ChangeAbility()
{
}

FString UAN_ChangeAbility::GetNotifyName_Implementation() const
{
	return TEXT("SpawnSpawnCheck");
}

void UAN_ChangeAbility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			FGameplayEventData Payload;

			// ASC가 있다는 가정하에 동작
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, Payload);
			//UE_LOG(LogTemp, Warning, TEXT("Notify 호출"));
		}
	}
}
