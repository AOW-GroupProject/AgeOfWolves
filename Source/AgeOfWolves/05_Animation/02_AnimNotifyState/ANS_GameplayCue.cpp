// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_GameplayCue.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "01_Character/01_Spell/UseAttachedMeshInterface.h"

void UANS_GameplayCue::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (MeshComp)
    {
        AActor* MyOwner = MeshComp->GetOwner();
        if (MyOwner)
        {
            UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MyOwner);
            if (ensure(ASC))
            {
                // Interface를 이용해 Weapon에 Mesh를 가져온다
                IUseAttachedMeshInterface* UseAttachActor = Cast<IUseAttachedMeshInterface>(MyOwner);
                USkeletalMeshComponent* OwnerMeshComp = (UseAttachActor && bAttachToWeapon) ? UseAttachActor->GetWeaponSkeletalMeshComponent() : MeshComp;

                // 메쉬가 존재한다면?
                if (OwnerMeshComp)
                {
                    // 초기화 시켜준다
                    Init(MyOwner, MeshComp, ASC);
                }
            }
        }
    }

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UANS_GameplayCue::NotifyBegin"));
}

void UANS_GameplayCue::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
}

void UANS_GameplayCue::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (MeshComp)
    {
        AActor* MyOwner = MeshComp->GetOwner();
        if (MyOwner)
        {
            UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MyOwner);

            if(ensure(ASC))
            {
                Reset(ASC);
            }
        }
    }

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UANS_GameplayCue::NotifyEnd"));
}

void UANS_GameplayCue::Init(AActor* Owner, USkeletalMeshComponent* MeshComp, UAbilitySystemComponent* ASC)
{
    FGameplayCueParameters Params;
    Params.Location = MeshComp->GetComponentLocation();

    ASC->AddGameplayCue(BeginGameplayCueTagTrigger, Params);
    TrackedGameplayCues.Add(BeginGameplayCueTagTrigger);

    if (UWorld* World = Owner->GetWorld())
    {
        if (ProjectileEffectFactory)
        {
            EffectActor = World->SpawnActor<AActor>(ProjectileEffectFactory);
            EffectActor->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
        }
    }
}

void UANS_GameplayCue::Reset(UAbilitySystemComponent* ASC)
{

    // 사용된 GameplayCue 제거
    for (FGameplayTag& GameplayCueTag : TrackedGameplayCues)
    {
        ASC->RemoveGameplayCue(GameplayCueTag);
    }

    TrackedGameplayCues.Empty();

    // Niagara로 생성된 Effect 제거
    if (EffectActor)
    {
        EffectActor->Destroy();
        EffectActor = nullptr;
    }
}
