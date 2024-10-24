// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbilitySet.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogAbilitySet)


void FBaseAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FBaseAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FBaseAbilitySet_GrantedHandles::AddAttributeSet(UBaseAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);

}

UBaseAbilitySet::UBaseAbilitySet(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UBaseAbilitySet::GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent * ASC, FBaseAbilitySet_GrantedHandles * OutGrantedHandles, UObject * SourceObject) const
{
	check(ASC);

	// Attribute Set
	for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
	{
		const FBaseAbilitySet_AttributeSet& SetToGrant = AttributeSets[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOGFMT(LogAbilitySet, Error, "Ability Set의 Attribute Set이 유효하지 않습니다!");
			continue;
		}
		// #1. ASC에 AttributeSet 등록
		UBaseAttributeSet* NewSet = NewObject<UBaseAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);
		// #2. BaseAilitySet에 GrantedHandle 추가
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

void UBaseAbilitySet::GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);

	// GE
	for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
	{
		const FBaseAbilitySet_GameplayEffect& EffectToGrant = GameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOGFMT(LogAbilitySet, Error, "Ability Set의 {0}번째 Gameplay Effect가 유효하지 않습니다!", FString::FromInt(EffectIndex));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}

void UBaseAbilitySet::GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
	{
		const FBaseAbilitySet_GameplayAbility& AbilityToGrant = GameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOGFMT(LogAbilitySet, Error, "Ability Set의 {0}번째 Input Binded Gameplay Ability가 유효하지 않습니다!", FString::FromInt(AbilityIndex));
			continue;
		}
		
		// 1. Ability CDO
		UBaseGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBaseGameplayAbility>();

		// 2. AbilitySpec 구성
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;

		// 3. Active GA는 별도의 Input Tag를 AbilitySpec에 저장합니다. 
		if(AbilityToGrant.bActive && AbilityToGrant.bInputBinded)
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// 4. ASC 등록
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		// 5. Passive GA

		// 6. Handle 저장
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

}
