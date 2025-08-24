#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AIDataSetInfos.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAIDataSetInfos, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBehaviorTree;
class UGameplayAbility;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EAIType
*
*	AI 유형을 열거형으로 정의합니다.
*/
UENUM(BlueprintType)
enum class EAIType : uint8
{
    Wolf = 0		UMETA(DisplayName = "Wolf"),
    KuroOni			UMETA(DisplayName = "Kuro Oni"),
    Ninja           UMETA(DisplayName = "Ninja"),
    MAX,
};

/**
 * @EAIDataSetValidationError
 *
 * AI DataSet 검증 과정에서 발생할 수 있는 오류 유형을 정의합니다.
 * 각 오류 타입은 AI 시스템의 특정한 문제점을 나타냅니다.
 */
UENUM(BlueprintType)
enum class EAIDataSetValidationError : uint8
{
    None                            UMETA(DisplayName = "오류 없음"),

    // === 기본 설정 관련 오류 ===
    EmptyDataSet                    UMETA(DisplayName = "비어있는 데이터셋"),
    InvalidAIType                   UMETA(DisplayName = "유효하지 않은 AI 타입"),
    DuplicateAIType                 UMETA(DisplayName = "중복된 AI 타입"),

    // === BehaviorTree 관련 오류 ===
    NullBehaviorTree                UMETA(DisplayName = "Null BehaviorTree"),
    InvalidBehaviorTree             UMETA(DisplayName = "유효하지 않은 BehaviorTree"),

    // === 블록 관련 오류 ===
    InvalidBlockIdentifier          UMETA(DisplayName = "유효하지 않은 블록 식별자"),
    DuplicateBlockIdentifier        UMETA(DisplayName = "중복된 블록 식별자"),
    InvalidExecutionPriority        UMETA(DisplayName = "유효하지 않은 실행 우선순위"),

    // === 어빌리티 관련 오류 ===
    InvalidAbilityTag               UMETA(DisplayName = "유효하지 않은 어빌리티 태그"),
    DuplicateAbilityTagInBlock      UMETA(DisplayName = "블록 내 중복된 어빌리티 태그"),
    InvalidAbilityPriority          UMETA(DisplayName = "유효하지 않은 어빌리티 우선순위"),

    // === 성향 특성 관련 오류 ===
    InvalidPersonalityValue         UMETA(DisplayName = "유효하지 않은 성향 값"),

    // === 논리적 일관성 오류 ===
    LogicalInconsistency            UMETA(DisplayName = "논리적 불일치")
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 * @FAIAbilityBlockUnit
 * 어빌리티 블록 내의 개별 어빌리티 정보
 */
USTRUCT(BlueprintType)
struct FAIAbilityBlockUnit
{
    GENERATED_BODY()

public:
    FAIAbilityBlockUnit()
        : AbilityTag(FGameplayTag())
        , Priority(0)
    {
    }

public:
    //@어빌리티 식별용 태그
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티 정보")
    FGameplayTag AbilityTag;

    //@실행 우선순위 (낮을수록 먼저 실행)
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티 정보")
    int32 Priority;

public:
    bool operator==(const FGameplayTag& InTag) const
    {
        return AbilityTag.MatchesTagExact(InTag);
    }

    FORCEINLINE FGameplayTag GetAbilityTag() const { return AbilityTag; }
    FORCEINLINE int32 GetPriority() const { return Priority; }
};

/**
 * @FAIAbilityBlock
 * AI 행동 단위를 구성하는 어빌리티들의 집합
 */
USTRUCT(BlueprintType)
struct FAIAbilityBlock
{
    GENERATED_BODY()

public:
    FAIAbilityBlock()
        : BlockIdentifier(FGameplayTag())
        , ExecutionPriority(0)
        , AbilityBlockUnits()
    {
    }

public:
    //@블록 식별 태그
    UPROPERTY(EditDefaultsOnly, Category = "블록 기본정보")
    FGameplayTag BlockIdentifier;

    //@블록 실행 우선순위 (낮을수록 먼저 실행)
    UPROPERTY(EditDefaultsOnly, Category = "블록 기본정보")
    int32 ExecutionPriority;

