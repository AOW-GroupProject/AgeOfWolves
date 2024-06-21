// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttributeSet.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogAttributeSet)

UBaseAttributeSet::UBaseAttributeSet()
{}

TArray<FGameplayAttribute> UBaseAttributeSet::GetAllAttributes() const
{
	TArray<FGameplayAttribute> AllAttributes;

	// ��� �Ӽ��� FGameplayAttribute ��ü�� �߰�
	AllAttributes.Add(GetHealthAttribute());
	AllAttributes.Add(GetMaxHealthAttribute());
	AllAttributes.Add(GetHealthRegenRateAttribute());

	AllAttributes.Add(GetManaAttribute());
	AllAttributes.Add(GetMaxManaAttribute());
	AllAttributes.Add(GetManaRegenRateAttribute());

	AllAttributes.Add(GetStaminaAttribute());
	AllAttributes.Add(GetMaxStaminaAttribute());
	AllAttributes.Add(GetStaminaRegenRateAttribute());

	AllAttributes.Add(GetPoiseAttribute());
	AllAttributes.Add(GetDefenseAttribute());
	AllAttributes.Add(GetOffenseAttribute());
	AllAttributes.Add(GetMoveSpeedAttribute());
	AllAttributes.Add(GetCharacterLevelAttribute());

	AllAttributes.Add(GetDamageAttribute());

	AllAttributes.Add(GetSealPointAttribute());
	AllAttributes.Add(GetGroggyAttribute());

	AllAttributes.Add(GetXPAttribute());
	AllAttributes.Add(GetGoldAttribute());
	AllAttributes.Add(GetXPBountyAttribute());
	AllAttributes.Add(GetGoldBountyAttribute());

	return AllAttributes;
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
<<<<<<< HEAD:Source/AgeOfWolves/02_AbilitySystem/01_AttributeSet/BaseAttributeSet.cpp

	// @Max HP
=======
<<<<<<< HEAD:Source/AgeOfWolves/02_AbilitySystem/01_AttributeSet/BaseAttributeSet.cpp

	// @Max HP
=======
	
>>>>>>> develop:Source/AgeOfWolves/02_GameplayAbility/BaseAttributeSet.cpp
>>>>>>> develop:Source/AgeOfWolves/02_GameplayAbility/BaseAttributeSet.cpp
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	// @Max MP
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	// @Max SP
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	// @Move Speed
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

void UBaseAttributeSet::AdjustAttributeForCurrentChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
}
