// Fill out your copyright notice in the Description page of Project Settings.



#include "BaseAttributeSet.h"
#include "Logging/StructuredLog.h"
#include "GameplayEffectExtension.h"
#include "04_Component/BaseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogAttributeSet)

UBaseAttributeSet::UBaseAttributeSet()
{}

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

	ClampAttribute(Attribute, NewValue);
}

void UBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		// 현재 체력이 새로운 최대 체력보다 크지 않도록 조정한다.
		if (GetHealth() > NewValue)
		{
			UBaseAbilitySystemComponent* BaseASC = GetBaseAbilitySystemComponent();
			check(BaseASC);

			BaseASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}


}


void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
			SetDamage(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
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

void UBaseAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Health가 MaxHealth, 0 사이에 있도록 Clamp 한다.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// MaxHealth가 1보다 작아지는 것을 막는다.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}


UBaseAbilitySystemComponent* UBaseAttributeSet::GetBaseAbilitySystemComponent() const
{
	return Cast<UBaseAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
