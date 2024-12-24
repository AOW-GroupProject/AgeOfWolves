// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(LogAttackGA)

//@Defualt Setting
#pragma region Default Setting
UAttackGameplayAbility::UAttackGameplayAbility(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAttackGameplayAbility::SendDamageEvent(const FHitResult& HitResult)
{
    //@Hit Actor
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: HitActor가 유효하지 않음");
        return;
    }

    //@Source Actor
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    //@Source ASC
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceActor || !SourceASC)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: Source Actor 또는 ASC가 유효하지 않음");
        return;
    }

    //@Event Data
    FGameplayEventData Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag("EventTag.OnDamaged");
    Payload.Instigator = SourceActor;
    Payload.Target = HitActor;
    Payload.OptionalObject = GetApplyGameplayEffectClass().GetDefaultObject();

    //@Context Handle
    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.AddHitResult(HitResult);
    Payload.ContextHandle = EffectContext;

    //@Target ASC
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
    if (!TargetASC)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - Target: {0}, 사유: 타겟의 ASC가 유효하지 않음",
            HitActor->GetName());
        return;
    }

    //@Handle Gameplay Event
    if (!TargetASC->HandleGameplayEvent(Payload.EventTag, &Payload))
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - Target: {0}, 사유: HandleGameplayEvent 호출 실패",
            HitActor->GetName());
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "데미지 이벤트 전송 완료 - Target: {0}, Instigator: {1}, Impact Location: {2}",
        HitActor->GetName(), SourceActor->GetName(), HitResult.ImpactPoint.ToString());
}

APlayerCharacter* UAttackGameplayAbility::GetPlayerCharacterFromActorInfo() const
{
    return Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion