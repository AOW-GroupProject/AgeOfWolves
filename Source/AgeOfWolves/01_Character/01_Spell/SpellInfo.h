#pragma once

#include "CoreMinimal.h"
#include "SpellInfo.generated.h"


USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	/** 어떤 Target을 기준인지 저장함(추적, 레이저 등) */
	UPROPERTY()
	USceneComponent* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> ProjectileFactory;
};
