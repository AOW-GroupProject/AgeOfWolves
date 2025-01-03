#pragma once

#include "CoreMinimal.h"
#include "SpellInfo.generated.h"


USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	/** � Target�� �������� ������(����, ������ ��) */
	UPROPERTY()
	USceneComponent* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> ProjectileFactory;
};
