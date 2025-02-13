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
        : AbilityClass(nullptr)
        , AbilityTag(FGameplayTag())
        , Priority(0)
    {}

    FAIAbilityBlockUnit(TSubclassOf<UGameplayAbility> InAbilityClass, const FGameplayTag& InAbilityTag, int32 InPriority)
        : AbilityClass(InAbilityClass)
        , AbilityTag(InAbilityTag)
        , Priority(InPriority)
    {}

public:
    //@어빌리티 클래스
    UPROPERTY(EditDefaultsOnly, Category = "어빌리티")
        TSubclassOf<UGameplayAbility> AbilityClass;

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
    FORCEINLINE TSubclassOf<UGameplayAbility> GetAbilityClass() const { return AbilityClass; }
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

public:
    // 직렬화 후 호출되는 함수
    void PostSerialize(const FArchive& Ar)
    {
        if (AbilityBlockUnits.Num() >= 2)
        {
            SortAbilityBlockUnitsByPriority();
        }
    }

    // 에디터에서 프로퍼티가 변경될 때 호출되는 함수
    void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
    {
        if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FAIAbilityBlock, AbilityBlockUnits))
        {
            if (AbilityBlockUnits.Num() >= 2)
            {
                SortAbilityBlockUnitsByPriority();
            }
        }
    }

public:
    // Getter 함수들
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

    //@AI의 전투 행동을 구성하는 어빌리티 블록들
    UPROPERTY(EditDefaultsOnly, Category = "AI 정보 | 전투 패턴")
        TArray<FAIAbilityBlock> AbilityBlocks;

    //@가드/일반 전투 상태 전환을 위한 종료 블록
    UPROPERTY(EditDefaultsOnly, Category = "AI 정보 | 전투 패턴")
        FAIAbilityBlock ExitBlock;
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