    //@포함된 어빌리티 목록
    UPROPERTY(EditDefaultsOnly, Category = "포함 어빌리티")
    TArray<FAIAbilityBlockUnit> AbilityBlockUnits;

public:
    FORCEINLINE FGameplayTag GetBlockIdentifier() const { return BlockIdentifier; }
    FORCEINLINE int32 GetExecutionPriority() const { return ExecutionPriority; }
    FORCEINLINE const TArray<FAIAbilityBlockUnit>& GetAbilityBlockUnits() const { return AbilityBlockUnits; }

    const FAIAbilityBlockUnit* FindAbilityBlockUnitByTag(const FGameplayTag& InTag) const
    {
        return AbilityBlockUnits.FindByPredicate([InTag](const FAIAbilityBlockUnit& Unit)
            {
                return Unit == InTag;
            });
    }

    void SortAbilityBlockUnitsByPriority()
    {
        AbilityBlockUnits.Sort([](const FAIAbilityBlockUnit& A, const FAIAbilityBlockUnit& B)
            {
                return A.GetPriority() < B.GetPriority();
            });
    }
};

/**
 * @FAICombatSequence
 * AI의 전투 패턴 시퀀스 정보
 */
USTRUCT(BlueprintType)
struct FAICombatSequence
{
    GENERATED_BODY()

public:
    FAICombatSequence()
        : StartBlock()
        , AbilityBlocks()
        , ExitBlock()
    {
    }

public:
    //@전투 시작 시 실행할 블록
    UPROPERTY(EditDefaultsOnly, Category = "전투 시퀀스 구성")
    FAIAbilityBlock StartBlock;

    //@주요 전투 패턴 블록들
    UPROPERTY(EditDefaultsOnly, Category = "전투 시퀀스 구성")
    TArray<FAIAbilityBlock> AbilityBlocks;

    //@전투 종료 시 실행할 블록
    UPROPERTY(EditDefaultsOnly, Category = "전투 시퀀스 구성")
    FAIAbilityBlock ExitBlock;

public:
    FORCEINLINE const FAIAbilityBlock& GetStartBlock() const { return StartBlock; }
    FORCEINLINE const TArray<FAIAbilityBlock>& GetAbilityBlocks() const { return AbilityBlocks; }
    FORCEINLINE const FAIAbilityBlock& GetExitBlock() const { return ExitBlock; }
};

/**
 * @FAIPersonalityTraits
 * AI 캐릭터의 성향 및 행동 특성
 */
USTRUCT(BlueprintType)
struct FAIPersonalityTraits
{
    GENERATED_BODY()

public:
    //@공격성: 연속 공격 횟수와 공격적 행동에 영향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI 성향 설정",
        meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
    float Aggressiveness = 50.0f;

    //@기민함: 회피 행동과 기동성에 영향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI 성향 설정",
        meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
    float Agility = 50.0f;

    //@적극성: 선제적 방어 및 준비 행동에 영향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI 성향 설정",
        meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
    float Proactiveness = 50.0f;

    //@계획성: 상황별 전략 변경에 영향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI 성향 설정",
        meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
    float Methodicalness = 50.0f;

public:
    //@공격성에 따른 공격 유닛 개수 계산
    int32 CalculateAttackUnitsCount(int32 BaseCount) const
    {
        float Multiplier = 1.0f + (Aggressiveness / 100.0f);
        return FMath::Max(1, FMath::RoundToInt(BaseCount * Multiplier));
    }

    //@기민함에 따른 회피 확률 계산
    float CalculateDodgeProbability() const
    {
        return 0.1f + (Agility / 100.0f * 0.7f);
    }

    //@적극성에 따른 선제적 가드 사용 여부
    bool ShouldUseProactiveGuard() const
    {
        float GuardProbability = Proactiveness / 100.0f;
        return FMath::FRand() < GuardProbability;
    }

