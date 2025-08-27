#include "AIDataSetInfos.h"
#include "Logging/StructuredLog.h"

#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDeviceRedirector.h"

#include "BehaviorTree/BehaviorTree.h"

DEFINE_LOG_CATEGORY(LogAIDataSetInfos)

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
    for (int32 DataSetIndex = 0; DataSetIndex < AIDataSets.Num(); ++DataSetIndex)
    {
        const FAIDataSet& DataSet = AIDataSets[DataSetIndex];

        if (!DataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
        {
            continue; // 적응형 시스템이 비활성화된 AI는 건너뛰기
        }

        const FAIRewardCalculator& Calculator = DataSet.AdaptiveBehaviorConfig.RewardCalculator;
        const FAIExperienceWeightConfig& WeightConfig = DataSet.AdaptiveBehaviorConfig.ExperienceWeightConfig;
        const FAISimplifiedPersonality& Personality = DataSet.AdaptiveBehaviorConfig.SimplifiedPersonality;

        // === 보상 계산기 검증 ===
        if (Calculator.BaseLearningRate <= 0.0f || Calculator.BaseLearningRate > 0.5f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 기본 학습률: AI %d (값: %.3f, 권장: 0.01-0.5)"),
                    DataSetIndex, Calculator.BaseLearningRate),
                DataSetIndex
            );
        }

        if (Calculator.LearningDecayFactor < 0.8f || Calculator.LearningDecayFactor >= 1.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 학습률 감쇠 인수: AI %d (값: %.3f, 권장: 0.8-0.99)"),
                    DataSetIndex, Calculator.LearningDecayFactor),
                DataSetIndex
            );
        }

        if (Calculator.OptimalCombatDistance < 100.0f || Calculator.OptimalCombatDistance > 1000.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 최적 전투 거리: AI %d (값: %.1f, 권장: 100-1000)"),
                    DataSetIndex, Calculator.OptimalCombatDistance),
                DataSetIndex
            );
        }

        // === 경험 가중치 설정 검증 ===
        if (WeightConfig.MaxWeightChange <= 0.0f || WeightConfig.MaxWeightChange > 1.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 최대 가중치 변화량: AI %d (값: %.3f, 권장: 0.1-0.5)"),
                    DataSetIndex, WeightConfig.MaxWeightChange),
                DataSetIndex
            );
        }

        if (WeightConfig.MinSingleChange <= 0.0f || WeightConfig.MinSingleChange > 0.1f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 최소 단일 변화량: AI %d (값: %.4f, 권장: 0.005-0.05)"),
                    DataSetIndex, WeightConfig.MinSingleChange),
                DataSetIndex
            );
        }

        if (WeightConfig.MinCategoryProbability <= 0.0f || WeightConfig.MinCategoryProbability > 0.5f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 최소 카테고리 확률: AI %d (값: %.3f, 권장: 0.01-0.2)"),
                    DataSetIndex, WeightConfig.MinCategoryProbability),
                DataSetIndex
            );
        }

        if (WeightConfig.EarlyStopThreshold < 5 || WeightConfig.EarlyStopThreshold > 100)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 조기 중단 임계값: AI %d (값: %d, 권장: 10-50)"),
                    DataSetIndex, WeightConfig.EarlyStopThreshold),
                DataSetIndex
            );
        }

        if (WeightConfig.AdaptiveResetPercentage <= 0.0f || WeightConfig.AdaptiveResetPercentage > 0.5f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("부적절한 적응적 리셋 비율: AI %d (값: %.3f, 권장: 0.05-0.3)"),
                    DataSetIndex, WeightConfig.AdaptiveResetPercentage),
                DataSetIndex
            );
        }

        // === 간소화된 성향 검증 ===
        if (Personality.Aggressiveness < 0.0f || Personality.Aggressiveness > 100.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("적응형 공격성 값이 유효 범위를 벗어남: AI %d (값: %.2f, 권장: 0-100)"),
                    DataSetIndex, Personality.Aggressiveness),
                DataSetIndex
            );
        }

        if (Personality.Agility < 0.0f || Personality.Agility > 100.0f)
        {
            return FAIDataSetValidationResult(
                EAIDataSetValidationError::InvalidPersonalityValue,
                FString::Printf(TEXT("적응형 기민함 값이 유효 범위를 벗어남: AI %d (값: %.2f, 권장: 0-100)"),
                    DataSetIndex, Personality.Agility),
                DataSetIndex
            );
        }

        // === 보상 가중치 밸런스 확인 (경고) ===
        float TotalWeight = Calculator.DamageWeight + Calculator.DistanceWeight + Calculator.SuccessWeight;
        if (TotalWeight < 1.0f || TotalWeight > 6.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}: 보상 가중치 총합이 권장 범위를 벗어남 (합계: {1}, 권장: 1.0-6.0). 정규화에 영향을 줄 수 있습니다.",
                DataSetIndex, TotalWeight);
        }

        // === 성향 조합 경고 ===
        if (Personality.Aggressiveness > 80.0f && Personality.Agility < 20.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}: 극도로 공격적이지만 기민함이 낮습니다 (공격성: {1}, 기민함: {2}). 적응 과정에서 불균형이 발생할 수 있습니다.",
                DataSetIndex, Personality.Aggressiveness, Personality.Agility);
        }
        else if (Personality.Agility > 80.0f && Personality.Aggressiveness < 20.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}: 극도로 기민하지만 공격성이 낮습니다 (기민함: {1}, 공격성: {2}). 소극적인 AI가 될 수 있습니다.",
                DataSetIndex, Personality.Agility, Personality.Aggressiveness);
        }

        // === 학습 복잡도 경고 ===
        float LearningComplexity = EvaluateLearningComplexity(DataSet.AIType);
        if (LearningComplexity > 7.0f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}: 학습 복잡도가 매우 높습니다 (복잡도: {1}/10). 성능 저하나 불안정한 학습이 발생할 수 있습니다.",
                DataSetIndex, LearningComplexity);
        }

        // === 학습률과 변화량 조합 검증 ===
        if (Calculator.BaseLearningRate > 0.1f && WeightConfig.MinSingleChange > 0.02f)
        {
            UE_LOGFMT(LogAIDataSetInfos, Warning,
                "AI {0}: 높은 학습률({1})과 큰 변화량({2})의 조합으로 과도하게 급진적인 학습이 될 수 있습니다.",
                DataSetIndex, Calculator.BaseLearningRate, WeightConfig.MinSingleChange);
        }
    }

    return FAIDataSetValidationResult(); // 검증 성공
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