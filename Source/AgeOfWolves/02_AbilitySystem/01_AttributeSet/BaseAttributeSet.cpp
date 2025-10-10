#include "BaseAttributeSet.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(LogAttributeSet)

//@Defualt Setting
#pragma region Default Setting
UBaseAttributeSet::UBaseAttributeSet()
{
	// ✅ 추가: ManaStack 초기화 (기본값 0)
	ManaStack = 0.f;

	// ✅ 추가: MaxManaStack 초기화 (기본값 설정, 예: 10)
	MaxManaStack = 3.f;
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
	// ✅ 추가: MaxManaStack 변경 시 ManaStack 조정
	else if (Attribute == GetMaxManaStackAttribute())
	{
		// MaxManaStack이 변경될 때 현재 ManaStack이 새로운 최대값을 초과하지 않도록 조정
		AdjustAttributeForMaxChange(ManaStack, MaxManaStack, NewValue, GetManaStackAttribute());
	}
	// ✅ 추가: ManaStack 클램핑 (0 ~ MaxManaStack)
	else if (Attribute == GetManaStackAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.f, GetMaxManaStack());
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

	// ✅ 추가: ManaStack 처리 - MaxManaStack 도달 시 Mana 증가 및 스택 초기화
	if (Data.EvaluatedData.Attribute == GetManaStackAttribute())
	{
		// 현재 ManaStack 클램핑
		SetManaStack(FMath::Clamp(GetManaStack(), 0.f, GetMaxManaStack()));

		// MaxManaStack에 도달했는지 확인
		if (GetManaStack() >= GetMaxManaStack())
		{
			UE_LOGFMT(LogAttributeSet, Log, "마나 스택 최대치 도달! Mana +1, 스택 초기화");

			// ManaStack 초기화
			SetManaStack(0.f);

			// Mana 1 증가 (MaxMana를 초과하지 않도록)
			float NewMana = FMath::Min(GetMana() + 1.f, GetMaxMana());
			SetMana(NewMana);

			UE_LOGFMT(LogAttributeSet, Log, "현재 Mana: {0}/{1}, ManaStack: {2}/{3}",
				GetMana(), GetMaxMana(), GetManaStack(), GetMaxManaStack());
		}
	}
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
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
{}
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
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
	// ✅ 추가: ManaStack 관련 Attribute
	AllAttributes.Add(GetManaStackAttribute());
	AllAttributes.Add(GetMaxManaStackAttribute());

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
	AllAttributes.Add(GetXPBountyAttribute());

	AllAttributes.Add(GetGoldAttribute());
	AllAttributes.Add(GetGoldBountyAttribute());

	AllAttributes.Add(GetCombatStateAttribute());

	AllAttributes.Add(GetAlertLevelAttribute());

	return AllAttributes;
}
#pragma endregion