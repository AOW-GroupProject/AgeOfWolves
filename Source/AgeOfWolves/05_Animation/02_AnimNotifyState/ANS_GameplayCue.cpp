// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_GameplayCue.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "01_Character/01_Spell/UseAttachedMeshInterface.h"
#include "01_Character/PlayerCharacter.h"

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
                // Interface�� �̿��� Weapon�� Mesh�� �����´�
                IUseAttachedMeshInterface* UseAttachActor = Cast<IUseAttachedMeshInterface>(MyOwner);
                USkeletalMeshComponent* OwnerMeshComp = (UseAttachActor && bAttachToWeapon) ? UseAttachActor->GetWeaponSkeletalMeshComponent() : MeshComp;

                // �޽��� �����Ѵٸ�?
                if (OwnerMeshComp)
                {
                    // �ʱ�ȭ �����ش�
                    Init(MyOwner, OwnerMeshComp, ASC);
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

            if(ASC)
            {
                Reset(ASC);
            }

            if (APlayerCharacter* Character = Cast<APlayerCharacter>(MyOwner))
            {
                Character->StopBurstParticleActor(SocketName);
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
        if (APlayerCharacter* Character = Cast<APlayerCharacter>(Owner))
        {
            Character->PlayBurstParticleActor(SocketName, Niagara);
        }

        //if (ProjectileEffectFactory)
        //{
        //    EffectActor = World->SpawnActor<AActor>(ProjectileEffectFactory);
        //    EffectActor->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
        //}
    }
}

void UANS_GameplayCue::Reset(UAbilitySystemComponent* ASC)
{

    // ���� GameplayCue ����
    for (FGameplayTag& GameplayCueTag : TrackedGameplayCues)
    {
        ASC->RemoveGameplayCue(GameplayCueTag);
    }


    TrackedGameplayCues.Empty();

    // Niagara�� ������ Effect ����
    if (EffectActor)
    {
        EffectActor->Destroy();
        EffectActor = nullptr;
    }
}
