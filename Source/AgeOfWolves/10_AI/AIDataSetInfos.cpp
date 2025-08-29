#include "AIDataSetInfos.h"
#include "Logging/StructuredLog.h"

#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDeviceRedirector.h"

#include "BehaviorTree/BehaviorTree.h"

DEFINE_LOG_CATEGORY(LogAIDataSetInfos)
DEFINE_LOG_CATEGORY(LogAILearning)

//@Structs
#pragma region FAISimplifiedPersonality
/**
 * @brief 특정 행동 카테고리에 대한 초기 선호도를 계산합니다.
 * @param ActionCategory 행동 카테고리 태그
 * @return 계산된 초기 선호도 (0.0f ~ 100.0f)
 */
    float FAISimplifiedPersonality::GetInitialActionPreference(const FGameplayTag& ActionCategory) const
{
    float InitialPreference = 50.0f; // 기본값

    UE_LOG(LogAILearning, VeryVerbose, TEXT("GetInitialActionPreference - 입력 카테고리: %s"), *ActionCategory.ToString());
    UE_LOG(LogAILearning, VeryVerbose, TEXT("GetInitialActionPreference - 현재 성향 (공격성: %.2f, 기민함: %.2f)"), Aggressiveness, Agility);

    if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"))))
    {
        InitialPreference = (Aggressiveness + Agility) * 0.5f; // 균등 가중치
        UE_LOG(LogAILearning, Log, TEXT("OpeningSkills 초기 선호도 계산: (%.2f + %.2f) * 0.5 = %.2f"),
            Aggressiveness, Agility, InitialPreference);
    }
    else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"))))
    {
        InitialPreference = Aggressiveness * 0.8f + Agility * 0.2f; // 공격성 위주
        UE_LOG(LogAILearning, Log, TEXT("Skills 초기 선호도 계산: %.2f * 0.8 + %.2f * 0.2 = %.2f"),
            Aggressiveness, Agility, InitialPreference);
    }
    else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))))
    {
        InitialPreference = Agility * 0.8f + Aggressiveness * 0.2f; // 기민함 위주
        UE_LOG(LogAILearning, Log, TEXT("StrafeOrDodge 초기 선호도 계산: %.2f * 0.8 + %.2f * 0.2 = %.2f"),
            Agility, Aggressiveness, InitialPreference);
    }
    else
    {
        UE_LOG(LogAILearning, Warning, TEXT("알 수 없는 행동 카테고리 '%s' - 기본값 %.2f 사용"),
            *ActionCategory.ToString(), InitialPreference);
    }

    UE_LOG(LogAILearning, VeryVerbose, TEXT("GetInitialActionPreference 결과: %.2f"), InitialPreference);
    return InitialPreference;
}
#pragma endregion

#pragma region FAIRewardCalculator
/**
 * @brief 정규화된 가중합 보상을 계산합니다.
 * @param DamageReceived 받은 데미지
 * @param CurrentDistance 현재 거리
 * @param bAbilitySuccess 어빌리티 성공 여부
 * @return 정규화된 보상값 (-1.0f ~ 1.0f)
 */
float FAIRewardCalculator::CalculateNormalizedReward(float DamageReceived, float CurrentDistance, bool bAbilitySuccess) const
{
    UE_LOG(LogAILearning, Log, TEXT("표준 보상 계산 시작 - 데미지: %.2f, 거리: %.2f, 성공: %s"),
        DamageReceived, CurrentDistance, bAbilitySuccess ? TEXT("True") : TEXT("False"));

    // 1. 데미지 보상 정규화 (받은 데미지가 많을수록 음의 보상)
    float NormalizedDamage = -FMath::Clamp(DamageReceived / 100.0f, 0.0f, 1.0f);
    UE_LOG(LogAILearning, VeryVerbose, TEXT("데미지 정규화: %.2f → %.3f"), DamageReceived, NormalizedDamage);

    // 2. 거리 보상 정규화 (근접 유형: MinAttackRange에 가까울수록 좋음)
    float DistanceDiff = FMath::Abs(CurrentDistance - OptimalCombatDistance);
    float MaxAcceptableDistance = OptimalCombatDistance * 2.0f; // 허용 범위
    float NormalizedDistance = FMath::Max(0.0f, 1.0f - (DistanceDiff / MaxAcceptableDistance));

    UE_LOG(LogAILearning, Log, TEXT("거리 정규화: 현재=%.2f, 최적=%.2f, 차이=%.2f → %.3f"),
        CurrentDistance, OptimalCombatDistance, DistanceDiff, NormalizedDistance);

    // 3. 성공 보상 (이진값)
    float NormalizedSuccess = bAbilitySuccess ? 1.0f : -1.0f;
    UE_LOG(LogAILearning, VeryVerbose, TEXT("성공 정규화: %s → %.3f"),
        bAbilitySuccess ? TEXT("Success") : TEXT("Fail"), NormalizedSuccess);

    // === 고정 가중치 사용 (제한사항 준수) ===
    float WeightedSum = (NormalizedDamage * FIXED_DAMAGE_WEIGHT) +
        (NormalizedDistance * FIXED_DISTANCE_WEIGHT) +
        (NormalizedSuccess * FIXED_SUCCESS_WEIGHT);

    float FinalReward = FMath::Clamp(WeightedSum / FIXED_TOTAL_WEIGHT, -1.0f, 1.0f);

    UE_LOG(LogAILearning, Log, TEXT("표준 보상 계산 완료 - 가중합: %.3f, 고정분모: %.3f, 최종보상: %.3f"),
        WeightedSum, FIXED_TOTAL_WEIGHT, FinalReward);

    if (FinalReward > 0.5f)
    {
        UE_LOG(LogAILearning, Log, TEXT("높은 양의 보상 달성! (%.3f)"), FinalReward);
    }
    else if (FinalReward < -0.5f)
    {
        UE_LOG(LogAILearning, Log, TEXT("높은 음의 보상 발생 (%.3f) - 행동 패턴 조정 필요"), FinalReward);
    }

    return FinalReward;
}

/**
 * @brief 적응적 학습률을 계산합니다.
 * @param LearningCount 현재 학습 횟수
 * @return 계산된 학습률
 */
float FAIRewardCalculator::CalculateAdaptiveLearningRate(int32 LearningCount) const
{
    UE_LOG(LogAILearning, VeryVerbose, TEXT("학습률 계산 - 학습횟수: %d, 기본학습률: %.3f, 감쇠인수: %.3f"),
        LearningCount, BaseLearningRate, LearningDecayFactor);

    float AdaptiveRate = BaseLearningRate * FMath::Pow(LearningDecayFactor, LearningCount / 10.0f);
    float ClampedRate = FMath::Clamp(AdaptiveRate, 0.01f, 0.15f);

    if (AdaptiveRate != ClampedRate)
    {
        UE_LOG(LogAILearning, Log, TEXT("학습률 클램핑: %.3f → %.3f (최소/최대 제한 적용)"),
            AdaptiveRate, ClampedRate);
    }

    UE_LOG(LogAILearning, VeryVerbose, TEXT("적응적 학습률 계산 결과: %.3f"), ClampedRate);

    // 학습률이 매우 낮아졌을 때 경고
    if (ClampedRate <= 0.02f)
    {
        UE_LOG(LogAILearning, Warning, TEXT("학습률이 매우 낮음 (%.3f) - 학습 효과 미미할 수 있음"), ClampedRate);
    }

    return ClampedRate;
}
#pragma endregion

#pragma region FAIExperienceWeightConfig
/**
 * @brief 경험 가중치 범위 내에서 변화량을 제한합니다.
 * @param CurrentWeight 현재 가중치
 * @param InitialWeight 초기 가중치
 * @param ProposedChange 제안된 변화량
 * @return 제한된 새로운 가중치
 */
