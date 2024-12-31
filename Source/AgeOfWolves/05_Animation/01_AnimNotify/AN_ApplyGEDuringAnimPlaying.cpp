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
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "MeshComp 또는 GE 클래스가 유효하지 않습니다.");
        return;
    }

    //@Owner
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "Owner가 유효하지 않습니다.");
        return;
    }

    //@Player Character
    APlayerCharacter* PC = Cast<APlayerCharacter>(Owner);
    if (!PC)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "Player Character 캐스팅 실패.");
        return;
    }

    //@ASC
    UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "ASC가 유효하지 않습니다.");
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
    UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Log, "GE({0})가 적용되었습니다.", *ApplyGEClass->GetName());
}
