#include "AN_ApplyGEDuringAnimPlaying.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "01_Character/CharacterBase.h"

#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogAN_ApplyGEDuringAnimPlaying)

void UAN_ApplyGEDuringAnimPlaying::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@Mesh Check
    if (!MeshComp)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "MeshComp가 유효하지 않습니다.");
        return;
    }

    //@Owner & Character Base
    AActor* Owner = MeshComp->GetOwner();
    ACharacterBase* CharacterBase = Cast<ACharacterBase>(Owner);
    if (!CharacterBase)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "Character Base 캐스팅 실패.");
        return;
    }

    //@ASC
    UAbilitySystemComponent* ASC = CharacterBase->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "ASC가 유효하지 않습니다.");
        return;
    }

    //@Current Animating Ability
    UGameplayAbility* CurrentAbility = ASC->GetAnimatingAbility();
    if (!CurrentAbility)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "현재 실행 중인 어빌리티가 없습니다.");
        return;
    }

    auto BaseAbility = Cast<UBaseGameplayAbility>(CurrentAbility);
    if (!BaseAbility)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "어빌리티가 Base Gaemplay Ability 유형으로 캐스팅하는데 실패했습니다.");
        return;
    }

    //@Get GE Class from Ability
    TSubclassOf<UGameplayEffect> GEClass = BaseAbility->GetApplyGameplayEffectClass();
    if (!GEClass)
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "어빌리티에서 GE 클래스를 가져올 수 없습니다.");
        return;
    }

    //@GEContextHandle
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(GEClass, 1, EffectContext);

    if (!EffectSpec.IsValid())
    {
        UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Error, "유효하지 않은 Effect Spec입니다.");
        return;
    }

    //@Apply GE
    ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
    UE_LOGFMT(LogAN_ApplyGEDuringAnimPlaying, Log, "GE({0})가 적용되었습니다.", *GEClass->GetName());
}