float FAIExperienceWeightConfig::ClampWeightChange(float CurrentWeight, float InitialWeight, float ProposedChange) const
{
    UE_LOG(LogAILearning, VeryVerbose, TEXT("가중치 변화 제한 - 현재: %.3f, 초기: %.3f, 제안변화: %.3f"),
        CurrentWeight, InitialWeight, ProposedChange);

    float NewWeight = CurrentWeight + ProposedChange;
    float MaxAllowed = InitialWeight + MaxWeightChange;
    float MinAllowed = InitialWeight - MaxWeightChange;

    float ClampedWeight = FMath::Clamp(NewWeight, MinAllowed, MaxAllowed);

    if (NewWeight != ClampedWeight)
    {
        UE_LOG(LogAILearning, Log, TEXT("가중치 제한 적용: %.3f → %.3f (허용범위: %.3f ~ %.3f)"),
            NewWeight, ClampedWeight, MinAllowed, MaxAllowed);
    }

    // 극한값에 도달했을 때 경고
    if (FMath::IsNearlyEqual(ClampedWeight, MaxAllowed, 0.01f))
    {
        UE_LOG(LogAILearning, Warning, TEXT("최대 가중치 한계에 도달: %.3f"), ClampedWeight);
    }
    else if (FMath::IsNearlyEqual(ClampedWeight, MinAllowed, 0.01f))
    {
        UE_LOG(LogAILearning, Warning, TEXT("최소 가중치 한계에 도달: %.3f"), ClampedWeight);
    }

    return ClampedWeight;
}

/**
 * @brief 단일 변화량을 제한합니다.
 * @param ProposedChange 제안된 변화량
 * @return 제한된 변화량
 */
float FAIExperienceWeightConfig::ClampSingleChange(float ProposedChange) const
{
    float ClampedChange = FMath::Clamp(ProposedChange, -MinSingleChange, MinSingleChange);

    if (ProposedChange != ClampedChange)
    {
        UE_LOG(LogAILearning, VeryVerbose, TEXT("단일 변화량 제한: %.4f → %.4f"), ProposedChange, ClampedChange);
    }

    return ClampedChange;
}

/**
 * @brief 카테고리 확률을 정규화하고 최소값을 보장합니다.
 * @param Probabilities 확률 배열 (참조로 수정됨)
 */
void FAIExperienceWeightConfig::NormalizeCategoryProbabilities(TArray<float>& Probabilities) const
{
    int32 NumCategories = Probabilities.Num();
    if (NumCategories == 0)
    {
        UE_LOG(LogAILearning, Warning, TEXT("확률 정규화: 빈 배열"));
        return;
    }

    UE_LOG(LogAILearning, VeryVerbose, TEXT("확률 정규화 시작 - 카테고리 수: %d"), NumCategories);

    // 정규화 전 로그
    for (int32 i = 0; i < NumCategories; ++i)
    {
        UE_LOG(LogAILearning, VeryVerbose, TEXT("정규화 전 확률[%d]: %.3f"), i, Probabilities[i]);
    }

    // 최소 확률 보장
    int32 MinProbAppliedCount = 0;
    for (float& Prob : Probabilities)
    {
        if (Prob < MinCategoryProbability)
        {
            Prob = MinCategoryProbability;
            MinProbAppliedCount++;
        }
    }

    if (MinProbAppliedCount > 0)
    {
        UE_LOG(LogAILearning, Log, TEXT("최소 확률 보장 적용: %d개 카테고리를 %.3f로 조정"),
            MinProbAppliedCount, MinCategoryProbability);
    }

    // 정규화
    float Total = 0.0f;
    for (float Prob : Probabilities)
    {
        Total += Prob;
    }

    if (Total > 0.0f)
    {
        for (float& Prob : Probabilities)
        {
            Prob /= Total;
        }

        UE_LOG(LogAILearning, Log, TEXT("확률 정규화 완료 - 총합: %.3f → 1.000"), Total);

        // 정규화 후 로그
        for (int32 i = 0; i < NumCategories; ++i)
        {
            UE_LOG(LogAILearning, VeryVerbose, TEXT("정규화 후 확률[%d]: %.3f"), i, Probabilities[i]);
        }
    }
    else
    {
        UE_LOG(LogAILearning, Error, TEXT("확률 정규화 실패: 총합이 0"));
    }
}
#pragma endregion

#pragma region FAIAdaptiveExecutionConfig
void FAIAdaptiveExecutionConfig::GetExecutionCountRange(const FGameplayTag& BlockCategory, int32& OutMinCount, int32& OutMaxCount) const
{
    UE_LOG(LogAILearning, Log, TEXT("실행 개수 범위 조회 - 카테고리: %s"), *BlockCategory.ToString());

    if (BlockCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"))))
    {
        OutMinCount = OpeningSkillsMinCount;
        OutMaxCount = OpeningSkillsMaxCount;
        UE_LOG(LogAILearning, Log, TEXT("OpeningSkills 실행 범위: %d~%d"), OutMinCount, OutMaxCount);
    }
    else if (BlockCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"))))
    {
        OutMinCount = SkillsMinCount;
        OutMaxCount = SkillsMaxCount;
        UE_LOG(LogAILearning, Log, TEXT("Skills 실행 범위: %d~%d"), OutMinCount, OutMaxCount);
    }
    else if (BlockCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))))
    {
        OutMinCount = StrafeOrDodgeMinCount;
        OutMaxCount = StrafeOrDodgeMaxCount;
        UE_LOG(LogAILearning, Log, TEXT("StrafeOrDodge 실행 범위: %d~%d"), OutMinCount, OutMaxCount);
    }
    else
    {
        // 기본값
        OutMinCount = 1;
        OutMaxCount = 2;
        UE_LOG(LogAILearning, Warning, TEXT("알 수 없는 블록 카테고리: %s, 기본값 적용 (%d~%d)"),
            *BlockCategory.ToString(), OutMinCount, OutMaxCount);
    }
}

int32 FAIAdaptiveExecutionConfig::CalculateExecutionCountByPreference(const FGameplayTag& BlockCategory, float PreferenceValue, bool bIsExplorationMode) const
{
    UE_LOG(LogAILearning, Log, TEXT("선호도 기반 실행 개수 계산 시작 - 카테고리: %s, 선호도: %.2f, 탐험모드: %s"),
        *BlockCategory.ToString(), PreferenceValue, bIsExplorationMode ? TEXT("활성") : TEXT("비활성"));

    int32 MinCount, MaxCount;
    GetExecutionCountRange(BlockCategory, MinCount, MaxCount);

    // 선호도를 0-1로 정규화
    float NormalizedPreference = FMath::Clamp(PreferenceValue / 100.0f, 0.0f, 1.0f);
    UE_LOG(LogAILearning, Log, TEXT("정규화된 선호도: %.3f"), NormalizedPreference);

    // 기본 계산: MinCount + (정규화된선호도 × 범위 × 영향강도)
    float Range = static_cast<float>(MaxCount - MinCount);
    float PreferenceAdjustment = NormalizedPreference * Range * PreferenceInfluenceStrength;
    int32 CalculatedCount = MinCount + FMath::RoundToInt(PreferenceAdjustment);

    UE_LOG(LogAILearning, Log, TEXT("기본 계산 결과: %d (범위: %.1f, 조정값: %.2f, 영향강도: %.2f)"),
        CalculatedCount, Range, PreferenceAdjustment, PreferenceInfluenceStrength);

    // 탐험 모드 보정
    if (bIsExplorationMode && bBoostExplorationExecution)
    {
        int32 BeforeBoost = CalculatedCount;
        CalculatedCount = FMath::Max(CalculatedCount, MinExplorationExecutionCount);

        if (BeforeBoost != CalculatedCount)
        {
            UE_LOG(LogAILearning, Log, TEXT("탐험 모드 보정 적용: %d → %d (최소 보장: %d)"),
                BeforeBoost, CalculatedCount, MinExplorationExecutionCount);
        }
    }

    // 최종 범위 클램핑
    int32 FinalCount = FMath::Clamp(CalculatedCount, MinCount, MaxCount);

    if (CalculatedCount != FinalCount)
    {
        UE_LOG(LogAILearning, Log, TEXT("범위 클램핑 적용: %d → %d"), CalculatedCount, FinalCount);
    }

    UE_LOG(LogAILearning, Log, TEXT("최종 실행 개수 결정: %d"), FinalCount);
    return FinalCount;
}

