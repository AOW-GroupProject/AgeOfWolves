// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/00_Projectile/Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagContainer.h"
#include "02_AbilitySystem/02_GamePlayAbility/SpellGameplayAbility.h"
#include "GameplayCueFunctionLibrary.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

    // 콜리전 컴포넌트 초기화
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1); // 필요시 사용자 정의 채널
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

    SetRootComponent(CollisionComponent);

    // ProjectileMovement 설정
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->SetupAttachment(RootComponent);

    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
    AS = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("Base AttributeSet"));
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

    if (ProjectileMovementComponent)
    {
        // Delegate 바인딩
        ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileImpact);
        ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectile::OnProjectileBounce);
    }
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 이전 타이머 취소
    GetWorld()->GetTimerManager().ClearTimer(TargetHandle);  

    Super::EndPlay(EndPlayReason);
}


void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnProjectileImpact(const FHitResult& Hit)
{

}

void AProjectile::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{

}

void AProjectile::HandleTarget()
{
    GetWorld()->GetTimerManager().ClearTimer(TargetHandle);
    GetWorld()->GetTimerManager().SetTimer(TargetHandle, this, &AProjectile::SetTarget, TargetInfo.TargetChaseDelay, false);
}


void AProjectile::SetTarget()
{
    InitTarget();

    //UE_LOG(LogTemp, Warning, TEXT("AProjectile::SetTarget_Implementation TestTestTest"));
}

void AProjectile::ExecuteGameplayCueWithParams(FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
    UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(GetOwner(), GameplayCueTag, GameplayCueParameters);

   /* if (ParentGameAbility)
    {

        ParentGameAbility->ExecuteGameplayCueWithParams(GameplayCueTag, GameplayCueParameters);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ParentGameAbility Is Not Valid"));
    }*/
}

void AProjectile::InitProjectile(FProjectileInfo ProjectileStat)
{
    /** 투사체 컴포넌트 초기화 */
    ProjectileMovementComponent->InitialSpeed = ProjectileStat.InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = ProjectileStat.InitialSpeed;
    ProjectileMovementComponent->bRotationFollowsVelocity = ProjectileStat.bRotationFollowsVelocity;
    ProjectileMovementComponent->bShouldBounce = ProjectileStat.bShouldBounce;
    ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * ProjectileStat.InitialSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = ProjectileStat.Gravity;
    
    /** 투사체 데미지 초기화 */
    AS->InitOffense(ProjectileStat.DamageAmount);
    
    /** Offset 조정 */
    //SetActorRelativeLocation(ProjectileStat.OffsetPos);

    // 타겟팅을 한다면?
    if (ProjectileStat.TargetInfo.bUseTargeting)
    {
        // 무브 시멘틱을 통해 전달한다 TargetInfo를 전달한다
        TargetInfo = MoveTemp(ProjectileStat.TargetInfo);
        check(TargetInfo.bUseTargeting);

        // Timer로 타겟을 언제 찾아갈 지 시간을 정해준다
        HandleTarget();
    }
    else
    {
        // 타겟팅을 수행하지 않는다.
        TargetInfo.bUseTargeting = false;
    }
    
}


void AProjectile::SetFriction(float InFriction)
{
    ProjectileMovementComponent->Friction = InFriction;
}


void AProjectile::SetDamage(float InDamage)
{
    AS->InitOffense(InDamage);
}

