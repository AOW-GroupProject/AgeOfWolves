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
#pragma endregion

};