bool FAIAdaptiveExecutionConfig::ValidateSettings() const
{
    UE_LOG(LogAILearning, Log, TEXT("적응형 실행 설정 유효성 검증 시작"));

    bool bIsValid = true;

    // 최소값이 최대값보다 큰지 검증
    if (OpeningSkillsMinCount > OpeningSkillsMaxCount)
    {
        UE_LOG(LogAILearning, Error, TEXT("OpeningSkills 최소값(%d)이 최대값(%d)보다 큼"),
            OpeningSkillsMinCount, OpeningSkillsMaxCount);
        bIsValid = false;
    }

    if (SkillsMinCount > SkillsMaxCount)
    {
        UE_LOG(LogAILearning, Error, TEXT("Skills 최소값(%d)이 최대값(%d)보다 큼"),
            SkillsMinCount, SkillsMaxCount);
        bIsValid = false;
    }

    if (StrafeOrDodgeMinCount > StrafeOrDodgeMaxCount)
    {
        UE_LOG(LogAILearning, Error, TEXT("StrafeOrDodge 최소값(%d)이 최대값(%d)보다 큼"),
            StrafeOrDodgeMinCount, StrafeOrDodgeMaxCount);
        bIsValid = false;
    }

    // 모든 최소값이 1 이상인지 검증
    if (OpeningSkillsMinCount < 1)
    {
        UE_LOG(LogAILearning, Error, TEXT("OpeningSkills 최소값(%d)이 1보다 작음"), OpeningSkillsMinCount);
        bIsValid = false;
    }

    if (SkillsMinCount < 1)
    {
        UE_LOG(LogAILearning, Error, TEXT("Skills 최소값(%d)이 1보다 작음"), SkillsMinCount);
        bIsValid = false;
    }

    if (StrafeOrDodgeMinCount < 1)
    {
        UE_LOG(LogAILearning, Error, TEXT("StrafeOrDodge 최소값(%d)이 1보다 작음"), StrafeOrDodgeMinCount);
        bIsValid = false;
    }

    // 선호도 영향 강도 범위 검증
    if (PreferenceInfluenceStrength < 0.0f || PreferenceInfluenceStrength > 1.0f)
    {
        UE_LOG(LogAILearning, Error, TEXT("선호도 영향 강도(%.2f)가 유효 범위(0.0~1.0)를 벗어남"),
            PreferenceInfluenceStrength);
        bIsValid = false;
    }

    UE_LOG(LogAILearning, Log, TEXT("적응형 실행 설정 유효성 검증 완료: %s"), bIsValid ? TEXT("성공") : TEXT("실패"));
    return bIsValid;
}
#pragma endregion

#pragma region FAIRuntimeLearningState
/**
 * @brief 경험 가중치를 업데이트합니다.
 * @param ActionCategory 행동 카테고리
 * @param WeightChange 가중치 변화량
 * @param Config 경험 가중치 설정
 * @param InitialPersonality 초기 성향
 */
void FAIRuntimeLearningState::UpdateExperienceWeight(const FGameplayTag& ActionCategory,
    float WeightChange,
    const FAIExperienceWeightConfig& Config,
    const FAISimplifiedPersonality& InitialPersonality)
{
    UE_LOG(LogAILearning, Log, TEXT("경험 가중치 업데이트 시작 - 카테고리: %s, 변화량: %.4f"),
        *ActionCategory.ToString(), WeightChange);

    float ClampedChange = Config.ClampSingleChange(WeightChange);

    if (WeightChange != ClampedChange)
    {
        UE_LOG(LogAILearning, VeryVerbose, TEXT("변화량 클램핑: %.4f → %.4f"), WeightChange, ClampedChange);
    }

    float OldWeight = 0.0f;
    float NewWeight = 0.0f;
    FString CategoryName;

    if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"))))
    {
        OldWeight = OpeningSkillsWeight;
        OpeningSkillsWeight = Config.ClampWeightChange(OpeningSkillsWeight, 0.0f, ClampedChange);
        NewWeight = OpeningSkillsWeight;
        CategoryName = TEXT("OpeningSkills");
    }
    else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"))))
    {
        OldWeight = SkillsWeight;
        SkillsWeight = Config.ClampWeightChange(SkillsWeight, 0.0f, ClampedChange);
        NewWeight = SkillsWeight;
        CategoryName = TEXT("Skills");
    }
    else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))))
    {
        OldWeight = StrafeOrDodgeWeight;
        StrafeOrDodgeWeight = Config.ClampWeightChange(StrafeOrDodgeWeight, 0.0f, ClampedChange);
        NewWeight = StrafeOrDodgeWeight;
        CategoryName = TEXT("StrafeOrDodge");
    }
    else
    {
        UE_LOG(LogAILearning, Warning, TEXT("알 수 없는 행동 카테고리: %s"), *ActionCategory.ToString());
        return;
    }

    LearningCount++;

    UE_LOG(LogAILearning, Log, TEXT("%s 가중치 업데이트: %.3f → %.3f (학습횟수: %d)"),
        *CategoryName, OldWeight, NewWeight, LearningCount);

    // 중요한 변화 감지
    float WeightDifference = FMath::Abs(NewWeight - OldWeight);
    if (WeightDifference > 0.1f)
    {
        UE_LOG(LogAILearning, Log, TEXT("상당한 가중치 변화 감지: %s 카테고리에서 %.3f 변화"),
            *CategoryName, WeightDifference);
    }

    // 학습 마일스톤 로그
    if (LearningCount % 10 == 0)
    {
        UE_LOG(LogAILearning, Log, TEXT("학습 마일스톤: %d회 - 현재 가중치 [Opening: %.3f, Skills: %.3f, Dodge: %.3f]"),
            LearningCount, OpeningSkillsWeight, SkillsWeight, StrafeOrDodgeWeight);
    }
}

/**
 * @brief 현재 행동 선호도를 계산합니다 (초기 성향 + 경험 가중치).
 * @param ActionCategory 행동 카테고리
 * @param InitialPersonality 초기 성향
 * @return 현재 행동 선호도
 */
float FAIRuntimeLearningState::GetCurrentActionPreference(const FGameplayTag& ActionCategory,
    const FAISimplifiedPersonality& InitialPersonality) const
{
    float InitialPreference = InitialPersonality.GetInitialActionPreference(ActionCategory);
    float ExperienceWeight = 0.0f;
    FString CategoryName;

    if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"))))
    {
        ExperienceWeight = OpeningSkillsWeight;
        CategoryName = TEXT("OpeningSkills");
    }
    else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"))))
    {
        ExperienceWeight = SkillsWeight;
        CategoryName = TEXT("Skills");
    }
    else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))))
    {
        ExperienceWeight = StrafeOrDodgeWeight;
        CategoryName = TEXT("StrafeOrDodge");
    }
    else
    {
        UE_LOG(LogAILearning, Warning, TEXT("알 수 없는 행동 카테고리: %s"), *ActionCategory.ToString());
        return InitialPreference;
    }

    float CurrentPreference = FMath::Clamp(InitialPreference + (ExperienceWeight * 100.0f), 0.0f, 100.0f);

    UE_LOG(LogAILearning, VeryVerbose, TEXT("%s 현재 선호도: 초기=%.2f + 경험=%.3f*100 = %.2f"),
        *CategoryName, InitialPreference, ExperienceWeight, CurrentPreference);

    return CurrentPreference;
}

/**
 * @brief 적응적 리셋을 실행합니다.
 * @param Config 경험 가중치 설정
 */
void FAIRuntimeLearningState::ApplyAdaptiveReset(const FAIExperienceWeightConfig& Config)
{
    UE_LOG(LogAILearning, Warning, TEXT("적응적 리셋 시작 - 리셋 비율: %.1f%%"), Config.AdaptiveResetPercentage * 100.0f);

    float ResetFactor = 1.0f - Config.AdaptiveResetPercentage;

    float OldOpeningWeight = OpeningSkillsWeight;
    float OldSkillsWeight = SkillsWeight;
    float OldDodgeWeight = StrafeOrDodgeWeight;

    OpeningSkillsWeight *= ResetFactor;
    SkillsWeight *= ResetFactor;
    StrafeOrDodgeWeight *= ResetFactor;

    bIsOverdrivePrevention = true;

    UE_LOG(LogAILearning, Warning, TEXT("적응적 리셋 완료 - Opening: %.3f→%.3f, Skills: %.3f→%.3f, Dodge: %.3f→%.3f"),
        OldOpeningWeight, OpeningSkillsWeight,
        OldSkillsWeight, SkillsWeight,
        OldDodgeWeight, StrafeOrDodgeWeight);

    UE_LOG(LogAILearning, Log, TEXT("오버드라이브 방지 모드 활성화"));
}
#pragma endregion

