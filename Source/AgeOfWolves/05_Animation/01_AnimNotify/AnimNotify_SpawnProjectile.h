// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "01_Character/01_Spell/SpellInfo.h"
#include "AnimNotify_SpawnProjectile.generated.h"

class AProjectile;

UENUM(BlueprintType)
enum class ETargetType : uint8
{
	Player,
	Enemy,
};

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

	/** 투사체의 타깃을 확인해서 투사체로 전달해준다 */
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> Target;

	/*
	 * Player, Enemy등을 Targeting할 때 사용하는 변수
	 * 다양한 것들을 타겟팅할 수 있도록 Array로 놓는다
	 * (혹시 모르니깐 Array로 지정함, Enemy, Boss 등으로 추척할 수 있으니)
	 */
	UPROPERTY()
	TArray<ETargetType> TargetType;

	// bUseWeaponSocket
	// WeaponMesh을 가져와서 해야함 -> 해당 작업은 아직 수행하지 않는다
	// Interface -> Mesh를 가져와서 사용한다

	/** 지팡이, 무기에서 생성된다면? */
	UPROPERTY(EditAnywhere)
	bool bUseWeaponSocket;

	/** 해당 이름 뼈에 투사체를 장착 */
	UPROPERTY(EditAnywhere)
	FName BoneSocket;

#pragma endregion

private:
	void CreateProjectile(USkeletalMeshComponent* MeshComp);
	void Targeting();

};
