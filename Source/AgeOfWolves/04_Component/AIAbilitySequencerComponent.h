#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "10_AI/AIDataSetInfos.h"
#include "GameplayEffectTypes.h"

#include "AIAbilitySequencerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAICombatPattern, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySystemComponent;
class UGameplayAbility;
struct FAIDataSet;
struct FAICombatSequence;
#pragma endregion

//@열거형
#pragma region Enums
/**
 * @ESequencerExecutionState
 * 시퀀서의 현재 실행 상태를 나타내는 열거형
 */
UENUM(BlueprintType)
enum class ESequencerExecutionState : uint8
{
    Idle            UMETA(DisplayName = "대기"),
    StartBlock      UMETA(DisplayName = "시작 블록"),
    AbilityBlocks   UMETA(DisplayName = "어빌리티 블록"),
    ExitBlock       UMETA(DisplayName = "종료 블록"),
    WaitingExit     UMETA(DisplayName = "종료 대기")
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 * @FSequencerStatistics
 * 시퀀서 실행 통계 정보
 */
USTRUCT(BlueprintType)
struct FSequencerStatistics
{
    GENERATED_BODY()

    // === 기존 어빌리티 실행 통계 ===
    UPROPERTY(BlueprintReadOnly)
    int32 TotalAbilitiesExecuted = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 SuccessfulAbilities = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 CancelledAbilities = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 FailedAbilities = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalCombatCycles = 0;

    UPROPERTY(BlueprintReadOnly)
    float CurrentCombatStartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float TotalCombatTime = 0.0f;

    // === 새로운 데미지 추적 통계 ===
    UPROPERTY(BlueprintReadOnly)
    float TotalDamageReceived = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float TotalDamageDealt = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float CurrentTurnDamageReceived = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float CurrentTurnDamageDealt = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float TurnStartHealth = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float TurnEndHealth = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalTurnsCompleted = 0;

    // FSequencerStatistics 헤더에서는 선언만
public:
    // === 기존 함수들 ===
    float GetSuccessRate() const;
    float GetCancelRate() const;
    float GetAverageCombatTime() const;

    // === 데미지 관련 함수들 ===
    float GetAverageDamageReceivedPerTurn() const;
    float GetAverageDamageDealtPerTurn() const;
    float GetDamageExchangeRatio() const;
    void ResetCurrentTurnData(float CurrentHealth);
    void CompleteTurn(float FinalHealth);

