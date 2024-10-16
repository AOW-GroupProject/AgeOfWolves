// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackGameplayAbility.h"


#include "01_Character/PlayerCharacter.h"
#include "04_Component/BaseAbilitySystemComponent.h"


#include "AbilitySystemBlueprintLibrary.h"


UAttackGameplayAbility::UAttackGameplayAbility()
{

}

void UAttackGameplayAbility::CauseDamageToTarget(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(ApplyGameplayEffectClass, GetAbilityLevel());
	for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
		
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	UE_LOG(LogTemp, Log, TEXT("CauseDamageEffect"));
}


APlayerCharacter* UAttackGameplayAbility::GetPlayerCharacterFromActorInfo() const
{
	return Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
}