//@Default Setting
#pragma region Default Setting
UAIDataSetInfos::UAIDataSetInfos(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UAIDataSetInfos::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FAIDataSetValidationResult ValidationResult = ValidateAIDataSets();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);

        // 자동 수정 시도
        if (!TryAutoFixValidationError(ValidationResult))
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI DataSet 무결성 검사 실패 - 수동 수정 필요: {0}",
                *ValidationResult.ErrorMessage);
        }
    }
    else
    {
        UE_LOGFMT(LogAIDataSetInfos, Log, "AI DataSet 무결성 검사 통과");
    }
}

void UAIDataSetInfos::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    // 배열이나 구조체 내부 프로퍼티 변경 시에도 검증 수행
    FAIDataSetValidationResult ValidationResult = ValidateAIDataSets();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);
        TryAutoFixValidationError(ValidationResult);
    }
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateAIDataSets() const
{
    // === 1단계: 기본 설정 검증 ===
    FAIDataSetValidationResult Result = ValidateBasicSettings();
    if (!Result.bIsValid) return Result;

    // === 2단계: AI 데이터셋 개별 검증 ===
    Result = ValidateIndividualAIDataSets();
    if (!Result.bIsValid) return Result;

    // === 3단계: AI 타입 중복 검증 ===
    Result = ValidateAITypeUniqueness();
    if (!Result.bIsValid) return Result;

    // === 4단계: 전투 시퀀스 검증 ===
    Result = ValidateCombatSequences();
    if (!Result.bIsValid) return Result;

    // === 5단계: 어빌리티 블록 검증 ===
    Result = ValidateAbilityBlocks();
    if (!Result.bIsValid) return Result;

    // === 6단계: 성향 특성 검증 ===
    Result = ValidatePersonalityTraits();
    if (!Result.bIsValid) return Result;

    // === 7단계: 논리적 일관성 검증 ===
    Result = ValidateLogicalConsistency();
    if (!Result.bIsValid) return Result;

    // === 8단계: 적응형 시스템 검증 (새로 추가) ===
    Result = ValidateAdaptiveSystemSettings();
    if (!Result.bIsValid) return Result;

    return FAIDataSetValidationResult(); // 모든 검증 통과
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateBasicSettings() const
{
    if (AIDataSets.Num() == 0)
    {
        return FAIDataSetValidationResult(
            EAIDataSetValidationError::EmptyDataSet,
            TEXT("AI 데이터셋이 비어있습니다. 최소한 하나의 AI 데이터는 필요합니다.")
        );
    }

    //@50개 제한
    if (AIDataSets.Num() > 50)
    {
        UE_LOGFMT(LogAIDataSetInfos, Warning,
            "AI 데이터셋이 매우 많습니다 ({0}개). 성능에 영향을 줄 수 있습니다.", AIDataSets.Num());
    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateIndividualAIDataSets() const
{
    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];

        //@AI Type
        if (DataSet.AIType == EAIType::MAX)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidAIType,
                FString::Printf(TEXT("유효하지 않은 AI 타입입니다 (인덱스: %d)"), DataSetIndex),
                DataSetIndex
            );
        }

        if (!DataSet.BehaviorTree)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::NullBehaviorTree,
                FString::Printf(TEXT("BehaviorTree가 설정되지 않았습니다 (인덱스: %d, AI타입: %s)"),
                    DataSetIndex, *UEnum::GetValueAsString(DataSet.AIType)),
                DataSetIndex
            );
        }

        // === 행동 트리 클래스 타입 검증 ===
        if (!DataSet.BehaviorTree->IsValidLowLevel())
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidBehaviorTree,
                FString::Printf(TEXT("BehaviorTree가 유효하지 않은 상태입니다 (인덱스: %d)"), DataSetIndex),
                DataSetIndex
            );
        }
    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateAITypeUniqueness() const
{
    TMap<EAIType, int32> UsedAITypes;

    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];

        if (int32* ExistingIndex = UsedAITypes.Find(DataSet.AIType))
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::DuplicateAIType,
                FString::Printf(TEXT("중복된 AI 타입 발견: %s (인덱스 %d와 %d)"),
                    *UEnum::GetValueAsString(DataSet.AIType), *ExistingIndex, DataSetIndex),
                DataSetIndex
            );
        }

        UsedAITypes.Add(DataSet.AIType, DataSetIndex);
    }

    return FAIDataSetValidationResult();
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateCombatSequences() const
{
    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];
        const FAICombatSequence& CombatSequence = DataSet.CombatSequence;

        // === StartBlock 검증 ===
        FAIDataSetValidationResult Result = ValidateSingleAbilityBlock(CombatSequence.StartBlock,
            DataSetIndex, TEXT("StartBlock"));
        if (!Result.bIsValid) return Result;

        // === AbilityBlocks 검증 ===
        if (CombatSequence.AbilityBlocks.Num() == 0)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0} ({1})의 전투 시퀀스에 AbilityBlock이 없습니다. 전투 패턴이 제한적일 수 있습니다.",
                DataSetIndex, *UEnum::GetValueAsString(DataSet.AIType));
        }

        for (int32 BlockIndex = 0; BlockIndex < CombatSequence.AbilityBlocks.Num(); ++BlockIndex)
        {
            Result = ValidateSingleAbilityBlock(CombatSequence.AbilityBlocks[BlockIndex],
                DataSetIndex, FString::Printf(TEXT("AbilityBlock[%d]"), BlockIndex));
            if (!Result.bIsValid) return Result;
        }

        // === ExitBlock 검증 ===
        Result = ValidateSingleAbilityBlock(CombatSequence.ExitBlock,
            DataSetIndex, TEXT("ExitBlock"));
        if (!Result.bIsValid) return Result;

        // === 블록 식별자 중복 검증 ===
        Result = ValidateBlockIdentifierUniqueness(CombatSequence, DataSetIndex);
        if (!Result.bIsValid) return Result;
    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateSingleAbilityBlock(const FAIAbilityBlock& Block,
    int32 DataSetIndex, const FString& BlockName) const
{
    // === 블록 식별자 검증 ===
    if (!Block.BlockIdentifier.IsValid())
    {
        return FAIDataSetValidationResult(
            EAIDataSetValidationError::InvalidBlockIdentifier,
            FString::Printf(TEXT("유효하지 않은 블록 식별자: %s (DataSet 인덱스: %d)"),
                *BlockName, DataSetIndex),
            DataSetIndex
        );
    }

    // === 실행 우선순위 검증 ===
    if (Block.ExecutionPriority < 0)
    {
        return FAIDataSetValidationResult(
            EAIDataSetValidationError::InvalidExecutionPriority,
            FString::Printf(TEXT("유효하지 않은 실행 우선순위: %s (DataSet 인덱스: %d, 우선순위: %d)"),
                *BlockName, DataSetIndex, Block.ExecutionPriority),
            DataSetIndex
        );
    }

    // === 어빌리티 블록 유닛 검증 ===
    if (Block.AbilityBlockUnits.Num() == 0)
    {
        UE_LOGFMT(LogAIDataSetInfos, Warning,
            "블록 {0} (DataSet {1})에 어빌리티가 없습니다. 이 블록은 실행되지 않을 수 있습니다.",
            *BlockName, DataSetIndex);
    }

    for (int32 UnitIndex = 0; UnitIndex < Block.AbilityBlockUnits.Num(); ++UnitIndex)
    {
        const FAIAbilityBlockUnit& Unit = Block.AbilityBlockUnits[UnitIndex];

        // === 어빌리티 태그 검증 ===
        if (!Unit.AbilityTag.IsValid())
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidAbilityTag,
                FString::Printf(TEXT("유효하지 않은 어빌리티 태그: %s[%d] (DataSet 인덱스: %d)"),
                    *BlockName, UnitIndex, DataSetIndex),
                DataSetIndex
            );
        }

        // === 우선순위 검증 ===
        if (Unit.Priority < 0)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidAbilityPriority,
                FString::Printf(TEXT("유효하지 않은 어빌리티 우선순위: %s[%d] (DataSet 인덱스: %d, 우선순위: %d)"),
                    *BlockName, UnitIndex, DataSetIndex, Unit.Priority),
                DataSetIndex
            );
        }
    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateBlockIdentifierUniqueness(const FAICombatSequence& CombatSequence,
    int32 DataSetIndex) const
{
    TMap<FGameplayTag, FString> UsedIdentifiers;

    // StartBlock 추가
    if (CombatSequence.StartBlock.BlockIdentifier.IsValid())
    {
        UsedIdentifiers.Add(CombatSequence.StartBlock.BlockIdentifier, TEXT("StartBlock"));
    }

    // AbilityBlocks 검사
    for (int32 BlockIndex = 0; BlockIndex < CombatSequence.AbilityBlocks.Num(); ++BlockIndex)
    {
        const FAIAbilityBlock& Block = CombatSequence.AbilityBlocks[BlockIndex];

        if (Block.BlockIdentifier.IsValid())
        {
            if (FString* ExistingBlockName = UsedIdentifiers.Find(Block.BlockIdentifier))
            {
                return FAIDataSetValidationResult(
                    EAIDataSetValidationError::DuplicateBlockIdentifier,
                    FString::Printf(TEXT("중복된 블록 식별자: %s (DataSet %d에서 %s와 AbilityBlock[%d] 간 중복)"),
                        *Block.BlockIdentifier.ToString(), DataSetIndex, **ExistingBlockName, BlockIndex),
                    DataSetIndex
                );
            }

            UsedIdentifiers.Add(Block.BlockIdentifier, FString::Printf(TEXT("AbilityBlock[%d]"), BlockIndex));
        }
    }

    // ExitBlock 추가
    if (CombatSequence.ExitBlock.BlockIdentifier.IsValid())
    {
        if (FString* ExistingBlockName = UsedIdentifiers.Find(CombatSequence.ExitBlock.BlockIdentifier))
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::DuplicateBlockIdentifier,
                FString::Printf(TEXT("중복된 블록 식별자: %s (DataSet %d에서 %s와 ExitBlock 간 중복)"),
                    *CombatSequence.ExitBlock.BlockIdentifier.ToString(), DataSetIndex, **ExistingBlockName),
                DataSetIndex
            );
        }
    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateAbilityBlocks() const
{
    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];
        const FAICombatSequence& CombatSequence = DataSet.CombatSequence;

        // === 각 블록 내 어빌리티 태그 중복 검증 ===
        for (int32 BlockIndex = 0; BlockIndex < CombatSequence.AbilityBlocks.Num(); ++BlockIndex)
        {
            const FAIAbilityBlock& Block = CombatSequence.AbilityBlocks[BlockIndex];

            TMap<FGameplayTag, int32> UsedAbilityTags;

            for (int32 UnitIndex = 0; UnitIndex < Block.AbilityBlockUnits.Num(); ++UnitIndex)
            {
                const FAIAbilityBlockUnit& Unit = Block.AbilityBlockUnits[UnitIndex];

                if (Unit.AbilityTag.IsValid())
                {
                    if (int32* ExistingIndex = UsedAbilityTags.Find(Unit.AbilityTag))
                    {
                        return FAIDataSetValidationResult(
                            EAIDataSetValidationError::DuplicateAbilityTagInBlock,
                            FString::Printf(TEXT("블록 내 중복된 어빌리티 태그: %s (DataSet %d, Block %d, Unit %d와 %d)"),
                                *Unit.AbilityTag.ToString(), DataSetIndex, BlockIndex, *ExistingIndex, UnitIndex),
                            DataSetIndex
                        );
                    }

                    UsedAbilityTags.Add(Unit.AbilityTag, UnitIndex);
                }
            }
        }

        // === 우선순위 순서 검증 (권장사항) ===
        for (int32 BlockIndex = 0; BlockIndex < CombatSequence.AbilityBlocks.Num(); ++BlockIndex)
        {
            const FAIAbilityBlock& Block = CombatSequence.AbilityBlocks[BlockIndex];

            for (int32 UnitIndex = 1; UnitIndex < Block.AbilityBlockUnits.Num(); ++UnitIndex)
            {
                const FAIAbilityBlockUnit& CurrentUnit = Block.AbilityBlockUnits[UnitIndex];
                const FAIAbilityBlockUnit& PreviousUnit = Block.AbilityBlockUnits[UnitIndex - 1];

                if (CurrentUnit.Priority < PreviousUnit.Priority)
                {
                    UE_LOGFMT(LogAIDataSetInfos, Warning,
                        "블록 내 어빌리티 우선순위가 정렬되지 않았습니다. (DataSet {0}, Block {1}) SortAbilityBlockUnitsByPriority() 호출을 권장합니다.",
                        DataSetIndex, BlockIndex);
                    break; // 한 번만 경고
                }
            }
        }

        // === 적응형 시스템 블록 유닛 개수 검증 추가 ===
        if (DataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
        {
            UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s의 적응형 블록 유닛 개수 검증 시작"),
                *UEnum::GetValueAsString(DataSet.AIType));

            const FAIAdaptiveExecutionConfig& ExecutionConfig = DataSet.AdaptiveBehaviorConfig.ExecutionConfig;

            for (int32 BlockIndex = 0; BlockIndex < CombatSequence.AbilityBlocks.Num(); ++BlockIndex)
            {
                const FAIAbilityBlock& Block = CombatSequence.AbilityBlocks[BlockIndex];
                const FGameplayTag& BlockIdentifier = Block.BlockIdentifier;
                int32 ActualUnitCount = Block.AbilityBlockUnits.Num();

                int32 RequiredMinCount = 0;
                FString BlockCategoryName;

                // 블록 카테고리별 필요 최소 개수 확인
                if (BlockIdentifier.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"))))
                {
                    RequiredMinCount = ExecutionConfig.OpeningSkillsMinCount;
                    BlockCategoryName = TEXT("OpeningSkills");
                }
                else if (BlockIdentifier.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"))))
                {
                    RequiredMinCount = ExecutionConfig.SkillsMinCount;
                    BlockCategoryName = TEXT("Skills");
                }
                else if (BlockIdentifier.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))))
                {
                    RequiredMinCount = ExecutionConfig.StrafeOrDodgeMinCount;
                    BlockCategoryName = TEXT("StrafeOrDodge");
                }
                else
                {
                    // 알 수 없는 카테고리는 건너뛰기
                    UE_LOG(LogAILearning, Log, TEXT("알 수 없는 블록 카테고리: %s, 검증 건너뛰기"),
                        *BlockIdentifier.ToString());
                    continue;
                }

                // 유닛 개수 부족 검증
                if (ActualUnitCount < RequiredMinCount)
                {
                    FString ErrorMessage = FString::Printf(
                        TEXT("적응형 시스템 블록 유닛 부족: AI 타입 %s의 %s 블록[%d]에 %d개 유닛이 있지만, 최소 %d개가 필요함 (블록ID: %s)"),
                        *UEnum::GetValueAsString(DataSet.AIType),
                        *BlockCategoryName,
                        BlockIndex,
                        ActualUnitCount,
                        RequiredMinCount,
                        *BlockIdentifier.ToString()
                    );

                    UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);

                    return FAIDataSetValidationResult(
                        EAIDataSetValidationError::LogicalInconsistency,
                        ErrorMessage,
                        DataSetIndex
                    );
                }
                else
                {
                    UE_LOG(LogAILearning, Log, TEXT("%s 블록[%d] 유닛 개수 검증 통과: %d개 (최소 %d개 필요)"),
                        *BlockCategoryName, BlockIndex, ActualUnitCount, RequiredMinCount);
                }
            }

            UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s의 적응형 블록 유닛 개수 검증 완료"),
                *UEnum::GetValueAsString(DataSet.AIType));
        }

    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidatePersonalityTraits() const
{
    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];
        const FAIPersonalityTraits& Traits = DataSet.PersonalityTraits;

        // === 범위 검증 (0-100) ===
        if (Traits.Aggressiveness < 0.0f || Traits.Aggressiveness > 100.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("공격성 값이 유효 범위(0-100)를 벗어났습니다 (DataSet %d, 값: %.2f)"),
                    DataSetIndex, Traits.Aggressiveness),
                DataSetIndex
            );
        }

        if (Traits.Agility < 0.0f || Traits.Agility > 100.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("기민함 값이 유효 범위(0-100)를 벗어났습니다 (DataSet %d, 값: %.2f)"),
                    DataSetIndex, Traits.Agility),
                DataSetIndex
            );
        }

        if (Traits.Proactiveness < 0.0f || Traits.Proactiveness > 100.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("적극성 값이 유효 범위(0-100)를 벗어났습니다 (DataSet %d, 값: %.2f)"),
                    DataSetIndex, Traits.Proactiveness),
                DataSetIndex
            );
        }

        if (Traits.Methodicalness < 0.0f || Traits.Methodicalness > 100.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("계획성 값이 유효 범위(0-100)를 벗어났습니다 (DataSet %d, 값: %.2f)"),
                    DataSetIndex, Traits.Methodicalness),
                DataSetIndex
            );
        }

        // === 밸런스 검증 (권장사항) ===
        float TotalTraits = Traits.Aggressiveness + Traits.Agility + Traits.Proactiveness + Traits.Methodicalness;
        float AverageTraits = TotalTraits / 4.0f;

        if (AverageTraits < 20.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}의 성향 특성이 전반적으로 너무 낮습니다 (평균: {1}). AI가 소극적으로 행동할 수 있습니다.",
                DataSetIndex, AverageTraits);
        }
        else if (AverageTraits > 80.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}의 성향 특성이 전반적으로 너무 높습니다 (평균: {1}). AI가 과도하게 공격적일 수 있습니다.",
                DataSetIndex, AverageTraits);
        }

        // === 극단적 조합 경고 ===
        if (Traits.Aggressiveness > 80.0f && Traits.Methodicalness < 20.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}는 높은 공격성과 낮은 계획성을 가지고 있습니다. 예측 불가능한 행동을 할 수 있습니다.",
                DataSetIndex);
        }
    }

    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateAdaptiveSystemSettings() const
{
    UE_LOG(LogAILearning, Log, TEXT("적응형 시스템 설정 전체 검증 시작"));

    for (int32 i = 0; i < AIDataSets.Num(); ++i)
    {
        const FAIDataSet& DataSet = AIDataSets[i];

        // 적응형 시스템이 비활성화된 경우 건너뛰기
        if (!DataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
        {
            UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s: 적응형 시스템 비활성화, 검증 건너뛰기"),
                *UEnum::GetValueAsString(DataSet.AIType));
            continue;
        }

        UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s의 적응형 시스템 설정 검증 중"),
            *UEnum::GetValueAsString(DataSet.AIType));

        const FAIAdaptiveBehaviorConfig& AdaptiveConfig = DataSet.AdaptiveBehaviorConfig;

        // === 기존 적응형 시스템 검증 로직 ===

        // SimplifiedPersonality 검증
        const FAISimplifiedPersonality& Personality = AdaptiveConfig.SimplifiedPersonality;
        if (Personality.Aggressiveness < 0.0f || Personality.Aggressiveness > 100.0f ||
            Personality.Agility < 0.0f || Personality.Agility > 100.0f)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 간소화된 성향 값이 유효 범위(0-100)를 벗어남: 공격성=%.1f, 기민함=%.1f"),
                *UEnum::GetValueAsString(DataSet.AIType), Personality.Aggressiveness, Personality.Agility
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::InvalidPersonalityValue, ErrorMessage, i);
        }

        // RewardCalculator 검증
        const FAIRewardCalculator& RewardCalc = AdaptiveConfig.RewardCalculator;
        if (RewardCalc.BaseLearningRate <= 0.0f || RewardCalc.BaseLearningRate > 1.0f ||
            RewardCalc.LearningDecayFactor <= 0.0f || RewardCalc.LearningDecayFactor >= 1.0f)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 학습률 설정이 유효하지 않음: 기본학습률=%.3f, 감쇠인수=%.3f"),
                *UEnum::GetValueAsString(DataSet.AIType), RewardCalc.BaseLearningRate, RewardCalc.LearningDecayFactor
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::InvalidPersonalityValue, ErrorMessage, i);
        }

        // ExperienceWeightConfig 검증
        const FAIExperienceWeightConfig& WeightConfig = AdaptiveConfig.ExperienceWeightConfig;
        if (WeightConfig.MaxWeightChange <= 0.0f || WeightConfig.MinSingleChange <= 0.0f ||
            WeightConfig.MinCategoryProbability <= 0.0f || WeightConfig.MinCategoryProbability >= 1.0f)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 경험 가중치 설정이 유효하지 않음"),
                *UEnum::GetValueAsString(DataSet.AIType)
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::InvalidPersonalityValue, ErrorMessage, i);
        }

        // === ExecutionConfig 검증 추가 ===

        UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s의 적응형 실행 설정 검증 시작"),
            *UEnum::GetValueAsString(DataSet.AIType));

        const FAIAdaptiveExecutionConfig& ExecutionConfig = AdaptiveConfig.ExecutionConfig;

        // ExecutionConfig 내부 유효성 검증
        if (!ExecutionConfig.ValidateSettings())
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 적응형 실행 설정이 유효하지 않음 (내부 검증 실패)"),
                *UEnum::GetValueAsString(DataSet.AIType)
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::LogicalInconsistency, ErrorMessage, i);
        }

        // 각 블록별 설정 상세 검증
        if (ExecutionConfig.OpeningSkillsMinCount > ExecutionConfig.OpeningSkillsMaxCount)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 OpeningSkills 실행 개수 설정 오류: 최소(%d) > 최대(%d)"),
                *UEnum::GetValueAsString(DataSet.AIType), ExecutionConfig.OpeningSkillsMinCount, ExecutionConfig.OpeningSkillsMaxCount
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::LogicalInconsistency, ErrorMessage, i);
        }

        if (ExecutionConfig.SkillsMinCount > ExecutionConfig.SkillsMaxCount)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 Skills 실행 개수 설정 오류: 최소(%d) > 최대(%d)"),
                *UEnum::GetValueAsString(DataSet.AIType), ExecutionConfig.SkillsMinCount, ExecutionConfig.SkillsMaxCount
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::LogicalInconsistency, ErrorMessage, i);
        }

        if (ExecutionConfig.StrafeOrDodgeMinCount > ExecutionConfig.StrafeOrDodgeMaxCount)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 StrafeOrDodge 실행 개수 설정 오류: 최소(%d) > 최대(%d)"),
                *UEnum::GetValueAsString(DataSet.AIType), ExecutionConfig.StrafeOrDodgeMinCount, ExecutionConfig.StrafeOrDodgeMaxCount
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::LogicalInconsistency, ErrorMessage, i);
        }

        // 선호도 영향 강도 검증
        if (ExecutionConfig.PreferenceInfluenceStrength < 0.0f || ExecutionConfig.PreferenceInfluenceStrength > 1.0f)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 선호도 영향 강도(%.2f)가 유효 범위(0.0~1.0)를 벗어남"),
                *UEnum::GetValueAsString(DataSet.AIType), ExecutionConfig.PreferenceInfluenceStrength
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::InvalidPersonalityValue, ErrorMessage, i);
        }

        // 게임 밸런스 검증 (경고 수준)
        int32 MaxPossibleExecution = FMath::Max3(ExecutionConfig.OpeningSkillsMaxCount,
            ExecutionConfig.SkillsMaxCount,
            ExecutionConfig.StrafeOrDodgeMaxCount);

        if (MaxPossibleExecution > 6)
        {
            UE_LOG(LogAILearning, Warning, TEXT("AI 타입 %s의 최대 실행 개수(%d)가 권장 범위(6개)를 초과 - 게임 밸런스 주의 필요"),
                *UEnum::GetValueAsString(DataSet.AIType), MaxPossibleExecution);
        }

        // 탐험 모드 설정 검증
        if (ExecutionConfig.bBoostExplorationExecution && ExecutionConfig.MinExplorationExecutionCount < 1)
        {
            FString ErrorMessage = FString::Printf(
                TEXT("AI 타입 %s의 탐험 모드 최소 실행 개수(%d)가 1보다 작음"),
                *UEnum::GetValueAsString(DataSet.AIType), ExecutionConfig.MinExplorationExecutionCount
            );
            UE_LOG(LogAILearning, Error, TEXT("%s"), *ErrorMessage);
            return FAIDataSetValidationResult(EAIDataSetValidationError::LogicalInconsistency, ErrorMessage, i);
        }

        UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s의 적응형 실행 설정 검증 완료 - 모든 설정이 유효함"),
            *UEnum::GetValueAsString(DataSet.AIType));

        UE_LOG(LogAILearning, Log, TEXT("AI 타입 %s의 적응형 시스템 전체 검증 완료"),
            *UEnum::GetValueAsString(DataSet.AIType));
    }

    UE_LOG(LogAILearning, Log, TEXT("적응형 시스템 설정 전체 검증 완료 - 모든 AI 타입 통과"));
    return FAIDataSetValidationResult(); // 성공
}

