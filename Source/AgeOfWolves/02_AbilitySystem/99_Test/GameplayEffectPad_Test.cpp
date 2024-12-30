// Fill out your copyright notice in the Description page of Project Settings.


#include "02_AbilitySystem/99_Test/GameplayEffectPad_Test.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"

AGameplayEffectPad_Test::AGameplayEffectPad_Test()
{
	PrimaryActorTick.bCanEverTick = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	AS = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("Base AttributeSet"));
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

    SetRootComponent(Sphere);
}

void AGameplayEffectPad_Test::BeginPlay()
{
	Super::BeginPlay();
	AS->InitOffense(20);

    if (Sphere)
    {
        Sphere->OnComponentBeginOverlap.AddDynamic(this, &AGameplayEffectPad_Test::OnPadOverlap);
    }
}

void AGameplayEffectPad_Test::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	GameplayEffectToApply = GameplayEffectToApply;
}


void AGameplayEffectPad_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameplayEffectPad_Test::OnPadOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    //if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
    //{
    //    if (GameplayEffectToApply)
    //    {
    //        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext(); // 컨텍스트
    //        float Level = 1.0f; // 레벨  

    //        UE_LOG(LogTemp, Warning, TEXT("Damage : %f"), AS->GetOffense());

    //        // GameplayEffect의 인스턴스를 생성하지 않고 클래스 타입으로 전달
    //        ASC->ApplyGameplayEffectToTarget
    //        (   
    //            GameplayEffectToApply->GetDefaultObject<UGameplayEffect>(), // 클래스에서 기본 객체를 가져옴
    //            TargetASC, // 대상 ASC
    //            Level,
    //            EffectContext// 컨텍스트
    //        );

    //        if (GameplayEfectDamaToHealth)
    //        {
    //            TargetASC->ApplyGameplayEffectToSelf
    //            (
    //                GameplayEfectDamaToHealth->GetDefaultObject<UGameplayEffect>(), 
    //                Level, 
    //                TargetASC->MakeEffectContext()
    //            );
    //        }
    //        
    //    }


    //}
}
