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
