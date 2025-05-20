#include "ExecCalc_Groggy.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogExecCalc_Groggy)

struct FGroggyStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Groggy);

	FGroggyStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Groggy, Source, false);
	}
};

static const FGroggyStatics& GroggyStatics()
{
	static FGroggyStatics GStatics;
	return GStatics;
}

UExecCalc_Groggy::UExecCalc_Groggy()
{
	RelevantAttributesToCapture.Add(GroggyStatics().GroggyDef);
}

void UExecCalc_Groggy::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Source, Target 관련 멤버 변수 설정
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	UE_LOGFMT(LogExecCalc_Groggy, Log, "그로기 계산 시작 - Source: {0}, Target: {1}",
		IsValid(SourceAvatar) ? SourceAvatar->GetName() : TEXT("Invalid"),
		IsValid(TargetAvatar) ? TargetAvatar->GetName() : TEXT("Invalid"));

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Source의 Groggy 수치 가져오기
	float SourceGroggy = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GroggyStatics().GroggyDef, EvaluationParameters, SourceGroggy);
	UE_LOGFMT(LogExecCalc_Groggy, Log, "Source의 Groggy 수치: {0}", SourceGroggy);

	// Target의 현재 Groggy에 Source의 Groggy 더하기
	const FGameplayModifierEvaluatedData GroggyEvaluatedData(UBaseAttributeSet::GetGroggyAttribute(), EGameplayModOp::Additive, SourceGroggy);
	OutExecutionOutput.AddOutputModifier(GroggyEvaluatedData);

	UE_LOGFMT(LogExecCalc_Groggy, Log, "그로기 계산 완료 - Target {0}에게 그로기 {1} 적용",
		IsValid(TargetAvatar) ? TargetAvatar->GetName() : TEXT("Invalid"),
		SourceGroggy);
}