FAIDataSetValidationResult UAIDataSetInfos::ValidateLogicalConsistency() const
{
    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];
        const FAICombatSequence& CombatSequence = DataSet.CombatSequence;

        // === 전투 패턴과 성향의 일관성 검증 ===

        // 공격적인 AI는 충분한 공격 어빌리티를 가져야 함
        if (DataSet.PersonalityTraits.Aggressiveness > 70.0f)
        {
            int32 TotalAbilityCount = 0;
            for (const FAIAbilityBlock& Block : CombatSequence.AbilityBlocks)
            {
                TotalAbilityCount += Block.AbilityBlockUnits.Num();
            }

            if (TotalAbilityCount < 2)
            {
                UE_LOGFMT(LogAIDataSetInfos, Warning,
                    "공격적인 AI {0} (공격성: {1})이지만 사용 가능한 어빌리티가 적습니다 ({2}개). 더 다양한 공격 패턴을 권장합니다.",
                    DataSetIndex, DataSet.PersonalityTraits.Aggressiveness, TotalAbilityCount);
            }
        }

        // === 빈 블록 검증 ===
        bool bHasEmptyBlocks = false;
        if (CombatSequence.StartBlock.AbilityBlockUnits.Num() == 0)
        {
            bHasEmptyBlocks = true;
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}의 StartBlock이 비어있습니다. 전투 시작 시 아무 행동도 하지 않을 수 있습니다.", DataSetIndex);
        }

        if (CombatSequence.ExitBlock.AbilityBlockUnits.Num() == 0)
        {
            bHasEmptyBlocks = true;
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}의 ExitBlock이 비어있습니다. 전투 종료 시 적절한 정리 작업이 이루어지지 않을 수 있습니다.", DataSetIndex);
        }

        // === 우선순위 분포 검증 ===
        for (int32 BlockIndex = 0; BlockIndex < CombatSequence.AbilityBlocks.Num(); ++BlockIndex)
        {
            const FAIAbilityBlock& Block = CombatSequence.AbilityBlocks[BlockIndex];

            if (Block.AbilityBlockUnits.Num() > 1)
            {
                // 모든 어빌리티가 같은 우선순위를 가지는지 확인
                int32 FirstPriority = Block.AbilityBlockUnits[0].Priority;
                bool bAllSamePriority = true;

                for (const FAIAbilityBlockUnit& Unit : Block.AbilityBlockUnits)
                {
                    if (Unit.Priority != FirstPriority)
                    {
                        bAllSamePriority = false;
                        break;
                    }
                }

                if (bAllSamePriority)
                {
                    UE_LOGFMT(LogAIDataSetInfos, Warning,
                        "AI {0}의 Block {1}에서 모든 어빌리티가 동일한 우선순위({2})를 가집니다. 실행 순서가 예측 불가능할 수 있습니다.",
                        DataSetIndex, BlockIndex, FirstPriority);
                }
            }
        }
    }

    return FAIDataSetValidationResult(); // 성공
}

