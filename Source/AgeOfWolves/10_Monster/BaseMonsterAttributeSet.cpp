// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAttributeSet.h"
#include "10_Monster/MonsterData.h"

UBaseMonsterAttributeSet::UBaseMonsterAttributeSet()
{
	
}

void UBaseMonsterAttributeSet::InitializeAttributeValue(struct FSingleMonsterData& SingleMonsterData)
{
	InitMaxHealth(SingleMonsterData.PermanentStat.PHealthStat);
	InitHealth(SingleMonsterData.PermanentStat.PHealthStat);
	InitDamage(SingleMonsterData.PermanentStat.PDamageStat);
	InitOffense(10);

}

TArray<FGameplayAttribute> UBaseMonsterAttributeSet::GetAllAttributes() const
{
	TArray<FGameplayAttribute> AllAttributes;
	AllAttributes.Add(GetHealthAttribute());
	AllAttributes.Add(GetMaxHealthAttribute());
	AllAttributes.Add(GetDamageAttribute());

	return AllAttributes;
}

void UBaseMonsterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UBaseMonsterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UBaseMonsterAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}