    //@계획성에 따른 전략 변경 여부
    bool ShouldChangeStrategyOnCancel() const
    {
        float ChangeStrategyProbability = Methodicalness / 100.0f;
        return FMath::FRand() < ChangeStrategyProbability;
    }
};

/**
 * @FAISimplifiedPersonality
 * 2가지 핵심 성향으로 간소화된 AI 성격 시스템
 */
USTRUCT(BlueprintType)
struct FAISimplifiedPersonality
{
    GENERATED_BODY()

public:
    FAISimplifiedPersonality()
        : Aggressiveness(50.0f)
        , Agility(50.0f)
    {
    }

public:
    //@공격성: Skills 계열 행동에 주로 영향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "간소화된 AI 성향",
        meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100",
            ToolTip = "공격적인 행동의 선호도에 영향을 줍니다"))
    float Aggressiveness;

    //@기민함: StrafeOrDodge 계열 행동에 주로 영향  
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "간소화된 AI 성향",
        meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100",
            ToolTip = "회피 및 기동성 행동의 선호도에 영향을 줍니다"))
    float Agility;

public:
    //@특정 행동 카테고리에 대한 초기 선호도 계산
    float GetInitialActionPreference(const FGameplayTag& ActionCategory) const
    {
        if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"))))
        {
            return (Aggressiveness + Agility) * 0.5f; // 균등 가중치
        }
        else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"))))
        {
            return Aggressiveness * 0.8f + Agility * 0.2f; // 공격성 위주
        }
        else if (ActionCategory.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))))
        {
            return Agility * 0.8f + Aggressiveness * 0.2f; // 기민함 위주
        }
        return 50.0f; // 기본값
    }
};

/**
 * @FAIRewardCalculator
 * 3가지 보상 요소의 정규화 가중합 계산 및 학습률 관리
 */
USTRUCT(BlueprintType)
struct FAIRewardCalculator
{
    GENERATED_BODY()

public:
    FAIRewardCalculator()
        : DamageWeight(1.0f)
        , DistanceWeight(0.7f)
        , SuccessWeight(0.8f)
        , BaseLearningRate(0.08f)
        , LearningDecayFactor(0.95f)
        , OptimalCombatDistance(300.0f)
    {
    }

public:
    //@데미지 받음에 대한 가중치 (음의 보상)
    UPROPERTY(EditAnywhere, Category = "보상 가중치",
        meta = (ClampMin = "0.1", ClampMax = "2.0", ToolTip = "받은 데미지에 대한 패널티 가중치"))
    float DamageWeight;

    //@거리 유지에 대한 가중치
    UPROPERTY(EditAnywhere, Category = "보상 가중치",
        meta = (ClampMin = "0.1", ClampMax = "2.0", ToolTip = "최적 거리 유지에 대한 보상 가중치"))
    float DistanceWeight;

    //@어빌리티 성공에 대한 가중치
    UPROPERTY(EditAnywhere, Category = "보상 가중치",
        meta = (ClampMin = "0.1", ClampMax = "2.0", ToolTip = "어빌리티 성공에 대한 보상 가중치"))
    float SuccessWeight;

    //@기본 학습률
    UPROPERTY(EditAnywhere, Category = "학습 파라미터",
        meta = (ClampMin = "0.01", ClampMax = "0.5", ToolTip = "초기 학습률"))
    float BaseLearningRate;

    //@학습률 감쇠 팩터
    UPROPERTY(EditAnywhere, Category = "학습 파라미터",
        meta = (ClampMin = "0.8", ClampMax = "0.99", ToolTip = "시간에 따른 학습률 감소율"))
    float LearningDecayFactor;

