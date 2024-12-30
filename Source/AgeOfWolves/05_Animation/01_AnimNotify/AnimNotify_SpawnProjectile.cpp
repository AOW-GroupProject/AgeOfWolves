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

	// MeshComp���� Owner�������� 
	AActor* OwnerActor = MeshComp->GetOwner();

	// MeshComp���� ���� ��������
	UWorld* World = OwnerActor->GetWorld();
	if (World)
	{
		FTransform Transform = FTransform();
		
		// ���⿡ ����ü�� ��ȯ�ϰ� �ʹٸ�?
		if (bUseWeaponSocket)
		{
			// ���⿡ ���δ�.. �� �κ��� �߰��ؾ���
		}
		else
		{
			// Onwer�� ���δ�
			Transform = MeshComp->GetSocketTransform(BoneSocket);
		}


		// ����ü Ÿ���� ��ȸ�ϸ鼭 ������ش�
		for (int32 Idx = 0; Idx < ProjectileInfos.Num(); ++Idx)
		{
			// ProjectileNums�� ProjectileTypes�� ������ ���ƾ� �Ѵ�
			if (ensure(ProjectileNums.IsValidIndex(Idx)))
			{
				// TSubclassOf<AProjectile>�� ������ �غ� ��ģ��
				TSubclassOf<AProjectile> ProjectileFactory = ProjectileInfos[Idx].ProjectileFactory;

				// ProjectileFactory�� �����ߴٸ� ����ü Ÿ���� ������ش�
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
