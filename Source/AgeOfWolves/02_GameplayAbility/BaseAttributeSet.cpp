// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttributeSet.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogAttributeSet)

TArray<FGameplayAttribute> UBaseAttributeSet::GetAllAttributes() const
{
	TArray<FGameplayAttribute> AllAttributes;

	// 모든 속성을 FGameplayAttribute 객체로 추가
	AllAttributes.Add(GetHealthAttribute());
	AllAttributes.Add(GetMaxHealthAttribute());
	AllAttributes.Add(GetHealthRegenRateAttribute());
	AllAttributes.Add(GetManaAttribute());
	AllAttributes.Add(GetMaxManaAttribute());
	AllAttributes.Add(GetManaRegenRateAttribute());
	AllAttributes.Add(GetStaminaAttribute());
	AllAttributes.Add(GetMaxStaminaAttribute());
	AllAttributes.Add(GetStaminaRegenRateAttribute());
	AllAttributes.Add(GetArmorAttribute());
	AllAttributes.Add(GetMoveSpeedAttribute());
	AllAttributes.Add(GetCharacterLevelAttribute());
	AllAttributes.Add(GetDamageAttribute());
	AllAttributes.Add(GetXPAttribute());
	AllAttributes.Add(GetGoldAttribute());
	AllAttributes.Add(GetXPBountyAttribute());
	AllAttributes.Add(GetGoldBountyAttribute());

	return AllAttributes;
}

UBaseAttributeSet::UBaseAttributeSet()
{}


void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}

}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

}

void UBaseAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
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
