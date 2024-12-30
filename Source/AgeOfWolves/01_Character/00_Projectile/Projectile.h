// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../01_Spell/SpellInfo.h"
#include "Projectile.generated.h"

class UAbilitySystemComponent;
struct FGameplayTag;
struct FGameplayCueParameters;
class UBaseAttributeSet;
class UGameplayEffect;



/** Projectile의 여부를 결정지어주는 Enum */
UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Linear,
	Homing,
	Curve,
};

/*
 *	전반적인 Projectile을 관리하는 클래스
 */
UCLASS()
class AGEOFWOLVES_API AProjectile : public AActor
{
	GENERATED_BODY()


#pragma region Default Setting
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

#pragma endregion


#pragma region Property or Subwidgets or Infos...etc
protected:
    /** 충돌 처리 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOW|Components")
    class USphereComponent* CollisionComponent; 

	/** 외형 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOW|Components")
    class UStaticMeshComponent* MeshComponent; 

	/** 이동 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AOW|Components")
    class UProjectileMovementComponent* ProjectileMovementComponent; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Abilities")
	UAbilitySystemComponent* ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Abilities")
	UBaseAttributeSet* AS;

	/** Damage 보관 Effect*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOW|Abilities")
	TSubclassOf<UGameplayEffect> GameplayEffectToApply;

	/** Damage -> Health 적용 Effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOW|Abilities")
	TSubclassOf<UGameplayEffect> GameplayEfectDamaToHealth;


#pragma endregion


#pragma region Callbacks
public:
	UFUNCTION()
	virtual void OnProjectileImpact(const FHitResult& Hit);

#pragma endregion


#pragma region Utility
public:
	/** Tag를 이용해 Cue를 호출하는 함수 */
	UFUNCTION(BlueprintCallable)
	void ExecuteGameplayCueWithParams(FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	/** 마찰 반발 계수 조정 함수 */
	UFUNCTION(BlueprintCallable)
	void SetFriction(float InFriction);

	/** ProjectileMovemntComponent 초기화 함수*/
	UFUNCTION(BlueprintCallable)
	void InitProjectile(FProjectileInfo ProjectileStat);

	/** Damage 전달 함수 */
	UFUNCTION(BlueprintCallable)
	void SetDamage(float InDamage);

	void StopProjectile();

#pragma endregion
};
