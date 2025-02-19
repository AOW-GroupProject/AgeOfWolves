#include "BaseAttributeSet.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"

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

	AllAttributes.Add(GetCombatStateAttribute());

	return AllAttributes;
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//@Max Health
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	//@Max MP
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	//@Max SP
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());

		UE_LOGFMT(LogAttributeSet, Error, "{0} : Stamina", NewValue);
	}
	//@Move Speed
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}
	// @CombatState - 0(비전투), 1(전투), 2(발도 준비) 상태만 가질 수 있도록 클램핑
	else if (Attribute == GetCombatStateAttribute())
	{
		NewValue = FMath::Clamp<float>(FMath::RoundToFloat(NewValue), 0.f, 4.f);
		UE_LOGFMT(LogAttributeSet, Log, "전투 상태 변경: {0}",
			NewValue == 0.f ? TEXT("비전투") :
			NewValue == 1.f ? TEXT("전투") :
			NewValue == 2.f ? TEXT("전투-발도") :
			NewValue == 3.f ? TEXT("전투-가드") :
			TEXT("전투-반대 가드"));
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
