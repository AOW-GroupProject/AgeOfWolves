// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"

#include "Area.h"
#include "17_GameMode/AgeOfWolvesGameMode.h"
#include "AreaQuestDataInfos.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAreaQuestDataInfos, Log, All)

//@전방 선언
#pragma region Forward Declaration
class AItem;
class UAreaManagerSubsystem;
#pragma endregion
/**
 * 
 */

//@열거형
#pragma region Enums
//@열거형
/*
 *  @EQuestType
 *
 *  퀘스트 타입에 대한 열거형입니다.
 */
UENUM(BlueprintType)
enum class EQuestType : uint8
{
	None,
	EliminateEnemies   UMETA(DisplayName = "제거형 퀘스트"),
	CollectItems      UMETA(DisplayName = "수집형 퀘스트"),
	ReachLocation  UMETA(DisplayName = "지정 위치 도달형 퀘스트"),
	InteractObject    UMETA(DisplayName = "오브젝트 상호작용형 퀘스트"),
	MAX,        
};

//@열거형
/*
 *  @EQuestStatus
 *
 *  런타임에 동적으로 변경될 퀘스트 생명주기에 대한 열거형입니다.
 */
UENUM(BlueprintType)
enum class EQuestStatus : uint8
{
	None,
	NotStarted   UMETA(DisplayName = "시작되지 않음"),
	InProgress      UMETA(DisplayName = "진행 중"),
	Completed  UMETA(DisplayName = "완료"),
	Failed    UMETA(DisplayName = "실패"),
	MAX,        
};

//@열거형
/*
 *  @EQuestRetryPolicy
 *
 *  퀘스트 시행횟수와 관련된 열거형입니다.
 */
UENUM(BlueprintType)
enum class EQuestRetryPolicy : uint8
{
	OneTime   UMETA(DisplayName = "한번만 시도 가능"),
	Repeatable      UMETA(DisplayName = "반복 시도 가능"),
	Unrepeatable  UMETA(DisplayName = "재시도가 불가능, 진행도는 유지 됨"),
	MAX,        
};

/**
 * @EAreaQuestDataValidationError
 *
 * AreaQuestData 검증 과정에서 발생할 수 있는 오류 유형을 정의합니다.
 * 각 오류 타입은 Quest Data 변수 설정의 문제점을 나타냅니다.
 */
UENUM(BlueprintType)
enum class EAreaQuestDataValidationError : uint8
{
	None                            UMETA(DisplayName = "오류 없음"),

	// 중복 관련 오류
	DuplicateAreaTag                UMETA(DisplayName = "중복된 지역 태그"),
	DuplicateQuestTag               UMETA(DisplayName = "중복된 퀘스트 태그"),
	DuplicateRewardItem             UMETA(DisplayName = "중복된 아이템 보상"),
	
	// 글자 제한 관련 오류
	StringLengthExceeded            UMETA(DisplayName = "글자 수 제한 초과"),

	// 영문 제한 관련 오류
	StringNotEngOnly                UMETA(DisplayName = "영문 외 문자 사용"),

	// 개행 포함 여부 관련 오류
	StringUseNewLine                UMETA(DisplayName = "개행 사용"),
	
	// 개수 제한 관련 오류
	RewardItemQuantityExceeded      UMETA(DisplayName = "보상 아이템 개수 초과"),

	// 유효하지 않은 QuestType
	InvalidQuestType                UMETA(DisplayName = "유효하지 않은 퀘스트 타입 : MAX 값 사용"),

	// bHasReward가 True인데 FQuestRewardItem가 없음
	MissingRewardItems              UMETA(DisplayName = "보상 아이템 누락")
};
#pragma endregion

//@구조체
#pragma region Structs


/**
 * @FAreaQuestDataValidationResult
 *
 * AreaQuestData 검증 결과를 담는 구조체입니다.
 * 어떤 오류가 어디서 발생했는지 상세한 정보를 제공합니다.
 */
USTRUCT(BlueprintType)
struct FAreaQuestDataValidationResult
{
	GENERATED_BODY()

public:
	// 검증 결과 (성공/실패)
	UPROPERTY(BlueprintReadOnly)
	bool bIsValid = true;

	//@오류 유형
	UPROPERTY(BlueprintReadOnly)
	EAreaQuestDataValidationError ErrorType = EAreaQuestDataValidationError::None;
	
	//@오류 메시지
	UPROPERTY(BlueprintReadOnly)
	FString ErrorMessage;

