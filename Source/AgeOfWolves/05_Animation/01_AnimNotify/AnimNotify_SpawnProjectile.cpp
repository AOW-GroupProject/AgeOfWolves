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
		
	// Owner(AkaOni, Wolf ��)�� ���Ͽ� ���̰� �ʹٸ�?
	if (!bAttachToWeapon)
	{
		// Owner(AkaOni, Wolf ��)�� ���δ�
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
				SpawnProjectile(MeshComp, ProjectileFactory, Transform, Idx);
			}
				
		}
	}
}

void UAnimNotify_SpawnProjectile::SpawnProjectile(USkeletalMeshComponent* MeshComp, TSubclassOf<AProjectile> ProjectileFactory, FTransform SpawnTransform, int32 Index)
{
	// MeshComp���� Owner�������� 
	AActor* OwnerActor = MeshComp->GetOwner();

	// Actor���� ���� ��������
	UWorld* World = OwnerActor->GetWorld();

	// �ش� Notify�� World �󿡼� �۵��Ѵٸ�?
	if (World)
	{
		// World�� ����ü�� �����Ѵ�(�� �� SpawnTransform�� ���⿡ �� ���� �� �������� Transform�� �����ߴ�)
		AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileFactory, SpawnTransform.GetLocation(), OwnerActor->GetActorRotation());
		Projectile->SetOwner(MeshComp->GetOwner());
		Projectile->InitProjectile(ProjectileInfos[Index]);

		// ���⿡ ���δ�
		if (bAttachToWeapon)
		{
			if (OwnerActor)
			{
				// Interface�� �̿��� Weapon�� Mesh�� �����´�
				IUseAttachedMeshInterface* UseAttachActor = Cast<IUseAttachedMeshInterface>(OwnerActor);

				// Weapon�� Attach �����ش�
				if (UseAttachActor)
				{
					UseAttachActor->AttachToWeapon(Projectile, BoneSocket);
				}
			}
		}
	}
}