void UAIDataSetInfos::ShowValidationError(const FAIDataSetValidationResult& ValidationResult) const
{
    // === 로그 시스템에 상세 오류 정보 기록 ===
    UE_LOGFMT(LogAIDataSetInfos, Error,
        "AI DataSet 검증 실패 | 오류 타입: {0} | 메시지: {1} | DataSet 인덱스: {2}",
        *UEnum::GetValueAsString(ValidationResult.ErrorType),
        *ValidationResult.ErrorMessage,
        ValidationResult.ProblemDataSetIndex);

    // === 개발자에게 즉각적인 피드백 제공 ===
    ensureMsgf(false, TEXT("AI DataSet 검증 실패: %s"), *ValidationResult.ErrorMessage);

    // === 화면에 시각적 경고 표시 ===
    if (GEngine)
    {
        FString ScreenMessage = FString::Printf(
            TEXT("AI DataSet 검증 실패: %s"),
            *ValidationResult.ErrorMessage
        );

        // 빨간색으로 7초간 화면에 표시
        GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Red, ScreenMessage);
    }
}

bool UAIDataSetInfos::TryAutoFixValidationError(const FAIDataSetValidationResult& ValidationResult)
{
    switch (ValidationResult.ErrorType)
    {
    case EAIDataSetValidationError::InvalidPersonalityValue:
    {
        // 기존 성향 값 자동 수정
        if (ValidationResult.ProblemDataSetIndex >= 0 && ValidationResult.ProblemDataSetIndex < AIDataSets.Num())
        {
            FAIDataSet& DataSet = const_cast<FAIDataSet&>(AIDataSets[ValidationResult.ProblemDataSetIndex]);
            FAIPersonalityTraits& Traits = DataSet.PersonalityTraits;
            FAIRewardCalculator& Calculator = DataSet.AdaptiveBehaviorConfig.RewardCalculator;
            FAIExperienceWeightConfig& WeightConfig = DataSet.AdaptiveBehaviorConfig.ExperienceWeightConfig;
            FAISimplifiedPersonality& SimplifiedPersonality = DataSet.AdaptiveBehaviorConfig.SimplifiedPersonality;

            bool bFixed = false;
            FString FixedValues;

            // === 기존 성향 특성 수정 ===
            if (Traits.Aggressiveness < 0.0f || Traits.Aggressiveness > 100.0f)
            {
                float OldValue = Traits.Aggressiveness;
                Traits.Aggressiveness = FMath::Clamp(Traits.Aggressiveness, 0.0f, 100.0f);
                FixedValues += FString::Printf(TEXT("공격성: %.2f->%.2f "), OldValue, Traits.Aggressiveness);
                bFixed = true;
            }

            if (Traits.Agility < 0.0f || Traits.Agility > 100.0f)
            {
                float OldValue = Traits.Agility;
                Traits.Agility = FMath::Clamp(Traits.Agility, 0.0f, 100.0f);
                FixedValues += FString::Printf(TEXT("기민함: %.2f->%.2f "), OldValue, Traits.Agility);
                bFixed = true;
            }

            if (Traits.Proactiveness < 0.0f || Traits.Proactiveness > 100.0f)
            {
                float OldValue = Traits.Proactiveness;
                Traits.Proactiveness = FMath::Clamp(Traits.Proactiveness, 0.0f, 100.0f);
                FixedValues += FString::Printf(TEXT("적극성: %.2f->%.2f "), OldValue, Traits.Proactiveness);
                bFixed = true;
            }

            if (Traits.Methodicalness < 0.0f || Traits.Methodicalness > 100.0f)
            {
                float OldValue = Traits.Methodicalness;
                Traits.Methodicalness = FMath::Clamp(Traits.Methodicalness, 0.0f, 100.0f);
                FixedValues += FString::Printf(TEXT("계획성: %.2f->%.2f "), OldValue, Traits.Methodicalness);
                bFixed = true;
            }

            // === 적응형 시스템 설정 자동 수정 ===
            if (Calculator.BaseLearningRate <= 0.0f || Calculator.BaseLearningRate > 0.5f)
            {
                float OldValue = Calculator.BaseLearningRate;
                Calculator.BaseLearningRate = FMath::Clamp(Calculator.BaseLearningRate, 0.01f, 0.15f);
                FixedValues += FString::Printf(TEXT("학습률: %.3f->%.3f "), OldValue, Calculator.BaseLearningRate);
                bFixed = true;
            }

            if (Calculator.LearningDecayFactor < 0.8f || Calculator.LearningDecayFactor >= 1.0f)
            {
                float OldValue = Calculator.LearningDecayFactor;
                Calculator.LearningDecayFactor = FMath::Clamp(Calculator.LearningDecayFactor, 0.9f, 0.99f);
                FixedValues += FString::Printf(TEXT("감쇠율: %.3f->%.3f "), OldValue, Calculator.LearningDecayFactor);
                bFixed = true;
            }

            if (WeightConfig.MaxWeightChange <= 0.0f || WeightConfig.MaxWeightChange > 1.0f)
            {
                float OldValue = WeightConfig.MaxWeightChange;
                WeightConfig.MaxWeightChange = FMath::Clamp(WeightConfig.MaxWeightChange, 0.15f, 0.5f);
                FixedValues += FString::Printf(TEXT("최대변화량: %.3f->%.3f "), OldValue, WeightConfig.MaxWeightChange);
                bFixed = true;
            }

            if (SimplifiedPersonality.Aggressiveness < 0.0f || SimplifiedPersonality.Aggressiveness > 100.0f)
            {
                float OldValue = SimplifiedPersonality.Aggressiveness;
                SimplifiedPersonality.Aggressiveness = FMath::Clamp(SimplifiedPersonality.Aggressiveness, 0.0f, 100.0f);
                FixedValues += FString::Printf(TEXT("적응형공격성: %.2f->%.2f "), OldValue, SimplifiedPersonality.Aggressiveness);
                bFixed = true;
            }

            if (SimplifiedPersonality.Agility < 0.0f || SimplifiedPersonality.Agility > 100.0f)
            {
                float OldValue = SimplifiedPersonality.Agility;
                SimplifiedPersonality.Agility = FMath::Clamp(SimplifiedPersonality.Agility, 0.0f, 100.0f);
                FixedValues += FString::Printf(TEXT("적응형기민함: %.2f->%.2f "), OldValue, SimplifiedPersonality.Agility);
                bFixed = true;
            }

            if (bFixed)
            {
                UE_LOGFMT(LogAIDataSetInfos, Log,
                    "자동 수정: AI DataSet {0}의 설정값들을 유효 범위로 조정했습니다. {1}",
                    ValidationResult.ProblemDataSetIndex, *FixedValues);
                return true;
            }
        }
        break;
    }

    case EAIDataSetValidationError::InvalidExecutionPriority:
    case EAIDataSetValidationError::InvalidAbilityPriority:
    {
        UE_LOGFMT(LogAIDataSetInfos, Log,
            "자동 수정 시도: 우선순위 오류는 수동 검토가 필요합니다. DataSet {0}을 확인하세요.",
            ValidationResult.ProblemDataSetIndex);
        return false;
    }

    default:
    {
        UE_LOGFMT(LogAIDataSetInfos, Log,
            "자동 수정 불가: {0} 오류는 수동 수정이 필요합니다. DataSet {1}을 검토하세요.",
            *UEnum::GetValueAsString(ValidationResult.ErrorType),
            ValidationResult.ProblemDataSetIndex);
        return false;
    }
    }

    return false;
}

