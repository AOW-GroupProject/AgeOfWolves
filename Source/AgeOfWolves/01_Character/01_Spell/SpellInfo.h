#pragma once

#include "CoreMinimal.h"
#include "SpellInfo.generated.h"

class USceneComponent;

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	/** AttributeSet�� ������*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Attribute")
	float DamageAmount;

	/*
	 * Projectile ����
	 */

	 /** �ʱ� �ӵ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	float InitialSpeed;

	/** �߷� ���� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	float Gravity;

	/** Bounce ���� üũ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	uint8 bShouldBounce : 1;

	/** Rotate�� ������ ������ ���� üũ*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Projectile")
	uint8 bRotationFollowsVelocity : 1;

	/** � Target�� �������� ������(����, ������ ��) */
	UPROPERTY()
	USceneComponent* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> ProjectileFactory;
};
