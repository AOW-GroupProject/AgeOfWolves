#pragma once

#include "CoreMinimal.h"
#include "SpellInfo.generated.h"

class USceneComponent;

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	/** AttributeSet의 데미지*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Attribute")
	float DamageAmount;

	/*
	 * Projectile 변수
	 */

	 /** 초기 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	float InitialSpeed;

	/** 중력 조정 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	float Gravity;

	/** Bounce 여부 체크 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	uint8 bShouldBounce : 1;

	/** Rotate를 수행할 것인지 여부 체크*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	uint8 bRotationFollowsVelocity : 1;

	/** 어떤 Target을 기준인지 저장함(추적, 레이저 등) */
	UPROPERTY()
	USceneComponent* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> ProjectileFactory;
};