    // === 데미지 행동 결과 분석 함수들 ===
    float CalculateCurrentTurnDamageEfficiency() const;
    float CalculateCurrentTurnSurvivalPerformance() const;
    float CalculateCurrentTurnAttackPerformance() const;
    float CalculateCurrentTurnDamageScore() const;
    float GetOverallDamagePerformance() const;
    FString GetDamageResultSummary() const;
    void GetNormalizedDamageDataForReward(float& OutDamageReceived, float& OutDamageDealt, float& OutDamageScore) const;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@어빌리티 활성화 요청 이벤트
DECLARE_DELEGATE_RetVal_OneParam(bool, FRequestActivateAbilityBlockUnit, const FGameplayTag&);

//@전투 패턴 Exit Block 완료 통지
DECLARE_DELEGATE_RetVal(bool, FNotifyCombatPatternExitComplete);

//@어빌리티 실행 통계 업데이트 통지 (학습 시스템용)
DECLARE_DELEGATE_FourParams(FNotifyAbilityExecutionStats, const FGameplayTag&, bool, bool, float);
#pragma endregion

/**
 * @UAIAbilitySequencerComponent
 *
 * AI의 전투 패턴과 어빌리티 블록을 관리하는 컴포넌트입니다.
 * 적응형 학습 시스템과 연동하여 실행 통계를 제공합니다.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UAIAbilitySequencerComponent : public UActorComponent
{
    GENERATED_BODY()

//@친추 클래스
#pragma region Friend Class
#pragma endregion

//@Default Setting
#pragma region Default Setting
public:
    UAIAbilitySequencerComponent(const FObjectInitializer& ObjectInitializer);

protected:
    //~UActorComponent Interface
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void InitializeComponent() override;
    virtual void DestroyComponent(bool bPromoteChildren = false) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of UActorComponent Interface

private:
    //@외부 바인딩
    void ExternalBindToASC(UBaseAbilitySystemComponent* ASC);
    void ExternalBindToAIController();

public:
    //@초기화
    UFUNCTION()
    void InitializeCombatPatternComponent();

    //@시퀀서 상태 초기화
    UFUNCTION(BlueprintCallable, Category = "AI Combat Pattern")
    void ResetSequencerState();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    //@학습 시스템 초기화
    void InitializeLearningSystem();

protected:
    //@활성화 요청에 대한 처리
    bool ExecuteAbilityActivationRequest(const FAIAbilityBlockUnit* CurrentUnit, const FString& BlockType);

    //============== 블록 선택 ==============
private:
    //@가중치 합 & 기본 성향 기반의 최종 선호도 계산 및 블록 선택
    float CalculateCurrentActionPreference(const FGameplayTag& ActionCategory) const;

    //@Bounded Control 및 ε-greedy 구현
    FGameplayTag SelectNextAbilityBlockWithEpsilonGreedy();
    //@Bounded Control 기반의 최종 선호도 계산
    FGameplayTag SelectBlockByPreference() const;
    //@ε-greedy 기반의 랜덤 블록 선택
    FGameplayTag SelectRandomBlock() const;

    //============== 블록 유닛 선택 ==============
private:
    //@현재 블록 내 실행할 블록 유닛 갯수
    int32 CalculateExecutionCountForSelectedBlock(const FGameplayTag& SelectedBlockTag, bool bIsExplorationMode) const;
    //@현재 블록 내 블록 유닛 실행 플랜 설정
    void SetCurrentBlockExecutionPlan(const FGameplayTag& BlockTag, int32 ExecutionCount);
    //@이번 턴 실행할 블록 유닛이 남아있는지 체크
    bool HasRemainingUnitsToExecute() const;

protected:
    //@인덱스 관리
    void AdvanceToNextUnit();
    void AdvanceToNextBlock();

protected:
    //@Exit Block 관련 함수들
    bool StartExitBlock();

    UFUNCTION()
    void ExecuteExitBlockUnits();

    //==== 경험 데이터 수집 ====//

private:
    void UpdateExperienceWeights(const FGameplayTag& CompletedBlockTag);


    //===== 데미지 전달/받음 행동 데이터 수집 =====//

private:
    //@데미지 행동 결과 분석 및 보상 계산용 데이터 제공
    void AnalyzeDamagePerformance();

    //@턴 시작 시 Health 추적 시작
    void StartDamageTracking();

    //@턴 종료 시 Health 추적 종료 및 데이터 집계
    void EndDamageTracking();

    //===== 최적 거리 데이터 수집 =====//
private:
    float CalcualteOptimalCombatDistance() const;


    //===== 어빌리티 성공 여부 데이터 수집 =====//
private:
    bool CalculateCurrentTurnAbilitySuccess() const;

protected:
    //@캐싱된 전투 시퀀스
    UPROPERTY()
    FAICombatSequence CachedCombatSequence;

    //@런타임 학습 상태 (학습 시스템 활성화 시에만 사용)
    UPROPERTY()
    FAIRuntimeLearningState RuntimeLearningState;

    //@현재 AI의 적응형 행동 설정 (캐시됨)
    UPROPERTY()
    FAIAdaptiveBehaviorConfig CachedAdaptiveBehaviorConfig;

protected:
    //@현재 활성화 된 블록 유닛의 어빌리티 태그
    UPROPERTY()
    FGameplayTag CurrentActivatingUnitTag;

    //@현재 실행 중인 블록/유닛 인덱스
    //@brief -1은 Start Block, 0~N 은 Ability Blocks 내부의 블록 인덱스, -2는 Exit Block
    UPROPERTY()
    int32 CurrentBlockIndex;

    UPROPERTY()
    int32 CurrentUnitIndex;

    //@전투 준비 상태
    UPROPERTY()
    bool bCombatReady;

    //@최초 실행 여부
    UPROPERTY()
    bool bIsFirstRun;

    //@현재 유닛 완료 후 Exit Block 시작 대기 상태
    UPROPERTY()
    bool bWaitingForExitBlock;

    //@실행 통계
    UPROPERTY()
    FSequencerStatistics ExecutionStats;

    //@현재 어빌리티 시작 시간 (성능 측정용)
    UPROPERTY()
    float CurrentAbilityStartTime;

    //@디버깅용 상세 로그 활성화
    UPROPERTY(EditAnywhere, Category = "Debug", meta = (DisplayName = "상세 로그 활성화"))
    bool bEnableVerboseLogging;

    //@학습 시스템 연동 활성화
    UPROPERTY(EditAnywhere, Category = "AI Learning", meta = (DisplayName = "학습 시스템 연동"))
    bool bEnableLearningSystemIntegration;

private:
    //@현재 블록 실행 계획 (학습 모드에서만 사용)
    UPROPERTY()
    FGameplayTag CurrentSelectedBlockTag;

    UPROPERTY()
    int32 PlannedExecutionCount;

    UPROPERTY()
    int32 CompletedExecutionCount;

    UPROPERTY()
    bool bIsCurrentlyInExplorationMode;

private:
    //@Health 추적용 델리게이트 핸들
    FDelegateHandle HealthChangeHandle;

    //@현재 타겟 액터 (데미지 소스 추적용)
    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentTargetActor;

    //@현재 턴의 Health 추적 데이터
    UPROPERTY()
    float PreviousHealthValue;

    UPROPERTY()
    bool bIsTrackingDamage;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@어빌리티 활성화 요청 이벤트
    FRequestActivateAbilityBlockUnit RequestActivateAbilityBlockUnit;

    //@전투 패턴 Exit Block 완료 통지 이벤트
    FNotifyCombatPatternExitComplete NotifyCombatPatternExitComplete;

    //@어빌리티 실행 통계 업데이트 통지 (학습 시스템용)
    FNotifyAbilityExecutionStats NotifyAbilityExecutionStats;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@전투 패턴 제어 콜백
    UFUNCTION()
    bool OnRequestActivateAICombatLoop();

    UFUNCTION()
    bool OnRequestEndCombatPattern();

protected:
    //@ASC 이벤트 콜백
    void OnAbilityActivated(UGameplayAbility* Ability);
    void OnAbilityEnded(UGameplayAbility* Ability);
    void OnAbilityCancelled(UGameplayAbility* Ability);

protected:
    //@Health 속성 변화 콜백
    void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

protected:
    //@AI가 데미지를 전달했을 때 호출되는 콜백
    UFUNCTION()
        void OnDamageDealtByAI(AActor* SourceActor, AActor* TargetActor, const FGameplayEventData& EventData);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@상태 검증 함수들
    bool ValidateCurrentSequencerState() const;
    bool ValidateAbilityTag(const UGameplayAbility* Ability) const;
    bool ValidateCombatSequenceIntegrity() const;

    //@전투 루프 실행 헬퍼 함수들
    const FAIAbilityBlockUnit* GetCurrentExecutingUnit(FString& OutBlockType, int32& OutTotalUnits) const;
    bool ValidateExecutionRequest() const;

    //@통계 및 학습 시스템 연동
    void UpdateExecutionStatistics(const FGameplayTag& AbilityTag, bool bSuccess, bool bCancelled, float ExecutionTime);
    //@부분 완료 통계 업데이트 (유닛 부족으로 인한 조기 종료 시)
    void UpdatePartialCompletionStatistics(const FGameplayTag& BlockTag, int32 CompletedCount, int32 PlannedCount);

    void NotifyLearningSystem(const FGameplayTag& AbilityTag, bool bSuccess, bool bCancelled, float ExecutionTime);

    //@디버깅 및 정보 출력
    void LogCurrentSequencerState() const;
    void LogBlockTransition(const FString& FromBlock, const FString& ToBlock) const;
    void LogAbilityExecution(const FGameplayTag& AbilityTag, const FString& Action) const;

private:
    //@상태 확인 인라인 함수들
    FORCEINLINE bool IsExecutingStartBlock() const
    {
        return bIsFirstRun && CurrentBlockIndex == -1;
    }

    FORCEINLINE bool IsExecutingAbilityBlocks() const
    {
        return CurrentBlockIndex >= 0;
    }

    FORCEINLINE bool IsExecutingExitBlock() const
    {
        return CurrentBlockIndex == -2;
    }

    FORCEINLINE void SetExecutingExitBlock()
    {
        CurrentBlockIndex = -2;
    }

    //@실행 상태 반환
    FORCEINLINE ESequencerExecutionState GetCurrentExecutionState() const
    {
        if (bWaitingForExitBlock) return ESequencerExecutionState::WaitingExit;
        if (IsExecutingStartBlock()) return ESequencerExecutionState::StartBlock;
        if (IsExecutingAbilityBlocks()) return ESequencerExecutionState::AbilityBlocks;
        if (IsExecutingExitBlock()) return ESequencerExecutionState::ExitBlock;
        return ESequencerExecutionState::Idle;
    }

private:
    //@강화 학습 수행 여부
    bool IsAdaptiveLearningEnabled() const;

private:
    //@최종 선호도 기반이 아닌, '탐험'해야 하는지 체크
    bool ShouldExplore() const;

public:
    //@상태 조회 함수들
    FORCEINLINE bool IsCombatReady() const { return bCombatReady; }
    FORCEINLINE void SetCombatReady(bool bReady) { bCombatReady = bReady; }

    //@인덱스 getter
    FORCEINLINE int32 GetCurrentBlockIndex() const { return CurrentBlockIndex; }
    FORCEINLINE int32 GetCurrentUnitIndex() const { return CurrentUnitIndex; }

    //@실행 상태 getter
    UFUNCTION(BlueprintCallable, Category = "AI Combat Pattern")
    ESequencerExecutionState GetExecutionState() const { return GetCurrentExecutionState(); }

    //@블록 정보 getter
    FORCEINLINE const FAICombatSequence& GetCachedCombatSequence() const { return CachedCombatSequence; }

    //@통계 정보 getter
    UFUNCTION(BlueprintCallable, Category = "AI Combat Pattern")
    const FSequencerStatistics& GetExecutionStatistics() const { return ExecutionStats; }

    //@현재 실행 중인 어빌리티 태그 getter
    UFUNCTION(BlueprintCallable, Category = "AI Combat Pattern")
    FGameplayTag GetCurrentExecutingAbilityTag() const { return CurrentActivatingUnitTag; }

    //@현재 블록 정보 반환
    UFUNCTION(BlueprintCallable, Category = "AI Combat Pattern")
    FString GetCurrentBlockInfo() const;

    //@전체 시퀀서 정보 출력 (디버깅용)
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Debug")
    void PrintSequencerDebugInfo() const;

    //@통계 초기화
    UFUNCTION(BlueprintCallable, Category = "AI Combat Pattern")
    void ResetExecutionStatistics();

    //@학습 시스템 연동 설정
    UFUNCTION(BlueprintCallable, Category = "AI Learning")
    void SetLearningSystemIntegration(bool bEnable) { bEnableLearningSystemIntegration = bEnable; }

    //@상세 로그 설정
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void SetVerboseLogging(bool bEnable) { bEnableVerboseLogging = bEnable; }

public:
    //@시퀀서 검증 함수들 (에디터/디버깅용)
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Validation")
    bool ValidateSequencerConfiguration() const;

    //@강제 상태 변경 (디버깅용)
    UFUNCTION(BlueprintCallable, Category = "Debug", meta = (CallInEditor = "true"))
    void ForceAdvanceToNextBlock();

    //@강제 Exit Block 시작 (디버깅용)
    UFUNCTION(BlueprintCallable, Category = "Debug", meta = (CallInEditor = "true"))
    void ForceStartExitBlock();

protected:
    //@Overdrive 방지 시스템 체크 및 적용
    void CheckAndApplyOverdrivePrevention();

    //@상세 학습 분석 로그 출력
    void LogDetailedLearningAnalysis(const FGameplayTag& BlockTag, float StandardReward, float DamagePerformance, float CombinedReward, float WeightChange) const;

public:
    //@현재 턴의 데미지 성과를 로그로 출력
    void LogCurrentTurnDamagePerformance() const;

    //@보상 계산을 위한 정규화된 데미지 데이터 반환
    UFUNCTION(BlueprintCallable, Category = "AI 적응형 시스템 분석 | 행동 데이터 | 데미지")
    void GetNormalizedDamageData(float& OutDamageReceived, float& OutDamageDealt, float& OutDamageScore) const;

    //@현재 턴의 데미지 효율성 반환
    UFUNCTION(BlueprintCallable, Category = "AI 적응형 시스템 분석 | 행동 데이터 | 데미지")
    float GetCurrentTurnDamageEfficiency() const;

private:
    //@타겟 과의 거리
    float GetCurrentDistanceToTarget() const;

#pragma endregion
};