void UAIDataSetInfos::ValidateAIDataSetsManually()
{
    FAIDataSetValidationResult ValidationResult = ValidateAIDataSets();

    if (ValidationResult.bIsValid)
    {
        UE_LOGFMT(LogAIDataSetInfos, Log, "✅ AI DataSet 무결성 검사 성공: 모든 설정이 유효합니다");

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                TEXT("✅ AI DataSet 검증 성공! 모든 AI 설정이 올바릅니다."));
        }
    }
    else
    {
        ShowValidationError(ValidationResult);
    }
}

void UAIDataSetInfos::ValidateCombatSequencesOnly()
{
    FAIDataSetValidationResult Result = ValidateCombatSequences();

    if (Result.bIsValid)
    {
        UE_LOGFMT(LogAIDataSetInfos, Log, "✅ 전투 시퀀스 검증 완료: 모든 전투 패턴이 유효합니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ 전투 시퀀스 검증 성공!"));
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}

void UAIDataSetInfos::ValidatePersonalityTraitsOnly()
{
    FAIDataSetValidationResult Result = ValidatePersonalityTraits();

    if (Result.bIsValid)
    {
        UE_LOGFMT(LogAIDataSetInfos, Log, "✅ 성향 특성 검증 완료: 모든 AI 성향이 유효합니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ AI 성향 특성 검증 성공!"));
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}