    //@최적 전투 거리
    UPROPERTY(EditAnywhere, Category = "전투 파라미터",
        meta = (ClampMin = "100.0", ClampMax = "1000.0", ToolTip = "AI가 선호하는 적과의 거리"))
    float OptimalCombatDistance;

public:
    //@정규화된 가중합 보상 계산
    float CalculateNormalizedReward(float DamageReceived, float CurrentDistance, bool bAbilitySuccess) const
    {
        // 1. 데미지 보상 정규화 (0-100 데미지 → 0~-1)
        float NormalizedDamage = -FMath::Clamp(DamageReceived / 100.0f, 0.0f, 1.0f);

        // 2. 거리 보상 정규화 (최적거리와의 차이 → -1~1)
        float DistanceDiff = FMath::Abs(CurrentDistance - OptimalCombatDistance);
        float MaxAcceptableDistance = OptimalCombatDistance * 2.0f;
        float NormalizedDistance = FMath::Max(0.0f, 1.0f - (DistanceDiff / MaxAcceptableDistance));

        // 3. 성공 보상 (이진값)
        float NormalizedSuccess = bAbilitySuccess ? 1.0f : -1.0f;

        // 가중합 계산 및 정규화
        float TotalWeight = DamageWeight + DistanceWeight + SuccessWeight;
        float WeightedSum = (NormalizedDamage * DamageWeight) +
            (NormalizedDistance * DistanceWeight) +
            (NormalizedSuccess * SuccessWeight);

        return FMath::Clamp(WeightedSum / TotalWeight, -1.0f, 1.0f);
    }

    //@적응적 학습률 계산
    float CalculateAdaptiveLearningRate(int32 LearningCount) const
    {
        float AdaptiveRate = BaseLearningRate * FMath::Pow(LearningDecayFactor, LearningCount / 10.0f);
        return FMath::Clamp(AdaptiveRate, 0.01f, 0.15f); // 최소/최대 학습률 제한
    }
};

/**
 * @FAIAdaptiveBehaviorConfig
 * 적응형 행동 시스템의 설정 정보 (에디터에서 설정)
 */
USTRUCT(BlueprintType)
struct FAIAdaptiveBehaviorConfig
{
    GENERATED_BODY()

public:
    FAIAdaptiveBehaviorConfig()
        : bEnableAdaptiveLearning(false)
        , SimplifiedPersonality()
        , RewardCalculator()
    {
    }

public:
    //@적응형 학습 시스템 활성화 여부
    UPROPERTY(EditAnywhere, Category = "적응형 시스템 활성화",
        meta = (ToolTip = "체크하면 이 AI가 플레이어 패턴에 적응하여 학습합니다"))
    bool bEnableAdaptiveLearning;

    //@간소화된 성향 시스템 (적응형 시스템 활성화 시 사용)
    UPROPERTY(EditAnywhere, Category = "적응형 성향",
        meta = (EditCondition = "bEnableAdaptiveLearning",
            ToolTip = "기존 4가지 성향을 2가지로 간소화한 시스템"))
    FAISimplifiedPersonality SimplifiedPersonality;

    //@보상 계산 시스템
    UPROPERTY(EditAnywhere, Category = "적응형 보상 시스템",
        meta = (EditCondition = "bEnableAdaptiveLearning",
            ToolTip = "AI가 행동 결과를 어떻게 평가할지 결정"))
    FAIRewardCalculator RewardCalculator;
};

/**
 * @FAIDataSet
 * AI의 기본 정보와 행동 패턴 설정
 */
USTRUCT(BlueprintType)
struct FAIDataSet
{
    GENERATED_BODY()

public:
    //@AI 캐릭터 유형
    UPROPERTY(EditDefaultsOnly, Category = "AI 기본 정보")
    EAIType AIType;

    //@사용할 행동 트리
    UPROPERTY(EditDefaultsOnly, Category = "AI 행동 설정")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    //@전투 시 사용할 패턴 시퀀스
    UPROPERTY(EditDefaultsOnly, Category = "AI 전투 설정")
    FAICombatSequence CombatSequence;

    //@AI 성향 특성
    UPROPERTY(EditDefaultsOnly, Category = "AI 성향 설정")
    FAIPersonalityTraits PersonalityTraits;

    //@적응형 행동 시스템 설정 (실험적 기능)
    UPROPERTY(EditDefaultsOnly, Category = "AI 적응형 시스템 (실험적)",
        meta = (ToolTip = "새로운 적응형 학습 시스템 설정"))
    FAIAdaptiveBehaviorConfig AdaptiveBehaviorConfig;
};

