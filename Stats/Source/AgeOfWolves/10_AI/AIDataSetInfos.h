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
#pragma endregion

//@구조체
#pragma region Structs
/**
 * @FAIAbilityBlockUnit
 *
 * 블록 내의 각 어빌리티 유닛에 대한 정보를 정의합니다.
 */
USTRUCT(BlueprintType)
struct FAIAbilityBlockUnit
{
    GENERATED_BODY()

public:
    FAIAbilityBlockUnit()
        : AbilityTag(FGameplayTag())
        , Priority(0)
    {}

    FAIAbilityBlockUnit(const FGameplayTag& InAbilityTag, int32 InPriority)
        : AbilityTag(InAbilityTag)
        , Priority(InPriority)
    {}

public:
    //@어빌리티 식별 태그
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티")
        FGameplayTag AbilityTag;

    //@어빌리티 실행 우선순위 (낮을수록 높은 우선순위)
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티")
        int32 Priority;

public:
    /**
     * 태그 일치 여부를 확인하는 연산자 오버로딩
     * @param InTag - 비교할 GameplayTag
     * @return 태그가 정확히 일치하면 true, 그렇지 않으면 false
     */
    bool operator==(const FGameplayTag& InTag) const
    {
        return AbilityTag.MatchesTagExact(InTag);
    }

public:
    FORCEINLINE FGameplayTag GetAbilityTag() const { return AbilityTag; }
    FORCEINLINE int32 GetPriority() const { return Priority; }
};

/**
 * @FAIAbilityBlock
 *
 * AI가 수행할 하나의 동작을 구성하는 어빌리티들의 집합을 정의합니다.
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

    FAIAbilityBlock(const FGameplayTag& InBlockIdentifier, int32 InExecutionPriority)
        : BlockIdentifier(InBlockIdentifier)
        , ExecutionPriority(InExecutionPriority)
        , AbilityBlockUnits()
    {
    }

public:
    //@어빌리티 블록의 고유 식별자
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티 블록 | 기본")
        FGameplayTag BlockIdentifier;

    //@어빌리티 블록의 실행 우선순위 (낮을수록 높은 우선순위)
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티 블록 | 기본")
        int32 ExecutionPriority;

    //@블록 내 어빌리티들의 정보
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티 블록 | 어빌리티")
        TArray<FAIAbilityBlockUnit> AbilityBlockUnits;

    // ... PostSerialize와 PostEditChangeProperty는 동일 ...

public:
    FORCEINLINE FGameplayTag GetBlockIdentifier() const { return BlockIdentifier; }
    FORCEINLINE int32 GetExecutionPriority() const { return ExecutionPriority; }
    FORCEINLINE const TArray<FAIAbilityBlockUnit>& GetAbilityBlockUnits() const { return AbilityBlockUnits; }

    /**
     * 지정된 태그와 일치하는 어빌리티 유닛을 찾습니다.
     * @param InTag - 찾고자 하는 어빌리티의 태그
     * @return 찾은 어빌리티 유닛에 대한 포인터, 없으면 nullptr
     */
    const FAIAbilityBlockUnit* FindAbilityBlockUnitByTag(const FGameplayTag& InTag) const
    {
        const FAIAbilityBlockUnit* FoundUnit = AbilityBlockUnits.FindByPredicate([InTag](const FAIAbilityBlockUnit& Unit)
            {
                return Unit == InTag;
            });
        return FoundUnit;
    }

    //@어빌리티 유닛의 정렬 작업
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
*
* AI의 전투 시퀀스를 정의합니다.
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
    {}

    //@전투 시작 블록
    UPROPERTY(EditDefaultsOnly, Category = "전투 시퀀스")
        FAIAbilityBlock StartBlock;

    //@주요 전투 패턴 블록 목록
    UPROPERTY(EditDefaultsOnly, Category = "전투 시퀀스")
        TArray<FAIAbilityBlock> AbilityBlocks;

    //@전투 종료 블록
    UPROPERTY(EditDefaultsOnly, Category = "전투 시퀀스")
        FAIAbilityBlock ExitBlock;

