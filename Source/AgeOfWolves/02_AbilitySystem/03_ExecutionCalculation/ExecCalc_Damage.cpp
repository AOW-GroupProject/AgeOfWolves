// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecCalc_Damage.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

#include "AbilitySystemComponent.h"

struct FDamageStatics
{
	// 참고 : 1, 2 방법의 매크로 대신 아래와 같은 방법으로도 선언 및 정의 가능
	/*
	FGameplayEffectAttributeCaptureDefinition DamageDef;
	FDamageStatics()
	{
		DamageDef = FGameplayEffectAttributeCaptureDefinition(
		UBaseAttributeSet::GetDamageAttribute(), 
		EGameplayEffectAttributeCaptureSource::Source, 
		true
		);
	}
	*/
	// 1. 데미지 계산시 사용할 Attribute들을 매크로를 통해 선언한다.
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Offense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Groggy);
	
	// Todo : 전투와 관련된 Attribute들에 대해서 Gameplay Tag 추가하여 Map에 등록 및 사용
	// TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	// 2. 사용할 Attribute들을 매크로를 통해 정의한다.
	FDamageStatics()
	{
		// bSnapshot = true인 경우, Attribute가 GameplayEffectSpec이 생성될때, Attribute가 캡처된다.
		// bSnapshot = false인 경우, Attribute는 GameplayEffectSpec이 적용될 때, Attribute가 캡처된다.
		// 예시: 파이어볼을 발사했을 때, 발사를 시작했을때(true) vs 적이 맞았을 때(false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Damage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Poise, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Offense, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Defense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Groggy, Target, false);
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DStatics;
	return DStatics;
}
UExecCalc_Damage::UExecCalc_Damage()
{
	//FDamageStatics에서 정의된 Attribute들을 RelevantAttributesToCapture에 추가한다.
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoiseDef);
	RelevantAttributesToCapture.Add(DamageStatics().OffenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().GroggyDef);
}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 4. 전투 기획에서 작성된 데미지와 관련된 로직은 여기서 코드로 구현된다.

	// Source, Target 관련 멤버 변수 설정
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// ToDo : 추가할 Source, Target 관련 멤버 변수 설정 ...


	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	
	// 계산결과 target에게 적용 될 Damage 변수
	float Damage = 0.f;
	
	float SourceOffense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, SourceOffense);
	Damage += SourceOffense;
	// ToDo : 스킬판정배율을 피해량(Damage)에 반영, 그로기 적용


	float TargetDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, TargetDefense);
	TargetDefense = FMath::Max<float>(TargetDefense, 0.f);
	// ToDo : 방어력이 Damage보다 더 높은 경우 로직 추가?

	Damage -= TargetDefense;
	// 0 이하면 1로 표기
	Damage = FMath::Max<float>(Damage, 1.f);

	const FGameplayModifierEvaluatedData EvaluatedData(UBaseAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