	// 문제가 발생한 Area 인덱스
	UPROPERTY(BlueprintReadOnly)
	int32 ProblemAreaIndex = -1;

	// 문제가 발생한 Quest 인덱스
	UPROPERTY(BlueprintReadOnly)
	int32 ProblemQuestIndex = -1;
	
	// 기본 생성자 (성공 케이스)
	FAreaQuestDataValidationResult()
	{
	}

	// 오류 케이스 생성자
	FAreaQuestDataValidationResult(EAreaQuestDataValidationError InErrorType, const FString& InErrorMessage,
		int32 InProblemAreaIndex, int32 InProblemQuestIndex)
		: bIsValid(false)
		, ErrorType(InErrorType)
		, ErrorMessage(InErrorMessage)
		, ProblemAreaIndex(InProblemAreaIndex)
		, ProblemQuestIndex(InProblemQuestIndex)
	{
	}
};

/*
*   @FQuestRewardItem
*
*   퀘스트 보상 아이템에 대한 정보를 담은 구조체입니다.
*/
USTRUCT(BlueprintType)
struct FQuestRewardItem
{
	GENERATED_BODY()

	//@ 보상 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItem> ItemClass = nullptr;

	//@ 아이템 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "50"))
	int32 Quantity = 1;

	//@ 드랍 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DropChance = 1.f;
};

/*
*   @FQuestDataInfo
*
*   퀘스트 정보와 진행 상황을 담은 구조체입니다.
*/

// FQuestDataInfo 하위 타입이 구현해야할 인터페이스  
#define QUEST_DATA_INTERFACE() \
bool ProcessEvent(const FGameplayTag& EventTag, AActor* Actor, AArea*, const FVector& Location); \
bool CheckCompletion(AArea* SourceArea) const; \
float GetProgressRatio(AArea* SourceArea) const; \
void ResetProgress(); \
static_assert(true, "")

