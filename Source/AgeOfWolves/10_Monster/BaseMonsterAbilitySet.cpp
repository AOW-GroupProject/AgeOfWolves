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

		//에디터에서 할당된 사용할 AttributeSet 클래스들 전부 객체를 생성하고 컴포넌트에 넣기.
		if (!IsValid(SetToGrant.AttributeSet))
		{
			//UE_LOGFMT(LogMonsterAbilitySet, Error, "Monster Ability Set의 Attribute Set이 유효하지 않습니다!");
			continue;
		}
		// #1. ASC에 AttributeSet 등록
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
			//UE_LOGFMT(LogMonsterAbilitySet, Error, "Monster Ability Set의 {0}번째 Gameplay Effect가 유효하지 않습니다!", FString::FromInt(EffectIndex));
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
			//UE_LOGFMT(LogMonsterAbilitySet, Error, "Monster Ability Set의 {0}번째 Gameplay Ability가 유효하지 않습니다!", FString::FromInt(AbilityIndex));
			continue;
		}

		// 1. Ability CDO
		UBaseGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBaseGameplayAbility>();

		// 2. AbilitySpec 구성
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;

		// 3. Active GA는 별도의 Input Tag를 AbilitySpec에 저장합니다. 
		if (AbilityToGrant.IsActive)
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// 4. ASC 등록
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		// 5. Passive GA

		
	}
}
