#include "AN_NotifyAbilityTiming.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogAN_NotifyAbilityTiming)

void UAN_NotifyAbilityTiming::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@Mesh Check
    if (!MeshComp)
    {
        UE_LOGFMT(LogAN_NotifyAbilityTiming, Error, "MeshComp가 유효하지 않습니다.");
        return;
    }

    //@Owner & Character Base
    AActor* Owner = MeshComp->GetOwner();
    ACharacterBase* CharacterBase = Cast<ACharacterBase>(Owner);
    if (!CharacterBase)
    {
        UE_LOGFMT(LogAN_NotifyAbilityTiming, Error, "Character Base 캐스팅 실패.");
        return;
    }

    //@ASC
    UAbilitySystemComponent* ASC = CharacterBase->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogAN_NotifyAbilityTiming, Error, "ASC가 유효하지 않습니다.");
        return;
    }

    //@Current Ability
    UBaseGameplayAbility* AnimatingAbility = Cast<UBaseGameplayAbility>(ASC->GetAnimatingAbility());
    if (!AnimatingAbility)
    {
        UE_LOGFMT(LogAN_NotifyAbilityTiming, Warning, "Notify 실패 - 사유: 실행 중인 어빌리티가 유효하지 않음");
        return;
    }

    //@Timing 함수 호출
    AnimatingAbility->TimingNotifiedByAN.Broadcast();

    UE_LOGFMT(LogAN_NotifyAbilityTiming, Log, "Notify 성공 - Character: {0}, Ability: {1}",
        *CharacterBase->GetName(), *AnimatingAbility->GetName());
}