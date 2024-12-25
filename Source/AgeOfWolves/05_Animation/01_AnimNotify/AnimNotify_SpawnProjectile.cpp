// Fill out your copyright notice in the Description page of Project Settings.


#include "05_Animation/01_AnimNotify/AnimNotify_SpawnProjectile.h"
#include "01_Character/00_Projectile/Projectile.h"
#include "AbilitySystemBlueprintLibrary.h"



UAnimNotify_SpawnProjectile::UAnimNotify_SpawnProjectile()
{
}

FString UAnimNotify_SpawnProjectile::GetNotifyName_Implementation() const
{
	return TEXT("SpawnProjectile");
}

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
		}

		if (ProjectileFactory)
		{
			// MeshComp에서 월드 가져오기
			UWorld* World = MeshComp->GetWorld();
			if (World)
			{
				FTransform Transform = MeshComp->GetSocketTransform(BoneSocket);
				
				World->SpawnActor<AProjectile>(ProjectileFactory, Transform.GetLocation(), OwnerActor->GetActorRotation());
			}
		}
	}
}
