// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/00_Projectile/Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile() : DamageAmount(10.f)
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
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

    if (ProjectileMovementComponent)
    {
        // Delegate 바인딩
        ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileImpact);
    }
}


void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnProjectileImpact(const FHitResult& Hit)
{
    
}

void AProjectile::SetFriction(float InFriction)
{
    ProjectileMovementComponent->Friction =  InFriction;
}

void AProjectile::InitProjectile()
{
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = InitialSpeed;
    ProjectileMovementComponent->bRotationFollowsVelocity = bRotationFollowsVelocity;
    ProjectileMovementComponent->bShouldBounce = bShouldBounce;
    ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * InitialSpeed;
}

