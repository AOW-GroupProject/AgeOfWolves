// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecCalc_Damage.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogExecCalc_Damage)


struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Poise);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Offense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Groggy);
	
	FDamageStatics()
	{
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
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoiseDef);
	RelevantAttributesToCapture.Add(DamageStatics().OffenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().GroggyDef);
}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Source, Target 관련 멤버 변수 설정
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// Source, Target 정보 로그
	UE_LOGFMT(LogExecCalc_Damage, Log, "데미지 계산 시작 - Source: {0}, Target: {1}",
		IsValid(SourceAvatar) ? SourceAvatar->GetName() : TEXT("Invalid"),
		IsValid(TargetAvatar) ? TargetAvatar->GetName() : TEXT("Invalid"));

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.f;

	//@Source의 공격력
	float SourceOffense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().OffenseDef, EvaluationParameters, SourceOffense);
	Damage += SourceOffense;
	UE_LOGFMT(LogExecCalc_Damage, Log, "Source의 공격력: {0}", SourceOffense);

	//@Target의 방어력
	float TargetDefense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters, TargetDefense);
	TargetDefense = FMath::Max<float>(TargetDefense, 0.f);
	UE_LOGFMT(LogExecCalc_Damage, Log, "Target의 방어력: {0}", TargetDefense);

	//@최종 데미지 계산
	Damage -= TargetDefense;
	Damage = FMath::Max<float>(Damage, 0.f);
	UE_LOGFMT(LogExecCalc_Damage, Log, "최종 데미지 계산 결과: {0} (공격력: {1} - 방어력: {2})",
		Damage, SourceOffense, TargetDefense);

	//@Damage 업데이트
	const FGameplayModifierEvaluatedData DamageEvaluatedData(UBaseAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(DamageEvaluatedData);

	UE_LOGFMT(LogExecCalc_Damage, Log, "데미지 계산 완료 - Target {0}에게 {1} 데미지 적용",
		IsValid(TargetAvatar) ? TargetAvatar->GetName() : TEXT("Invalid"), Damage);
}
