// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackGameplayAbility.h"

#include "01_Character/PlayerCharacter.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


UAttackGameplayAbility::UAttackGameplayAbility()
{

}

void UAttackGameplayAbility::CauseDamageToTarget(AActor* TargetActor, FGameplayTag HitDirectionTag)
{
    if (IsValid(ApplyGameplayEffectClass))
    {
        FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(ApplyGameplayEffectClass, GetAbilityLevel());
        if (HitDirectionTag.MatchesTag(AOWGameplayTags::TAG_EventTag_HitReact))
        {
            DamageSpecHandle.Data.Get()->AddDynamicAssetTag(HitDirectionTag);
        }
        for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
        {
            const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
        }
        GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
    }
}

FGameplayTag UAttackGameplayAbility::CalculateHitDirection(const FVector& HitLocation, const AActor* HitActor)
{
    if (!IsValid(HitActor)) return FGameplayTag(); 

    FVector ActorLocation = HitActor->GetActorLocation();
    FVector ForwardVector = HitActor->GetActorForwardVector();
    FVector RightVector = HitActor->GetActorRightVector();

    FVector ToHit = (HitLocation - ActorLocation);
    ToHit.Z = 0;
    ToHit = ToHit.GetSafeNormal();

    float ForwardDot = FVector::DotProduct(ForwardVector, ToHit); 
    float RightDot = FVector::DotProduct(RightVector, ToHit);
    const float Threshold = FMath::Cos(FMath::DegreesToRadians(45.f));

    if (ForwardDot >= Threshold && FMath::Abs(RightDot) < Threshold) // Forward
    {
        return AOWGameplayTags::TAG_EventTag_HitReact_Forward;
    }
    else if (-ForwardDot > Threshold && FMath::Abs(RightDot) < Threshold) // Backward
    {
        return AOWGameplayTags::TAG_EventTag_HitReact_Backward;
    }
    else if (RightDot >= Threshold && FMath::Abs(ForwardDot) < Threshold) // Right
    {
        return AOWGameplayTags::TAG_EventTag_HitReact_Right;
    }
    else if (-RightDot > Threshold && FMath::Abs(ForwardDot) < Threshold) // Left
    {
        return AOWGameplayTags::TAG_EventTag_HitReact_Left;
    }

    return FGameplayTag();
}


APlayerCharacter* UAttackGameplayAbility::GetPlayerCharacterFromActorInfo() const
{
	return Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
}
