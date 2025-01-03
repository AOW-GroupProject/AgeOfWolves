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

	// ProjectileTypes의 작업은 싱글톤으로 해당 투사체의 정보를 불러오는 게 나을 것 같다(예정)
	// 여기서 Enemy, Player Type에 따라 충돌체만 깔끔하게 바꿔주면 될 것 같다
	// 해당 작업은 아직 수행하지 않는다

	/** 한 프레임에 발사되는 투사체 타입 */
	UPROPERTY(EditAnywhere)
	TArray<FProjectileInfo> ProjectileInfos;

	/** 한 프레임에 발사되는 투사체의 개수 */
	UPROPERTY(EditAnywhere)
	TArray<uint8> ProjectileNums;

	// bUseWeaponSocket
	// WeaponMesh을 가져와서 해야함 -> 해당 작업은 아직 수행하지 않는다
	// Interface -> Mesh를 가져와서 사용한다

	/** 지팡이, 무기에서 생성된다면? */
	UPROPERTY(EditAnywhere)
	bool bAttachToWeapon;

	/** 해당 이름 뼈에 투사체를 장착 */
	UPROPERTY(EditAnywhere)
	FName BoneSocket;

#pragma endregion

private:
	/** 전반적인 투사체 초기화 관리 */
	void InitProjectile(USkeletalMeshComponent* MeshComp);
	
	/** 투사체 생성 */
	void SpawnProjectile(USkeletalMeshComponent* MeshComp, TSubclassOf<AProjectile> ProjectileFactory, FTransform SpawnTransform, int32 Index);


};