void UAIDataSetInfos::ValidateAdaptiveSystemOnly()
{
    FAIDataSetValidationResult Result = ValidateAdaptiveSystemSettings();

    if (Result.bIsValid)
    {
        // 적응형 시스템이 활성화된 AI 개수 확인
        int32 AdaptiveEnabledCount = 0;
        for (const FAIDataSet& DataSet : AIDataSets)
        {
            if (DataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
            {
                AdaptiveEnabledCount++;
            }
        }

        UE_LOGFMT(LogAIDataSetInfos, Log,
            "✅ 적응형 시스템 검증 완료: {0}개 AI의 적응형 설정이 유효합니다",
            AdaptiveEnabledCount);

        if (GEngine)
        {
            FString Message = FString::Printf(
                TEXT("✅ 적응형 시스템 검증 성공! (%d개 AI 설정 유효)"),
                AdaptiveEnabledCount
            );
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Message);
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}

void UAIDataSetInfos::SortAllAbilityBlocksByPriority()
{
    bool bAnySorted = false;

    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        FAIDataSet& DataSet = const_cast<FAIDataSet&>(AIDataSets[DataSetIndex]);
        FAICombatSequence& CombatSequence = DataSet.CombatSequence;

        // StartBlock 정렬
        if (CombatSequence.StartBlock.AbilityBlockUnits.Num() > 1)
        {
            CombatSequence.StartBlock.SortAbilityBlockUnitsByPriority();
            bAnySorted = true;
        }

        // AbilityBlocks 정렬
        for (FAIAbilityBlock& Block : CombatSequence.AbilityBlocks)
        {
            if (Block.AbilityBlockUnits.Num() > 1)
            {
                Block.SortAbilityBlockUnitsByPriority();
                bAnySorted = true;
            }
        }

        // ExitBlock 정렬
        if (CombatSequence.ExitBlock.AbilityBlockUnits.Num() > 1)
        {
            CombatSequence.ExitBlock.SortAbilityBlockUnitsByPriority();
            bAnySorted = true;
        }
    }

    if (bAnySorted)
    {
        UE_LOGFMT(LogAIDataSetInfos, Log, "모든 AI DataSet의 어빌리티 블록을 우선순위 순으로 정렬했습니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("어빌리티 블록 우선순위 정렬 완료!"));
        }
    }
    else
    {
        UE_LOGFMT(LogAIDataSetInfos, Log, "정렬할 어빌리티 블록이 없습니다");
    }
}

#endif
#pragma endregion

#pragma region Utility
void UAIDataSetInfos::ApplyConservativeLearningPreset()
{
    for (FAIDataSet& DataSet : AIDataSets)
    {
        if (DataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
        {
            // 보수적 학습 설정
            FAIExperienceWeightConfig& Config = DataSet.AdaptiveBehaviorConfig.ExperienceWeightConfig;
            Config.MaxWeightChange = 0.15f; // 15% 제한
            Config.MinSingleChange = 0.005f; // 아주 작은 변화
            Config.MinCategoryProbability = 0.1f; // 10% 최소 보장
            Config.EarlyStopThreshold = 40; // 늦은 중단
            Config.AdaptiveResetPercentage = 0.05f; // 5% 리셋

            // 학습률도 보수적으로
            FAIRewardCalculator& Reward = DataSet.AdaptiveBehaviorConfig.RewardCalculator;
            Reward.BaseLearningRate = 0.05f;
            Reward.LearningDecayFactor = 0.98f;

            UE_LOGFMT(LogAIDataSetInfos, Log,
                "AI {0}에 보수적 학습 프리셋 적용 완료",
                *UEnum::GetValueAsString(DataSet.AIType));
        }
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,
            TEXT("보수적 학습 프리셋 적용 - 안정적이고 점진적인 적응"));
    }
}

void UAIDataSetInfos::ApplyAggressiveLearningPreset()
{
    for (FAIDataSet& DataSet : AIDataSets)
    {
        if (DataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
        {
            // 공격적 학습 설정
            FAIExperienceWeightConfig& Config = DataSet.AdaptiveBehaviorConfig.ExperienceWeightConfig;
            Config.MaxWeightChange = 0.4f; // 40% 제한
            Config.MinSingleChange = 0.02f; // 큰 변화
            Config.MinCategoryProbability = 0.03f; // 3% 최소 보장
            Config.EarlyStopThreshold = 15; // 빠른 중단
            Config.AdaptiveResetPercentage = 0.2f; // 20% 리셋

            // 학습률도 공격적으로
            FAIRewardCalculator& Reward = DataSet.AdaptiveBehaviorConfig.RewardCalculator;
            Reward.BaseLearningRate = 0.12f;
            Reward.LearningDecayFactor = 0.92f;

            UE_LOGFMT(LogAIDataSetInfos, Log,
                "AI {0}에 공격적 학습 프리셋 적용 완료",
                *UEnum::GetValueAsString(DataSet.AIType));
        }
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange,
            TEXT("공격적 학습 프리셋 적용 - 빠르고 대폭적인 적응"));
    }
}
#pragma endregion