// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SpawnProjectile.generated.h"

class AProjectile;
struct FProjectileInfo;


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

	// ProjectileTypes�� �۾��� �̱������� �ش� ����ü�� ������ �ҷ����� �� ���� �� ����(����)
	// ���⼭ Enemy, Player Type�� ���� �浹ü�� ����ϰ� �ٲ��ָ� �� �� ����
	// �ش� �۾��� ���� �������� �ʴ´�

	/** �� �����ӿ� �߻�Ǵ� ����ü Ÿ�� */
	UPROPERTY(EditAnywhere)
	TArray<FProjectileInfo> ProjectileInfos;

	/** �� �����ӿ� �߻�Ǵ� ����ü�� ���� */
	UPROPERTY(EditAnywhere)
	TArray<uint8> ProjectileNums;

	// bUseWeaponSocket
	// WeaponMesh�� �����ͼ� �ؾ��� -> �ش� �۾��� ���� �������� �ʴ´�
	// Interface -> Mesh�� �����ͼ� ����Ѵ�

	/** ������, ���⿡�� �����ȴٸ�? */
	UPROPERTY(EditAnywhere)
	bool bAttachToWeapon;

	/** �ش� �̸� ���� ����ü�� ���� */
	UPROPERTY(EditAnywhere)
	FName BoneSocket;

#pragma endregion

private:
	/** �������� ����ü �ʱ�ȭ ���� */
	void InitProjectile(USkeletalMeshComponent* MeshComp);
	
	/** ����ü ���� */
	void SpawnProjectile(USkeletalMeshComponent* MeshComp, TSubclassOf<AProjectile> ProjectileFactory, FTransform SpawnTransform, int32 Index);


};
