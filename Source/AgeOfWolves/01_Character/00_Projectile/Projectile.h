// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Linear,
	Homing,
	Curve,
};


UCLASS()
class AGEOFWOLVES_API AProjectile : public AActor
{
	GENERATED_BODY()


#pragma region Default Setting
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion


#pragma region Property or Subwidgets or Infos...etc
protected:
    /**  */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOW|Components")
    class USphereComponent* CollisionComponent; // 충돌 처리

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOW|Components")
    class UStaticMeshComponent* MeshComponent; // 외형

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOW|Components")
    class UProjectileMovementComponent* ProjectileMovementComponent; // 이동

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Stats")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Stats")
	float InitialSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Stats")
	float Gravity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Stats")
	uint8 bShouldBounce:1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Stats")
	uint8 bRotationFollowsVelocity:1;

	UPROPERTY()
	USceneComponent* Target;

#pragma endregion


#pragma region Callbacks
public:
	UFUNCTION()
	void OnProjectileImpact(const FHitResult& Hit);
#pragma endregion


#pragma region Utility
public:
	UFUNCTION(BlueprintCallable)
	void SetFriction(float InFriction);

	/** ProjectileMovemntComponent 초기화 함수*/
	UFUNCTION(BlueprintCallable)
	void InitProjectile();
#pragma endregion
};
