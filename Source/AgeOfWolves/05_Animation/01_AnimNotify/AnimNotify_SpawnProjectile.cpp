// Fill out your copyright notice in the Description page of Project Settings.


#include "05_Animation/01_AnimNotify/AnimNotify_SpawnProjectile.h"
#include "01_Character/00_Projectile/Projectile.h"
#include "01_Character/01_Spell/UseAttachedMeshInterface.h"
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


	InitProjectile(MeshComp);
}


void UAnimNotify_SpawnProjectile::InitProjectile(USkeletalMeshComponent* MeshComp)
{
	if (!ensure(MeshComp))
	{
		return;
	}

	FTransform Transform = FTransform();
		
	// Owner(AkaOni, Wolf 등)에 소켓에 붙이고 싶다면?
	if (!bAttachToWeapon)
	{
		// Owner(AkaOni, Wolf 등)에 붙인다
		Transform = MeshComp->GetSocketTransform(BoneSocket);
	}


	// 투사체 타입을 순회하면서 만들어준다
	for (int32 Idx = 0; Idx < ProjectileInfos.Num(); ++Idx)
	{
		// ProjectileNums과 ProjectileTypes의 개수는 같아야 한다
		if (ensure(ProjectileNums.IsValidIndex(Idx)))
		{
			// TSubclassOf<AProjectile>로 생성할 준비를 마친다
			TSubclassOf<AProjectile> ProjectileFactory = ProjectileInfos[Idx].ProjectileFactory;

			// ProjectileFactory를 선언했다면 투사체 타입을 만들어준다
			if (ProjectileFactory)
			{
				SpawnProjectile(MeshComp, ProjectileFactory, Transform, Idx);
			}
				
		}
	}
}

void UAnimNotify_SpawnProjectile::SpawnProjectile(USkeletalMeshComponent* MeshComp, TSubclassOf<AProjectile> ProjectileFactory, FTransform SpawnTransform, int32 Index)
{
	// MeshComp에서 Owner가져오기 
	AActor* OwnerActor = MeshComp->GetOwner();

	// Actor에서 월드 가져오기
	UWorld* World = OwnerActor->GetWorld();

	// 해당 Notify가 World 상에서 작동한다면?
	if (World)
	{
		// World에 투사체를 생성한다(이 때 SpawnTransform은 무기에 안 붙일 시 기준으로 Transform을 구성했다)
		AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileFactory, SpawnTransform.GetLocation(), OwnerActor->GetActorRotation());
		Projectile->SetOwner(MeshComp->GetOwner());
		Projectile->InitProjectile(ProjectileInfos[Index]);

		// 무기에 붙인다
		if (bAttachToWeapon)
		{
			if (OwnerActor)
			{
				// Interface를 이용해 Weapon에 Mesh를 가져온다
				IUseAttachedMeshInterface* UseAttachActor = Cast<IUseAttachedMeshInterface>(OwnerActor);

				// Weapon에 Attach 시켜준다
				if (UseAttachActor)
				{
					UseAttachActor->AttachToWeapon(Projectile, BoneSocket);
				}
			}
		}
	}
}