// 인터페이스 구현 검증 매크로
#define IMPLEMENT_QUEST_INTERFACE(ClassName) \
static_assert(std::is_same_v<decltype(&ClassName::ProcessEvent), bool(ClassName::*)(const FGameplayTag&, AActor*, AArea*, const FVector&)>, \
#ClassName " must implement ProcessEvent(const FGameplayTag&, AActor*, const FVector&)"); \
static_assert(std::is_same_v<decltype(&ClassName::CheckCompletion), bool(ClassName::*)(AArea* SourceArea) const>, \
#ClassName " must implement CheckCompletion() const"); \
static_assert(std::is_same_v<decltype(&ClassName::GetProgressRatio), float(ClassName::*)(AArea* SourceArea) const>, \
#ClassName " must implement GetProgressRatio() const"); \
static_assert(std::is_same_v<decltype(&ClassName::ResetProgress), void(ClassName::*)()>, \
#ClassName " must implement ResetProgress()"); \
static_assert(true, "")


USTRUCT(BlueprintType)
struct FQuestDataInfo
{
	GENERATED_BODY()

	virtual ~FQuestDataInfo() = default;
	
	//@퀘스트 고유 Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault", meta = (Categories = "Quest"))
	FGameplayTag QuestTag = FGameplayTag::EmptyTag; 

	//@퀘스트 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault")
	FString QuestName;

	//@퀘스트 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault")
	EQuestType QuestType = EQuestType::EliminateEnemies;

	//@퀘스트 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestDefault")
	EQuestStatus QuestStatus = EQuestStatus::NotStarted;

	//@퀘스트 시행횟수와 관련된 정책
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault")
	EQuestRetryPolicy QuestPolicy =	EQuestRetryPolicy::OneTime;

	//@퀘스트 보상 플래그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault")
	bool bHasReward = false;
	
	//@퀘스트 보상 아이템 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault", meta = (EditCondition = "bHasReward"))
	TArray<FQuestRewardItem> RewardItems;
	
	//@퀘스트 제한 시간 플래그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault")
	bool bHasTimeLimit = false;

	//@퀘스트 제한 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestDefault", meta = (EditCondition = "bHasTimeLimit"))
	float TimeLimit = 300.f;

	//@퀘스트 경과 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestDefault")
	float ElapsedTime = 0.f;

public:
	//@ 퀘스트 진행처리
	virtual bool ProcessEvent(const FGameplayTag& EventTag, AActor* Actor, AArea* SourceArea, const FVector& Location) { return false;};
	//@ 퀘스트 완료 확인
	virtual bool CheckCompletion(AArea* SourceArea) const { return false; }
	//@ 진행률 가져오기
	virtual float GetProgressRatio(AArea* SourceArea) const { return 0.f; }
	//@ 진행도 초기화
	virtual void ResetProgress() { ElapsedTime = 0.f;}

	//@ 퀘스트 성공
	void QuestSuccess()
	{
		QuestStatus = EQuestStatus::Completed;
	}
	
	//@ 퀘스트 실패 
	void QuestFail()
	{
		switch (QuestPolicy)
		{
		case EQuestRetryPolicy::OneTime:
			QuestStatus = EQuestStatus::Failed;
			break;
		case EQuestRetryPolicy::Repeatable:
			QuestStatus = EQuestStatus::NotStarted;
			ResetProgress();
			break;
		case EQuestRetryPolicy::Unrepeatable:
			QuestStatus = EQuestStatus::Failed;
			break;
		default:
			break;
		}
	}
};

USTRUCT(BlueprintType)
struct FEliminationQuestDataInfo : public FQuestDataInfo
{
    GENERATED_BODY()
	
    //@ EAIHierarchyType 계급 이상의 적들을 처치해야함 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|EliminateEnemies")
    EAIHierarchyType MinimumHierarchy = EAIHierarchyType::Regular;

	//@ 모든 적 처치 플래그
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|EliminateEnemies")
    bool bEliminateAllEnemies = true;

	//@ 필요한 킬 카운트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|EliminateEnemies", meta = (EditCondition = "!bEliminateAllEnemies"))
    int32 RequiredKillCount = 1;

	//@ 동적으로 관리되는 킬 카운트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|EliminateEnemies")
    int32 CurrentKillCount = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|EliminateEnemies")
    FGameplayTag SpecificFlag;
	
public:
    //@ 인터페이스 구현
    bool ProcessEvent(const FGameplayTag& EventTag, AActor* Actor, AArea* SourceArea, const FVector& Location)
    {	
    	//@ 완료한 퀘스트나 실패한 퀘스트는 진행하지 않음
    	if (QuestStatus == EQuestStatus::Failed || QuestStatus == EQuestStatus::Completed)
    	{
    		return false;
    	}

    	//@ 퀘스트 시작
    	if (QuestStatus == EQuestStatus::NotStarted)
    	{
    		QuestStatus = EQuestStatus::InProgress;	
    	}
    	
    	//@ 처치한 AI의 계급이 조건에 맞을 경우  
    	if (IsValidTarget(Actor, SourceArea))
    	{
    		CurrentKillCount++;
    	}

    	return true;
    }
    
    bool CheckCompletion(AArea* SourceArea) const 
    {
    	//@ 시간 초과
    	if (bHasTimeLimit && ElapsedTime >= TimeLimit)
        {
            return false; 
        }

    	//@ bEliminateAllEnemies 활성화 시 모든 맵에 적이 제거되었는지 확인 
    	if (bEliminateAllEnemies)
    	{
    		return CheckAllEnemiesEliminated(SourceArea);
    	}
    	
    	//@ 킬카운트 달성
    	if (CurrentKillCount >= RequiredKillCount)
    	{
    		return true;
    	}

    	return false;
    }
    
    float GetProgressRatio(AArea* SourceArea) const
    {
        if (bEliminateAllEnemies)
        {
            int32 TotalEnemies = GetTotalEnemiesInArea(SourceArea);
            return TotalEnemies > 0 ? (float)CurrentKillCount / TotalEnemies : 0.0f;
        }
        return RequiredKillCount > 0 ? (float)CurrentKillCount / RequiredKillCount : 0.0f;
    }
    
    void ResetProgress()
    {
        FQuestDataInfo::ResetProgress(); // 부모 클래스 호출
        CurrentKillCount = 0;
    }

public:
    bool IsValidTarget(AActor* Actor, AArea* SourceArea) const
    {
        if (!Actor) return false;

    	EAIHierarchyType TargetHierarchy = SourceArea->GetAIHierarchyType(Actor);
    	const bool bFindTargetHierarchy = TargetHierarchy <= MinimumHierarchy;
        return bFindTargetHierarchy;
    }
    
	bool CheckAllEnemiesEliminated(AArea* SourceArea) const
    {
    	TArray<FAreaAIInfo> AllAreaAI = SourceArea->GetAreaAIInfos();
    
    	for (const FAreaAIInfo& AIInfo : AllAreaAI)
    	{
    		if (!AIInfo.AIActor.IsValid()) continue;
    		
    		if (!AIInfo.CurrentState.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")))
    		{
    			return false;
    		}
    	}
    	
    	return true;
    }
    
    int32 GetTotalEnemiesInArea(AArea* SourceArea) const
    {
    	int32 AliveAI = 0;
    	TArray<FAreaAIInfo> AllAreaAI = SourceArea->GetAreaAIInfos();
    	
    	for (const FAreaAIInfo& AIInfo : AllAreaAI)
    	{
    		if (AIInfo.AIActor.IsValid() && !AIInfo.CurrentState.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")))
    		{
    			AliveAI++;
    		}
    	}
    	
    	return AliveAI;
    }
};

USTRUCT(BlueprintType)
struct FAreaQuestDataSet
{
    GENERATED_BODY()
	//@ Area 고유 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Area"))
	FGameplayTag AreaTag = FGameplayTag::EmptyTag;

	// 해당 Area의 제거형 퀘스트 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEliminationQuestDataInfo> EliminationQuests;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAreaQuestDataInfos
 *
 *	각 Area별 퀘스트 정보를 담은 Data Asset
 *  
 */
UCLASS()
class AGEOFWOLVES_API UAreaQuestDataInfos : public UPrimaryDataAsset
{
	//@친추 클래스
#pragma region Friend Class
#pragma endregion
	GENERATED_BODY()

	//@Defualt Setting
#pragma region Default Setting
public:
	UAreaQuestDataInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	//@ 에디터에서 프로퍼티가 변경될 때 호출되는 함수
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	//@ 배열이나 구조체 내부의 프로퍼티가 변경될 때 호출되는 함수
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
	//@ Area 고유 태그
	UPROPERTY(EditDefaultsOnly)
	TArray<FAreaQuestDataSet> AreaQuestSets;
	
#pragma endregion

#if WITH_EDITOR
protected:
	// === 무결성 검사 핵심 함수들 ===
	// 각 함수는 Quest 시스템의 특정 영역에 대한 검증을 담당합니다
	
	// 전체 무결성 검사를 수행하는 마스터 함수
	FAreaQuestDataValidationResult ValidateAreaQuestData() const;

	// PostEditChangeProperty에 의한 무결성 검사를 수행하는 마스터 함수
	FAreaQuestDataValidationResult ValidateAreaQuestDataAuto() const;

	// === 개별 검증 함수들 ===
	// Area 고유태그에 대한 중복을 검사하는 함수
	FAreaQuestDataValidationResult ValidateAreaTagUniqueness() const;
	
	// 퀘스트 고유태그에 대한 중복을 검사하는 함수
	FAreaQuestDataValidationResult ValidateQuestTagUniqueness() const;

	// 퀘스트 이름의 글자수를 체크하는 함수
	FAreaQuestDataValidationResult ValidateQuestNameLength() const;

	// 퀘스트 이름의 영문을 체크하는 함수
	FAreaQuestDataValidationResult ValidateQuestNameEngOnly() const;

	// 퀘스트 이름의 개행을 체크하는 함수
	FAreaQuestDataValidationResult ValidateQuestNameNewLine() const;

	// 퀘스트 타입이 MAX인지 확인하는 함수, MAX는 사용 불가능
	FAreaQuestDataValidationResult ValidateInvalidQuestType() const;

	// 보상 설정 여부에 따른 보상 아이템 누락 검사 함수
	FAreaQuestDataValidationResult ValidateRewardItemsIfHasReward() const;

	// 보상 아이템에 대한 중복을 검사하는 함수
	FAreaQuestDataValidationResult ValidateRewardItemUniqueness() const;

	// 보상 아이템의 개수 초과를 검사하는 함수
	FAreaQuestDataValidationResult ValidateRewardItemQuantityExceeded() const;
	
	//@검증 실패 시 에러 출력하는 함수
	void ShowValidationError(const FAreaQuestDataValidationResult& ValidationResult) const;

	//@검증 실패 시 자동 수정 진행하는 함수
	bool TryAutoFixValidationError(const FAreaQuestDataValidationResult& ValidationResult);

public:
	// === 에디터에서 수동으로 실행할 수 있는 검증 함수들 ===

	// 전체 무결성 검사를 수동으로 실행
	UFUNCTION(CallInEditor, Category = "AreaQuest DataSet Validation")
	void ValidateAreaQuestDataManually();
#endif
	
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
