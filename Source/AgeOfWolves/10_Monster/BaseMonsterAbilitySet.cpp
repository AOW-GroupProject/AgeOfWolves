// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAbilitySet.h"
#include "10_Monster/BaseMonsterAttributeSet.h"
#include "10_Monster/BaseMonsterASC.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"
#include "Logging/StructuredLog.h"



void UBaseMonsterAbilitySet::GiveStartupAttributeSetToAbilitySystem(UBaseMonsterASC* ASC, UObject* SourceObject) const
{
	check(ASC);

	// Attribute Set
	for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
	{
		const FBaseMonsterAbilitySet_AttributeSet& SetToGrant = AttributeSets[SetIndex];

		//�����Ϳ��� �Ҵ�� ����� AttributeSet Ŭ������ ���� ��ü�� �����ϰ� ������Ʈ�� �ֱ�.
		if (!IsValid(SetToGrant.AttributeSet))
		{
			//UE_LOGFMT(LogMonsterAbilitySet, Error, "Monster Ability Set�� Attribute Set�� ��ȿ���� �ʽ��ϴ�!");
			continue;
		}
		// #1. ASC�� AttributeSet ���
		UBaseMonsterAttributeSet* NewSet = NewObject<UBaseMonsterAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);
	}
}

void UBaseMonsterAbilitySet::GiveStartupGameplayEffectToAbilitySystem(UBaseMonsterASC* ASC, UObject* SourceObject) const
{
	check(ASC);

	// GE
	for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
	{
		const FBaseMonsterAbilitySet_GameplayEffect& EffectToGrant = GameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			//UE_LOGFMT(LogMonsterAbilitySet, Error, "Monster Ability Set�� {0}��° Gameplay Effect�� ��ȿ���� �ʽ��ϴ�!", FString::FromInt(EffectIndex));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

	}
}

void UBaseMonsterAbilitySet::GiveStartupGameplayAbilityToAbilitySystem(UBaseMonsterASC* ASC, UObject* SourceObject) const
{
	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
	{
		const FBaseMonsterAbilitySet_GameplayAbility& AbilityToGrant = GameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			//UE_LOGFMT(LogMonsterAbilitySet, Error, "Monster Ability Set�� {0}��° Gameplay Ability�� ��ȿ���� �ʽ��ϴ�!", FString::FromInt(AbilityIndex));
			continue;
		}

		// 1. Ability CDO
		UBaseGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBaseGameplayAbility>();

		// 2. AbilitySpec ����
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;

		// 3. Active GA�� ������ Input Tag�� AbilitySpec�� �����մϴ�. 
		if (AbilityToGrant.IsActive)
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// 4. ASC ���
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		// 5. Passive GA

		
	}
}