public:
    //@시작 블록 반환
    FORCEINLINE const FAIAbilityBlock& GetStartBlock() const { return StartBlock; }

    //@전투 패턴 블록 목록 반환
    FORCEINLINE const TArray<FAIAbilityBlock>& GetAbilityBlocks() const { return AbilityBlocks; }

    //@종료 블록 반환
    FORCEINLINE const FAIAbilityBlock& GetExitBlock() const { return ExitBlock; }
};

/**
 *  @FAIPersonalityTraits
 * 
 *  AI의 성향 정보를 담고 있는 구조체
 */
USTRUCT(BlueprintType)
struct FAIPersonalityTraits
{
    GENERATED_BODY()

public:
    //@공격성: 공격 유닛의 연속 사용 횟수와 공격적인 행동에 영향 + 높을수록 더 많은 공격 유닛 사용 및 더 공격적인 행동
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Personality", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
        float Aggressiveness = 50.0f;

    //@기민함: 회피 및 기동성에 영향 + 높을수록 더 자주 회피하고 적극적으로 움직임
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Personality", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
        float Agility = 50.0f;

    //@적극성: 선제적인 방어 및 준비 행동에 영향 + 높을수록 가드 장착 등의 준비 행동을 생략함
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Personality", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
        float Proactiveness = 50.0f;

    //@계획성: 상황에 따른 전략 변경에 영향 + 높을수록 피격 시 전략 변경 가능성 증가
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Personality", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
        float Methodicalness = 50.0f;

    //@성향에 따른 공격 유닛 사용 개수 계산 - 공격성 수치 영향
    int32 CalculateAttackUnitsCount(int32 BaseCount) const
    {
        // 공격성이 0일 때는 기본값, 100일 때는 기본값의 2배까지 증가
        float Multiplier = 1.0f + (Aggressiveness / 100.0f);
        return FMath::Max(1, FMath::RoundToInt(BaseCount * Multiplier));
    }

    //@성향에 따른 회피 확률 계산 - 기민함 수치 영항
    float CalculateDodgeProbability() const
    {
        // 기민함이 0일 때는 10%, 100일 때는 80%
        return 0.1f + (Agility / 100.0f * 0.7f);
    }

    //@성향에 따른 선제적 가드 장착 여부 결정 - 적극성 수치 영향
    bool ShouldUseProactiveGuard() const
    {
        // 적극성 수치에 따른 가드 장착 확률
        float GuardProbability = Proactiveness / 100.0f;
        return FMath::FRand() < GuardProbability;
    }

    //@성향에 따른 전략 변경 여부 결정 - 계획성 수치 영향
    bool ShouldChangeStrategyOnCancel() const
    {
        // 계획성이 높을수록 전략 변경 확률 증가
        float ChangeStrategyProbability = Methodicalness / 100.0f;
        return FMath::FRand() < ChangeStrategyProbability;
    }
};


/**
* @FAIDataSet
*
* AI의 기본 정보와 행동 패턴을 정의합니다.
*/
USTRUCT(BlueprintType)
struct FAIDataSet
{
    GENERATED_BODY()

public:
    //@AI 유형
    UPROPERTY(EditDefaultsOnly, Category = "AI 정보 | AI 유형")
        EAIType AIType;

    //@비헤이비어 트리
    UPROPERTY(EditDefaultsOnly, Category = "AI 정보 | Behavior Tree")
        TObjectPtr<UBehaviorTree> BehaviorTree;

    //@전투 시퀀스
    UPROPERTY(EditDefaultsOnly, Category = "AI 정보 | 전투 패턴")
        FAICombatSequence CombatSequence;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAIDataSet
 * 
 *	AI 데이터 정보를 담은 Data Asset
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
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	UPROPERTY(EditDefaultsOnly, Category = "AI 정보 목록")
		TArray<FAIDataSet> AIDataSets;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
