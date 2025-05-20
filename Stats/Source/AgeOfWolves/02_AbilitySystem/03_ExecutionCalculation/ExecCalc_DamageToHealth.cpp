#include "ExecCalc_DamageToHealth.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogExecCalc_DamageToHealth)

struct FDamageToHealthStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	FDamageToHealthStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Damage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Health, Source, false);
	}
};

static const FDamageToHealthStatics& DamageToHealthStatics()
{
	static FDamageToHealthStatics DToHStatics;
	return DToHStatics;
}

UExecCalc_DamageToHealth::UExecCalc_DamageToHealth()
{
	RelevantAttributesToCapture.Add(DamageToHealthStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageToHealthStatics().DamageDef);
}

void UExecCalc_DamageToHealth::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // Source, Target 관련 멤버 변수 설정
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    // Source, Target 정보 로그
    UE_LOGFMT(LogExecCalc_DamageToHealth, Log, "체력 감소 계산 시작 - Source: {0}, Target: {1}",
        IsValid(SourceAvatar) ? SourceAvatar->GetName() : TEXT("Invalid"),
        IsValid(TargetAvatar) ? TargetAvatar->GetName() : TEXT("Invalid"));

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = SourceTags;

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float Damage = 0.f;
    float Health = 0.f;

    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageToHealthStatics().DamageDef, EvaluationParameters, Damage);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageToHealthStatics().HealthDef, EvaluationParameters, Health);

    UE_LOGFMT(LogExecCalc_DamageToHealth, Log, "현재 상태 - 체력: {0}, 받은 데미지: {1}", Health, Damage);

    float NewHealth = Health - Damage;

    UE_LOGFMT(LogExecCalc_DamageToHealth, Log, "체력 감소 계산 완료 - 최종 체력: {0} (이전 체력: {1} - 데미지: {2})",
        NewHealth, Health, Damage);

    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageToHealthStatics().HealthProperty, EGameplayModOp::Override, NewHealth));
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageToHealthStatics().DamageProperty, EGameplayModOp::Additive, -Damage));

    UE_LOGFMT(LogExecCalc_DamageToHealth, Log, "체력 속성 업데이트 완료");
}