// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttributeSet.h"
#include "Logging/StructuredLog.h"

#include "GameplayEffectExtension.h"


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

	// @Max HP
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}

	float MinimumHealth = 0.f;
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalIncomingDamage = GetDamage();
		SetDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				// ToDo : 죽음 관련 로직 처리
			}
			else
			{
				// ToDo : 피격 관련 로직 처리
			}
			// ToDo : 그 외 로직 처리
		}
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

void UBaseAttributeSet::AdjustAttributeForCurrentChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
}
