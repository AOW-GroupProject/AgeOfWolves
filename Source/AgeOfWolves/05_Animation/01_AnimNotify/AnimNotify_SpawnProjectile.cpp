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
		CreateProjectile(MeshComp);
	}
}


void UAnimNotify_SpawnProjectile::CreateProjectile(USkeletalMeshComponent* MeshComp)
{

	// MeshComp에서 Owner가져오기 
	AActor* OwnerActor = MeshComp->GetOwner();

	// MeshComp에서 월드 가져오기
	UWorld* World = OwnerActor->GetWorld();
	if (World)
	{
		FTransform Transform = FTransform();
		
		// 무기에 투사체를 소환하고 싶다면?
		if (bUseWeaponSocket)
		{
			// 무기에 붙인다.. 이 부분은 추가해야함
		}
		else
		{
			// Onwer에 붙인다
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
					AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileFactory, Transform.GetLocation(), OwnerActor->GetActorRotation());
					Projectile->SetOwner(MeshComp->GetOwner());
					Projectile->InitProjectile(ProjectileInfos[Idx]);
				}
				
			}
		}
		
	}
}

void UAnimNotify_SpawnProjectile::Targeting()
{
}
