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
    //        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext(); // ���ؽ�Ʈ
    //        float Level = 1.0f; // ����  

    //        UE_LOG(LogTemp, Warning, TEXT("Damage : %f"), AS->GetOffense());

    //        // GameplayEffect�� �ν��Ͻ��� �������� �ʰ� Ŭ���� Ÿ������ ����
    //        ASC->ApplyGameplayEffectToTarget
    //        (   
    //            GameplayEffectToApply->GetDefaultObject<UGameplayEffect>(), // Ŭ�������� �⺻ ��ü�� ������
    //            TargetASC, // ��� ASC
    //            Level,
    //            EffectContext// ���ؽ�Ʈ
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
