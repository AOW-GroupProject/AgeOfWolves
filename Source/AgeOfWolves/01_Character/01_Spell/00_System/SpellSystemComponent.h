// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../SpellInfo.h"
#include "SpellSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API USpellSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpellSystemComponent();

protected:
	virtual void BeginPlay() override;

protected:	
	/** Projectile의 정보를 담아 사용할 수 있도록 캐싱해두는 변수*/
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TArray<FProjectileInfo> ProjectileInfoSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TArray<uint8> ProjectileInfoSet;*/


};
