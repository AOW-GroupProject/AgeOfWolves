#include "AN_ApplyGEDuringAnimPlaying.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "01_Character/PlayerCharacter.h"

DEFINE_LOG_CATEGORY(LogAN_ApplyGEDuringAnimPlaying)


void UAN_ApplyGEDuringAnimPlaying::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@Mesh, GE
    if (!MeshComp || !ApplyGEClass)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "MeshComp �Ǵ� GE Ŭ������ ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    //@Owner
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "Owner�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    //@Player Character
    APlayerCharacter* PC = Cast<APlayerCharacter>(Owner);
    if (!PC)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "Player Character ĳ���� ����.");
        return;
    }

    //@ASC
    UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "ASC�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    //@GEContextHandle
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(ApplyGEClass, 1, EffectContext);

    if (!EffectSpec.IsValid())
    {
        return;
    }

    //@Apply GE
    ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
    UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Log, "GE({0})�� ����Ǿ����ϴ�.", *ApplyGEClass->GetName());
}