/**
 * @FAIDataSetValidationResult
 *
 * AI DataSet 검증 결과를 담는 구조체입니다.
 * 어떤 오류가 어디서 발생했는지 상세한 정보를 제공합니다.
 */
USTRUCT(BlueprintType)
struct FAIDataSetValidationResult
{
    GENERATED_BODY()

public:
    // 검증 결과 (성공/실패)
    UPROPERTY(BlueprintReadOnly)
    bool bIsValid = true;

    // 오류 유형
    UPROPERTY(BlueprintReadOnly)
    EAIDataSetValidationError ErrorType = EAIDataSetValidationError::None;

    // 오류 메시지
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    // 문제가 발생한 AI DataSet 인덱스
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemDataSetIndex = -1;

    // 기본 생성자 (성공 케이스)
    FAIDataSetValidationResult()
    {
    }

    // 오류 케이스 생성자
    FAIDataSetValidationResult(EAIDataSetValidationError InErrorType, const FString& InErrorMessage,
        int32 InDataSetIndex = -1)
        : bIsValid(false)
        , ErrorType(InErrorType)
        , ErrorMessage(InErrorMessage)
        , ProblemDataSetIndex(InDataSetIndex)
    {
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAIDataSetInfos
 *
 *	AI 데이터 정보를 담은 Data Asset
 *  에디터에서 실시간 무결성 검사를 통해 AI 설정 오류를 방지합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAIDataSetInfos : public UPrimaryDataAsset
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UAIDataSetInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
    // 에디터에서 프로퍼티가 변경될 때 호출되는 함수
    // AI 설정이 변경되는 즉시 무결성 검사를 수행합니다
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // 배열이나 구조체 내부의 프로퍼티가 변경될 때 호출되는 함수
    // AI DataSets 배열이나 CombatSequence 구조체의 변경사항을 감지합니다
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    UPROPERTY(EditDefaultsOnly, Category = "AI 정보 목록")
    TArray<FAIDataSet> AIDataSets;
#pragma endregion

#if WITH_EDITOR
protected:
    // === 무결성 검사 핵심 함수들 ===
    // 각 함수는 AI 시스템의 특정 영역에 대한 검증을 담당합니다

    // 전체 무결성 검사를 수행하는 마스터 함수
    FAIDataSetValidationResult ValidateAIDataSets() const;

    // === 개별 검증 함수들 ===
    // 각 함수는 특정한 무결성 규칙을 담당합니다

    // 기본 설정 검증 (빈 배열, AI 타입 등)
    FAIDataSetValidationResult ValidateBasicSettings() const;

    // 개별 AI DataSet 검증 (BehaviorTree, AI 타입 등)
    FAIDataSetValidationResult ValidateIndividualAIDataSets() const;

    // AI 타입 중복 검증
    FAIDataSetValidationResult ValidateAITypeUniqueness() const;

    // 전투 시퀀스 관련 검증
    FAIDataSetValidationResult ValidateCombatSequences() const;

    // 개별 어빌리티 블록 검증 헬퍼 함수
    FAIDataSetValidationResult ValidateSingleAbilityBlock(const FAIAbilityBlock& Block,
        int32 DataSetIndex, const FString& BlockName) const;

    // 블록 식별자 중복 검증
    FAIDataSetValidationResult ValidateBlockIdentifierUniqueness(const FAICombatSequence& CombatSequence,
        int32 DataSetIndex) const;

    // 어빌리티 블록 상세 검증
    FAIDataSetValidationResult ValidateAbilityBlocks() const;

    // 성향 특성 검증
    FAIDataSetValidationResult ValidatePersonalityTraits() const;

    // 논리적 일관성 검증
    FAIDataSetValidationResult ValidateLogicalConsistency() const;

    // 검증 실패 시 사용자에게 알림을 표시하는 함수
    void ShowValidationError(const FAIDataSetValidationResult& ValidationResult) const;

    // 문제가 있는 설정을 자동으로 수정하려고 시도하는 함수
    bool TryAutoFixValidationError(const FAIDataSetValidationResult& ValidationResult);

public:
    // === 에디터에서 수동으로 실행할 수 있는 검증 함수들 ===

    // 전체 무결성 검사를 수동으로 실행
    UFUNCTION(CallInEditor, Category = "AI DataSet Validation")
    void ValidateAIDataSetsManually();

    // 개별 검증 함수들을 에디터에서 직접 호출할 수 있는 함수들
    UFUNCTION(CallInEditor, Category = "AI DataSet Validation")
    void ValidateCombatSequencesOnly();

    UFUNCTION(CallInEditor, Category = "AI DataSet Validation")
    void ValidatePersonalityTraitsOnly();

    // === 유틸리티 함수들 ===

    // 모든 어빌리티 블록의 우선순위를 자동으로 정렬
    UFUNCTION(CallInEditor, Category = "AI DataSet Utilities")
    void SortAllAbilityBlocksByPriority();

#endif

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    // AI 타입으로 특정 AI DataSet 찾기
    const FAIDataSet* FindAIDataSetByType(EAIType AIType) const
    {
        return AIDataSets.FindByPredicate([AIType](const FAIDataSet& DataSet)
            {
                return DataSet.AIType == AIType;
            });
    }

    // AI 타입으로 특정 AI DataSet 찾기 (수정 가능한 버전)
    FAIDataSet* FindAIDataSetByType(EAIType AIType)
    {
        return AIDataSets.FindByPredicate([AIType](const FAIDataSet& DataSet)
            {
                return DataSet.AIType == AIType;
            });
    }

    // 전체 AI DataSet 목록 반환
    const TArray<FAIDataSet>& GetAIDataSets() const { return AIDataSets; }

    // 특정 인덱스의 AI DataSet 반환 (안전한 접근)
    const FAIDataSet* GetAIDataSetAtIndex(int32 Index) const
    {
        return AIDataSets.IsValidIndex(Index) ? &AIDataSets[Index] : nullptr;
    }

    // AI DataSet 개수 반환
    int32 GetAIDataSetCount() const { return AIDataSets.Num(); }

    // 특정 AI 타입이 존재하는지 확인
    bool HasAIType(EAIType AIType) const
    {
        return FindAIDataSetByType(AIType) != nullptr;
    }


    //@기존 PersonalityTraits를 새 시스템으로 변환
    UFUNCTION(BlueprintCallable, Category = "AI 적응형 시스템")
    FAISimplifiedPersonality ConvertLegacyPersonalityToSimplified(EAIType AIType) const
    {
        const FAIDataSet* DataSet = FindAIDataSetByType(AIType);
        if (!DataSet)
        {
            return FAISimplifiedPersonality();
        }

        FAISimplifiedPersonality SimplifiedPersonality;
        const FAIPersonalityTraits& Legacy = DataSet->PersonalityTraits;

        // 4가지를 2가지로 변환
        SimplifiedPersonality.Aggressiveness = (Legacy.Aggressiveness + Legacy.Proactiveness) * 0.5f;
        SimplifiedPersonality.Agility = (Legacy.Agility + Legacy.Methodicalness) * 0.5f;

        return SimplifiedPersonality;
    }

    //@적응형 시스템이 활성화되어 있는지 확인
    UFUNCTION(BlueprintCallable, Category = "AI 적응형 시스템")
    bool IsAdaptiveSystemEnabled(EAIType AIType) const
    {
        const FAIDataSet* DataSet = FindAIDataSetByType(AIType);
        return DataSet && DataSet->AdaptiveBehaviorConfig.bEnableAdaptiveLearning;
    }

    //@특정 AI의 적응형 설정 가져오기
    UFUNCTION(BlueprintCallable, Category = "AI 적응형 시스템")
    FAIAdaptiveBehaviorConfig GetAdaptiveBehaviorConfig(EAIType AIType) const
    {
        const FAIDataSet* DataSet = FindAIDataSetByType(AIType);
        return DataSet ? DataSet->AdaptiveBehaviorConfig : FAIAdaptiveBehaviorConfig();
    }
#pragma endregion

};