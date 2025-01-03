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
class USceneComponent;

/** 어떤 Target을 추적할 지 설정하는 값 */
UENUM(BlueprintType)
enum class ETargetType : uint8
{
	Player,
	Enemy,
};

USTRUCT(BlueprintType)
struct FProjectileTarget
{
	GENERATED_BODY()

	/** 타겟팅을 설정할 지 체크 */
	UPROPERTY(EditAnywhere)
	bool bUseTargeting;

	/** Target을 몇 초 뒤에 추척할 지(기본 값 0.f -> 바로 추척하는 기능)*/
	UPROPERTY(EditAnywhere)
	float TargetChaseDelay;

	/*
	 * Player, Enemy등을 Targeting할 때 사용하는 변수
	 * 다양한 것들을 타겟팅할 수 있도록 Array로 놓는다
	 * (혹시 모르니깐 Array로 지정함, Enemy, Boss 등으로 추척할 수 있으니)
	 */
	UPROPERTY()
	TArray<ETargetType> TargetTypes;

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> Target;
	
	FProjectileTarget() {}

	FProjectileTarget(const FProjectileTarget& Other)
		: bUseTargeting(Other.bUseTargeting)
		, TargetChaseDelay(Other.TargetChaseDelay)
		, TargetTypes(Other.TargetTypes)
		, Target(Other.Target)
	{}
};

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

	/** OffsetPos */
	UPROPERTY(EditAnywhere)
	FVector OffsetPos;


	/** 해당 SubClassOf로 투사체를 생성*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> ProjectileFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOW|Target")
	FProjectileTarget TargetInfo;
};


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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	/** Target 정보를 사용할 수 있도록 설정하는 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AOW|Target")
	FProjectileTarget TargetInfo;

	/** Target을 다시 추척할 때 사용하는 변수 */
	FTimerHandle TargetHandle;

#pragma endregion


#pragma region Callbacks
public:
	UFUNCTION()
	virtual void OnProjectileImpact(const FHitResult& Hit);

	UFUNCTION()
	virtual void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitTarget();

protected:
	void SetTarget();

	void HandleTarget();

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
