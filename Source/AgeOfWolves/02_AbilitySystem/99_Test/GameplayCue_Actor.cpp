
#include "GameplayCue_Actor.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AGameplayCue_Actor::AGameplayCue_Actor()
{
    bLoopingEffectsRemoved = false;
}

void AGameplayCue_Actor::AddApplicationEffects()
{
    DefaultPlacementInfo.SocketName = AttachSocketName;
    DefaultPlacementInfo.AttachPolicy = EGameplayCueNotify_AttachPolicy::AttachToTarget;

    Recycle();
}

void AGameplayCue_Actor::SpawnBurstEffects()
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        if (USkeletalMeshComponent* SkelMesh = Character->GetMesh())
        {

            if (NiagaraComponent)
            {
                // ������ �Ȱ��� ���̾ư��� ����ߴٸ� Activate �ٷ� ����
                if (NiagaraComponent->GetFXSystemAsset() == NiagaraEffectTemplate)
                {
                    NiagaraComponent->Activate(true);
                }
                
            }
            else
            {
                // ������ �ٸ� ���̰��� ����ߴٸ� AttachScoketName�� �ٿ���
                FVector SpawnLocation = SkelMesh->GetSocketLocation(AttachSocketName);
                FRotator SpawnRotation = SkelMesh->GetSocketRotation(AttachSocketName);
                FVector SpawnScale = SkelMesh->GetRelativeScale3D();


                NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraEffectTemplate, SkelMesh, AttachSocketName,
                    FVector::ZeroVector, FRotator::ZeroRotator, SpawnScale, EAttachLocation::SnapToTarget, false, ENCPoolMethod::None, true);
            }
        }        
    }
}

void AGameplayCue_Actor::StopBurstEffects()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate();
    }
}

