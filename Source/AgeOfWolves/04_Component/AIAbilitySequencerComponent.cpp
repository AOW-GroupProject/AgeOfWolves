#include "AIAbilitySequencerComponent.h"
#include "Logging/StructuredLog.h"
#include "Engine/World.h"

#include "10_AI/BaseAIController.h"
#include "10_AI/AIDataSetInfos.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "14_Subsystem/AIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(LogAICombatPattern);

#pragma region Statistics
// AIAbilitySequencerComponent.cpp에 추가

float FSequencerStatistics::GetSuccessRate() const
{
    float SuccessRate = TotalAbilitiesExecuted > 0 ?
        (float)SuccessfulAbilities / (float)TotalAbilitiesExecuted : 0.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("성공률 계산: %d/%d = %.3f"),
        SuccessfulAbilities, TotalAbilitiesExecuted, SuccessRate);

    return SuccessRate;
}

float FSequencerStatistics::GetCancelRate() const
{
    float CancelRate = TotalAbilitiesExecuted > 0 ?
        (float)CancelledAbilities / (float)TotalAbilitiesExecuted : 0.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("취소율 계산: %d/%d = %.3f"),
        CancelledAbilities, TotalAbilitiesExecuted, CancelRate);

    return CancelRate;
}

float FSequencerStatistics::GetAverageCombatTime() const
{
    float AvgTime = TotalCombatCycles > 0 ?
        TotalCombatTime / (float)TotalCombatCycles : 0.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("평균 전투 시간 계산: %.2f/%d = %.3f초"),
        TotalCombatTime, TotalCombatCycles, AvgTime);

    return AvgTime;
}

float FSequencerStatistics::GetAverageDamageReceivedPerTurn() const
{
    float AvgDamageReceived = TotalTurnsCompleted > 0 ?
        TotalDamageReceived / (float)TotalTurnsCompleted : 0.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("턴당 평균 피해량 계산: %.2f/%d = %.3f"),
        TotalDamageReceived, TotalTurnsCompleted, AvgDamageReceived);

    return AvgDamageReceived;
}

float FSequencerStatistics::GetAverageDamageDealtPerTurn() const
{
    float AvgDamageDealt = TotalTurnsCompleted > 0 ?
        TotalDamageDealt / (float)TotalTurnsCompleted : 0.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("턴당 평균 가해량 계산: %.2f/%d = %.3f"),
        TotalDamageDealt, TotalTurnsCompleted, AvgDamageDealt);

    return AvgDamageDealt;
}

float FSequencerStatistics::GetDamageExchangeRatio() const
{
    float ExchangeRatio = TotalDamageReceived > 0.0f ?
        TotalDamageDealt / TotalDamageReceived : 0.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 교환비 계산: %.2f/%.2f = %.3f"),
        TotalDamageDealt, TotalDamageReceived, ExchangeRatio);

    return ExchangeRatio;
}

void FSequencerStatistics::ResetCurrentTurnData(float CurrentHealth)
{
    UE_LOG(LogAICombatPattern, Log, TEXT("턴 데이터 초기화 - 이전: 가해=%.2f, 피해=%.2f | 새로운 Health: %.2f"),
        CurrentTurnDamageDealt, CurrentTurnDamageReceived, CurrentHealth);

    CurrentTurnDamageReceived = 0.0f;
    CurrentTurnDamageDealt = 0.0f;
    TurnStartHealth = CurrentHealth;
    TurnEndHealth = CurrentHealth;
}

void FSequencerStatistics::CompleteTurn(float FinalHealth)
{
    UE_LOG(LogAICombatPattern, Log, TEXT("턴 완료 - Health: %.2f→%.2f, 가해: %.2f, 피해: %.2f"),
        TurnStartHealth, FinalHealth, CurrentTurnDamageDealt, CurrentTurnDamageReceived);

    TurnEndHealth = FinalHealth;
    TotalDamageReceived += CurrentTurnDamageReceived;
    TotalDamageDealt += CurrentTurnDamageDealt;
    TotalTurnsCompleted++;

    UE_LOG(LogAICombatPattern, Log, TEXT("누적 통계 업데이트 - 총 턴: %d, 총 가해: %.2f, 총 피해: %.2f"),
        TotalTurnsCompleted, TotalDamageDealt, TotalDamageReceived);
}

float FSequencerStatistics::CalculateCurrentTurnDamageEfficiency() const
{
    float Efficiency = 0.5f; // 기본값

    if (CurrentTurnDamageDealt <= 0.0f && CurrentTurnDamageReceived <= 0.0f)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("데미지 효율성: 데미지 없음 (중립값 0.5)"));
        return 0.5f;
    }

    if (CurrentTurnDamageReceived <= 0.0f)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("데미지 효율성: 피해 없음 (최고 효율 1.0)"));
        return 1.0f;
    }

    Efficiency = CurrentTurnDamageDealt / (CurrentTurnDamageDealt + CurrentTurnDamageReceived);
    float ClampedEfficiency = FMath::Clamp(Efficiency, 0.0f, 1.0f);

    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 효율성 계산: %.2f/(%.2f+%.2f) = %.3f"),
        CurrentTurnDamageDealt, CurrentTurnDamageDealt, CurrentTurnDamageReceived, ClampedEfficiency);

    return ClampedEfficiency;
}

float FSequencerStatistics::CalculateCurrentTurnSurvivalPerformance() const
{
    float HealthLossRatio = (TurnStartHealth > 0.0f) ?
        (CurrentTurnDamageReceived / TurnStartHealth) : 0.0f;

    float SurvivalScore = FMath::Clamp(1.0f - HealthLossRatio, 0.0f, 1.0f);

    UE_LOG(LogAICombatPattern, Log, TEXT("생존 성능 계산: 1.0 - (%.2f/%.2f) = %.3f"),
        CurrentTurnDamageReceived, TurnStartHealth, SurvivalScore);

    return SurvivalScore;
}

float FSequencerStatistics::CalculateCurrentTurnAttackPerformance() const
{
    const float BaselineDamage = 100.0f;
    float AttackScore = FMath::Min(CurrentTurnDamageDealt / BaselineDamage, 1.0f);
    float ClampedScore = FMath::Clamp(AttackScore, 0.0f, 1.0f);

    UE_LOG(LogAICombatPattern, Log, TEXT("공격 성능 계산: min(%.2f/%.2f, 1.0) = %.3f"),
        CurrentTurnDamageDealt, BaselineDamage, ClampedScore);

    return ClampedScore;
}

float FSequencerStatistics::CalculateCurrentTurnDamageScore() const
{
    float AttackScore = CalculateCurrentTurnAttackPerformance();
    float SurvivalScore = CalculateCurrentTurnSurvivalPerformance();
    float EfficiencyScore = CalculateCurrentTurnDamageEfficiency();

    // 가중 평균 (공격 30%, 생존 50%, 효율성 20%)
    float WeightedScore = (AttackScore * 0.3f) + (SurvivalScore * 0.5f) + (EfficiencyScore * 0.2f);

    // -1.0 ~ 1.0 범위로 정규화 (0.5를 기준으로)
    float FinalScore = (WeightedScore - 0.5f) * 2.0f;

    UE_LOG(LogAICombatPattern, Log, TEXT("종합 데미지 점수 계산: (%.3f*0.3 + %.3f*0.5 + %.3f*0.2) = %.3f → %.3f"),
        AttackScore, SurvivalScore, EfficiencyScore, WeightedScore, FinalScore);

    return FinalScore;
}

float FSequencerStatistics::GetOverallDamagePerformance() const
{
    if (TotalTurnsCompleted <= 0)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("전체 데미지 성과: 완료된 턴 없음 (0.0)"));
        return 0.0f;
    }

    float OverallEfficiency = GetDamageExchangeRatio();
    float AvgDamageDealt = GetAverageDamageDealtPerTurn();
    float AvgDamageReceived = GetAverageDamageReceivedPerTurn();

    float PerformanceScore = 0.0f;

    // 데미지 교환비 점수 (1.0 이상이면 양수, 미만이면 음수)
    if (OverallEfficiency >= 1.0f)
    {
        PerformanceScore += FMath::Min((OverallEfficiency - 1.0f), 1.0f) * 0.6f;
    }
    else
    {
        PerformanceScore -= (1.0f - OverallEfficiency) * 0.6f;
    }

    // 공격량 점수
    PerformanceScore += FMath::Min(AvgDamageDealt / 50.0f, 1.0f) * 0.2f;

    // 생존 점수 (받은 데미지가 적을수록 좋음)
    PerformanceScore += FMath::Max(0.0f, 1.0f - (AvgDamageReceived / 100.0f)) * 0.2f;

    float FinalPerformance = FMath::Clamp(PerformanceScore, -1.0f, 1.0f);

    UE_LOG(LogAICombatPattern, Log, TEXT("전체 데미지 성과: 교환비=%.3f, 평균가해=%.2f, 평균피해=%.2f → %.3f"),
        OverallEfficiency, AvgDamageDealt, AvgDamageReceived, FinalPerformance);

    return FinalPerformance;
}

FString FSequencerStatistics::GetDamageResultSummary() const
{
    FString Summary = FString::Printf(
        TEXT("턴별평균: 전달=%.1f, 수신=%.1f | 교환비=%.2f | 현재턴점수=%.2f | 전체성과=%.2f"),
        GetAverageDamageDealtPerTurn(),
        GetAverageDamageReceivedPerTurn(),
        GetDamageExchangeRatio(),
        CalculateCurrentTurnDamageScore(),
        GetOverallDamagePerformance()
    );

    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 결과 요약 생성: %s"), *Summary);

    return Summary;
}

void FSequencerStatistics::GetNormalizedDamageDataForReward(float& OutDamageReceived, float& OutDamageDealt, float& OutDamageScore) const
{
    // 데미지 수신량 정규화 (0 ~ 100 기준으로 0.0 ~ 1.0)
    OutDamageReceived = FMath::Clamp(CurrentTurnDamageReceived / 100.0f, 0.0f, 1.0f);

    // 데미지 전달량 정규화 (0 ~ 100 기준으로 0.0 ~ 1.0)
    OutDamageDealt = FMath::Clamp(CurrentTurnDamageDealt / 100.0f, 0.0f, 1.0f);

    // 종합 데미지 성과 점수 (-1.0 ~ 1.0)
    OutDamageScore = CalculateCurrentTurnDamageScore();

    UE_LOG(LogAICombatPattern, Log, TEXT("보상용 정규화 데이터: 수신=%.3f(%.2f), 전달=%.3f(%.2f), 점수=%.3f"),
        OutDamageReceived, CurrentTurnDamageReceived,
        OutDamageDealt, CurrentTurnDamageDealt,
        OutDamageScore);
}
#pragma endregion

//@Default Setting
#pragma region Default Setting
UAIAbilitySequencerComponent::UAIAbilitySequencerComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentActivatingUnitTag(FGameplayTag())
    , CurrentBlockIndex(-1)
    , CurrentUnitIndex(0)
    , bCombatReady(true)
    , bIsFirstRun(true)
    , bWaitingForExitBlock(false)
    , ExecutionStats()
    , CurrentAbilityStartTime(0.0f)
    , bEnableVerboseLogging(false)
    , bEnableLearningSystemIntegration(true)
{
    PrimaryComponentTick.bCanEverTick = false;

    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component 생성자 호출"));
}

void UAIAbilitySequencerComponent::OnRegister()
{
    Super::OnRegister();

    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component 등록"));
}

void UAIAbilitySequencerComponent::OnUnregister()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component 등록 해제"));

    Super::OnUnregister();
}

void UAIAbilitySequencerComponent::InitializeComponent()
{
    Super::InitializeComponent();

    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component 초기화 시작"));

    // 통계 초기화
    ResetExecutionStatistics();
}

void UAIAbilitySequencerComponent::DestroyComponent(bool bPromoteChildren)
{
    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component 소멸 - 최종 통계: 총실행=%d, 성공=%d, 취소=%d"),
        ExecutionStats.TotalAbilitiesExecuted, ExecutionStats.SuccessfulAbilities, ExecutionStats.CancelledAbilities);

    Super::DestroyComponent(bPromoteChildren);
}

void UAIAbilitySequencerComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component BeginPlay"));

    // 전투 시작 시간 기록
    ExecutionStats.CurrentCombatStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}

void UAIAbilitySequencerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 최종 통계 출력
    if (bEnableVerboseLogging)
    {
        PrintSequencerDebugInfo();
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("AI Ability Sequencer Component EndPlay - 종료 사유: %s"),
        *UEnum::GetValueAsString(EndPlayReason));

    Super::EndPlay(EndPlayReason);
}

void UAIAbilitySequencerComponent::ExternalBindToASC(UBaseAbilitySystemComponent* ASC)
{
    if (!ASC)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("ExternalBindToASC 실패: ASC가 유효하지 않음"));
        return;
    }

    // === 기존 어빌리티 이벤트 바인딩 ===
    ASC->AbilityActivated.AddUObject(this, &UAIAbilitySequencerComponent::OnAbilityActivated);
    ASC->AbilityEnded.AddUObject(this, &UAIAbilitySequencerComponent::OnAbilityEnded);
    ASC->AbilityCancelled.AddUObject(this, &UAIAbilitySequencerComponent::OnAbilityCancelled);

    // === 새로운 데미지 전달 이벤트 바인딩 ===
    ASC->DamageDealtByActor.AddUObject(this, &UAIAbilitySequencerComponent::OnDamageDealtByAI);

    // === Health Attribute 변경 이벤트 바인딩 ===
    if (const UBaseAttributeSet* AttributeSet = ASC->GetSet<UBaseAttributeSet>())
    {
        FGameplayAttribute HealthAttribute = AttributeSet->GetHealthAttribute();
        FGameplayAttribute MaxHealthAttribute = AttributeSet->GetMaxHealthAttribute();

        HealthChangeHandle = ASC->GetGameplayAttributeValueChangeDelegate(HealthAttribute)
            .AddUObject(this, &UAIAbilitySequencerComponent::OnHealthAttributeChanged);

        // 초기 Health 값 저장
        PreviousHealthValue = ASC->GetNumericAttribute(MaxHealthAttribute);
        bIsTrackingDamage = false;

        UE_LOG(LogAICombatPattern, Log, TEXT("Health Attribute 변경 이벤트 바인딩 완료"));
    }
    else
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("BaseAttributeSet을 찾을 수 없음 - Health 추적 불가"));
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("ASC 이벤트 바인딩 완료 (어빌리티 + 데미지전달 + Health)"));
}

void UAIAbilitySequencerComponent::ExternalBindToAIController()
{
    ABaseAIController* OwnerController = Cast<ABaseAIController>(GetOwner());
    if (!OwnerController)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("ExternalBindToAIController 실패: 소유 컨트롤러가 BaseAIController가 아님"));
        return;
    }

    // AI Controller 델리게이트 바인딩
    OwnerController->RequestStartCombatPattern.BindUFunction(this, "OnRequestActivateAICombatLoop");
    OwnerController->RequestEndCombatPattern.BindUFunction(this, "OnRequestEndCombatPattern");

    UE_LOG(LogAICombatPattern, Log, TEXT("AI Controller 델리게이트 바인딩 완료"));
}

void UAIAbilitySequencerComponent::InitializeCombatPatternComponent()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("전투 패턴 컴포넌트 초기화 시작"));

    // 소유 컨트롤러 검증
    ABaseAIController* OwnerController = Cast<ABaseAIController>(GetOwner());
    if (!OwnerController)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("초기화 실패: 소유 컨트롤러가 BaseAIController가 아님"));
        return;
    }

    // ASC 검증 및 바인딩
    auto ASC = Cast<UBaseAbilitySystemComponent>(OwnerController->GetAbilitySystemComponent());
    if (!ASC)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("초기화 실패: ASC를 찾을 수 없음"));
        return;
    }

    // 외부 바인딩 실행
    ExternalBindToAIController();
    ExternalBindToASC(ASC);

    // Game Instance 및 AI Manager 검증
    auto GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("초기화 실패: Game Instance가 유효하지 않음"));
        return;
    }

    auto AIManager = GameInstance->GetSubsystem<UAIManagerSubsystem>();
    if (!AIManager)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("초기화 실패: AI Manager Subsystem을 찾을 수 없음"));
        return;
    }

    // AI 데이터셋 가져오기
    FAIDataSet AIDataSet;
    EAIType AIType = OwnerController->GetAIType();
    if (!AIManager->GetAIDataSet(AIType, AIDataSet))
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("AI 데이터 초기화 실패: AI 타입(%s)에 대한 데이터셋이 존재하지 않음"),
            *UEnum::GetValueAsString(AIType));
        return;
    }

    // 전투 시퀀스 캐싱 및 검증
    CachedCombatSequence = AIDataSet.CombatSequence;

    // 적응형 행동 설정 캐싱 (추가)
    CachedAdaptiveBehaviorConfig = AIDataSet.AdaptiveBehaviorConfig;

    if (!ValidateCombatSequenceIntegrity())
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("전투 시퀀스 무결성 검사 실패"));
        return;
    }

    // 학습 시스템 초기화 (추가)
    if (IsAdaptiveLearningEnabled())
    {
        InitializeLearningSystem();
        UE_LOG(LogAICombatPattern, Log, TEXT("적응형 학습 시스템 초기화 완료 - AI타입: %s"), *UEnum::GetValueAsString(AIType));
    }
    else
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("기존 순환 시스템 사용 - AI타입: %s"), *UEnum::GetValueAsString(AIType));
    }

    // 시퀀서 상태 초기화
    ResetSequencerState();

    // 초기화 완료 로그
    UE_LOG(LogAICombatPattern, Log, TEXT("전투 시퀀스 초기화 완료 - AI타입: %s"), *UEnum::GetValueAsString(AIType));
    UE_LOG(LogAICombatPattern, Log, TEXT("시작 블록 ID: %s, 어빌리티 블록 수: %d"),
        *CachedCombatSequence.GetStartBlock().GetBlockIdentifier().ToString(),
        CachedCombatSequence.GetAbilityBlocks().Num());

    if (bEnableVerboseLogging)
    {
        LogCurrentSequencerState();
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("전투 패턴 컴포넌트 초기화 완료"));
}

void UAIAbilitySequencerComponent::ResetSequencerState()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("시퀀서 상태 초기화"));

    bIsFirstRun = true;
    CurrentBlockIndex = -1;
    CurrentUnitIndex = 0;
    bCombatReady = true;
    bWaitingForExitBlock = false;
    CurrentActivatingUnitTag = FGameplayTag();
    CurrentAbilityStartTime = 0.0f;

    // 새로운 전투 사이클 시작
    if (GetWorld())
    {
        ExecutionStats.CurrentCombatStartTime = GetWorld()->GetTimeSeconds();
    }

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("시퀀서 상태 초기화 완료 - 실행상태: %s"),
            *UEnum::GetValueAsString(GetCurrentExecutionState()));
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAIAbilitySequencerComponent::InitializeLearningSystem()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("학습 시스템 초기화 시작"));

    // 런타임 학습 상태 초기화
    RuntimeLearningState = FAIRuntimeLearningState();

    // 실행 계획 상태 초기화
    CurrentSelectedBlockTag = FGameplayTag();
    PlannedExecutionCount = 0;
    CompletedExecutionCount = 0;
    bIsCurrentlyInExplorationMode = false;

    // 초기 학습률 설정
    RuntimeLearningState.CurrentLearningRate = CachedAdaptiveBehaviorConfig.RewardCalculator.BaseLearningRate;

    UE_LOG(LogAICombatPattern, Log, TEXT("학습 시스템 초기화 완료 - 초기 학습률: %.3f"), RuntimeLearningState.CurrentLearningRate);

    // 초기 선호도 로깅
    if (bEnableVerboseLogging)
    {
        float OpeningPref = CalculateCurrentActionPreference(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills")));
        float SkillsPref = CalculateCurrentActionPreference(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills")));
        float DodgePref = CalculateCurrentActionPreference(FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge")));

        UE_LOG(LogAICombatPattern, Log, TEXT("초기 선호도 - Opening: %.2f, Skills: %.2f, Dodge: %.2f"),
            OpeningPref, SkillsPref, DodgePref);
    }
}

bool UAIAbilitySequencerComponent::ExecuteAbilityActivationRequest(const FAIAbilityBlockUnit* CurrentUnit, const FString& BlockType)
{
    if (!CurrentUnit)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("ExecuteAbilityActivationRequest: 현재 유닛이 유효하지 않음"));
        return false;
    }

    // 어빌리티 태그 유효성 검사
    if (!CurrentUnit->GetAbilityTag().IsValid())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("유효하지 않은 어빌리티 태그 - BlockType: %s, UnitIndex: %d"),
            *BlockType, CurrentUnitIndex);
        return false;
    }

    // 전투 준비 비활성화
    SetCombatReady(false);

    // 현재 실행할 어빌리티 태그 저장
    CurrentActivatingUnitTag = CurrentUnit->GetAbilityTag();

    LogAbilityExecution(CurrentActivatingUnitTag, TEXT("활성화 요청"));

    // 어빌리티 활성화 요청
    if (!RequestActivateAbilityBlockUnit.Execute(CurrentActivatingUnitTag))
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("어빌리티 활성화 요청 실패: %s Unit[%d] Tag[%s]"),
            *BlockType, CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());

        // 통계 업데이트 (실패)
        UpdateExecutionStatistics(CurrentActivatingUnitTag, false, false, 0.0f);

        // 실패 시 상태 복구
        CurrentActivatingUnitTag = FGameplayTag();
        SetCombatReady(true);

        return false;
    }

    // 성공 로그
    UE_LOG(LogAICombatPattern, Log, TEXT("어빌리티 유닛 활성화 요청 성공: %s Unit[%d] Tag[%s] Priority[%d]"),
        *BlockType, CurrentUnitIndex, *CurrentActivatingUnitTag.ToString(), CurrentUnit->GetPriority());

    // 실행 시작 시간 기록
    if (GetWorld())
    {
        CurrentAbilityStartTime = GetWorld()->GetTimeSeconds();
    }

    return true;
}

float UAIAbilitySequencerComponent::CalculateCurrentActionPreference(const FGameplayTag& ActionCategory) const
{
    if (!IsAdaptiveLearningEnabled())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("학습 시스템이 비활성화된 상태에서 선호도 계산 요청"));
        return 50.0f;
    }

    const FAISimplifiedPersonality& Personality = CachedAdaptiveBehaviorConfig.SimplifiedPersonality;
    float CurrentPreference = RuntimeLearningState.GetCurrentActionPreference(ActionCategory, Personality);

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("선호도 계산: %s = %.2f"),
            *ActionCategory.ToString(), CurrentPreference);
    }

    return CurrentPreference;
}

FGameplayTag UAIAbilitySequencerComponent::SelectNextAbilityBlockWithEpsilonGreedy()
{
    if (!IsAdaptiveLearningEnabled())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("학습 시스템이 비활성화된 상태에서 블록 선택 요청"));
        return FGameplayTag();
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("ε-greedy 블록 선택 시작"));

    // === Health 추적 시작 ===
    StartDamageTracking();

    FGameplayTag SelectedTag;
    bool bIsExplorationMode = ShouldExplore();
    bIsCurrentlyInExplorationMode = bIsExplorationMode;

    if (bIsExplorationMode)
    {
        // 탐험: 랜덤 블록 선택
        SelectedTag = SelectRandomBlock();
        UE_LOG(LogAICombatPattern, Log, TEXT("탐험 모드: 랜덤 블록 선택 - %s"), *SelectedTag.ToString());
    }
    else
    {
        // 활용: 선호도 기반 블록 선택
        SelectedTag = SelectBlockByPreference();
        UE_LOG(LogAICombatPattern, Log, TEXT("활용 모드: 선호도 기반 블록 선택 - %s"), *SelectedTag.ToString());
    }

    // 선택된 블록의 실행 개수 계산 및 계획 설정
    int32 ExecutionCount = CalculateExecutionCountForSelectedBlock(SelectedTag, bIsExplorationMode);
    SetCurrentBlockExecutionPlan(SelectedTag, ExecutionCount);

    UE_LOG(LogAICombatPattern, Log, TEXT("블록 선택 및 실행 계획 완료 - 블록: %s, 실행 개수: %d, 탐험 모드: %s, Health 추적: 활성"),
        *SelectedTag.ToString(), ExecutionCount, bIsExplorationMode ? TEXT("활성") : TEXT("비활성"));

    return SelectedTag;
}

FGameplayTag UAIAbilitySequencerComponent::SelectBlockByPreference() const
{
    // 각 카테고리별 선호도 계산
    FGameplayTag OpeningTag = FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills"));
    FGameplayTag SkillsTag = FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills"));
    FGameplayTag DodgeTag = FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"));

    float OpeningPref = CalculateCurrentActionPreference(OpeningTag);
    float SkillsPref = CalculateCurrentActionPreference(SkillsTag);
    float DodgePref = CalculateCurrentActionPreference(DodgeTag);

    // 확률 정규화를 위한 배열 생성
    TArray<float> Probabilities = { OpeningPref, SkillsPref, DodgePref };
    TArray<FGameplayTag> CategoryTags = { OpeningTag, SkillsTag, DodgeTag };

    // 최소 확률 보장 및 정규화 (const_cast 문제 해결 필요)
    FAIExperienceWeightConfig Config = CachedAdaptiveBehaviorConfig.ExperienceWeightConfig;
    Config.NormalizeCategoryProbabilities(Probabilities);

    // 확률적 선택
    float RandomValue = FMath::FRand();
    float CumulativeProbability = 0.0f;

    for (int32 i = 0; i < Probabilities.Num(); ++i)
    {
        CumulativeProbability += Probabilities[i];
        if (RandomValue <= CumulativeProbability)
        {
            if (bEnableVerboseLogging)
            {
                UE_LOG(LogAICombatPattern, Log, TEXT("확률적 선택: %s (확률: %.3f, 랜덤값: %.3f)"),
                    *CategoryTags[i].ToString(), Probabilities[i], RandomValue);
            }
            return CategoryTags[i];
        }
    }

    // 안전장치: 마지막 카테고리 반환
    UE_LOG(LogAICombatPattern, Warning, TEXT("확률적 선택 실패, 마지막 카테고리 반환: %s"), *CategoryTags.Last().ToString());
    return CategoryTags.Last();
}

FGameplayTag UAIAbilitySequencerComponent::SelectRandomBlock() const
{
    TArray<FGameplayTag> AvailableBlocks = {
        FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.OpeningSkills")),
        FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.Skills")),
        FGameplayTag::RequestGameplayTag(TEXT("AbilityBlock.StrafeOrDodge"))
    };

    int32 RandomIndex = FMath::RandRange(0, AvailableBlocks.Num() - 1);
    FGameplayTag SelectedTag = AvailableBlocks[RandomIndex];

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("랜덤 블록 선택: %s (인덱스: %d)"),
            *SelectedTag.ToString(), RandomIndex);
    }

    return SelectedTag;
}

int32 UAIAbilitySequencerComponent::CalculateExecutionCountForSelectedBlock(const FGameplayTag& SelectedBlockTag, bool bIsExplorationMode) const
{
    if (!IsAdaptiveLearningEnabled())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("학습 시스템이 비활성화된 상태에서 실행 개수 계산 요청"));
        return 1;
    }

    UE_LOG(LogAICombatPattern, Log,
        TEXT("선택된 블록의 실행 개수 계산 시작 - 블록: %s, 탐험모드: %s"),
        *SelectedBlockTag.ToString(), bIsExplorationMode ? TEXT("활성") : TEXT("비활성"));

    // 현재 선호도 계산
    float CurrentPreference = CalculateCurrentActionPreference(SelectedBlockTag);

    // ExecutionConfig를 통한 이론적 실행 개수 계산
    const FAIAdaptiveExecutionConfig& ExecutionConfig = CachedAdaptiveBehaviorConfig.ExecutionConfig;
    int32 DesiredCount = ExecutionConfig.CalculateExecutionCountByPreference(SelectedBlockTag, CurrentPreference, bIsExplorationMode);

    // 실제 블록에서 사용 가능한 유닛 개수 확인
    int32 ActualAvailableUnits = 0;
    const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();

    for (const FAIAbilityBlock& Block : AbilityBlocks)
    {
        if (Block.GetBlockIdentifier().MatchesTag(SelectedBlockTag))
        {
            ActualAvailableUnits = Block.GetAbilityBlockUnits().Num();
            break;
        }
    }

    // 실제 유닛 개수로 제한
    int32 FinalCount = FMath::Min(DesiredCount, ActualAvailableUnits);

    if (FinalCount < DesiredCount)
    {
        UE_LOG(LogAICombatPattern, Warning,
            TEXT("유닛 부족으로 실행 개수 조정 - 희망: %d개, 실제유닛: %d개, 최종: %d개, 블록: %s"),
            DesiredCount, ActualAvailableUnits, FinalCount, *SelectedBlockTag.ToString());
    }

    UE_LOG(LogAICombatPattern, Log,
        TEXT("실행 개수 계산 완료 - 블록: %s, 선호도: %.2f, 최종 개수: %d (유닛제한 적용)"),
        *SelectedBlockTag.ToString(), CurrentPreference, FinalCount);

    return FinalCount;
}

void UAIAbilitySequencerComponent::SetCurrentBlockExecutionPlan(const FGameplayTag& BlockTag, int32 ExecutionCount)
{
    CurrentSelectedBlockTag = BlockTag;
    PlannedExecutionCount = ExecutionCount;
    CompletedExecutionCount = 0;

    UE_LOG(LogAICombatPattern, Log, TEXT("블록 실행 계획 설정 - 블록: %s, 계획 실행 개수: %d"),
        *BlockTag.ToString(), ExecutionCount);
}

bool UAIAbilitySequencerComponent::HasRemainingUnitsToExecute() const
{
    bool bHasRemaining = CompletedExecutionCount < PlannedExecutionCount;

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("실행 계획 확인 - 완료: %d, 계획: %d, 남은 실행: %s"),
            CompletedExecutionCount, PlannedExecutionCount, bHasRemaining ? TEXT("있음") : TEXT("없음"));
    }

    return bHasRemaining;
}

void UAIAbilitySequencerComponent::AdvanceToNextUnit()
{
    int32 PreviousUnitIndex = CurrentUnitIndex;

    // 학습 모드에서는 완료 카운트 기반으로 진행 상태 관리
    if (IsAdaptiveLearningEnabled() && IsExecutingAbilityBlocks())
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("적응형 모드 진행 상태 확인: %d/%d 완료"),
            CompletedExecutionCount, PlannedExecutionCount);

        // 계획된 실행이 완료되었는지 확인
        if (CompletedExecutionCount >= PlannedExecutionCount)
        {
            UE_LOG(LogAICombatPattern, Log, TEXT("적응형 블록(%s) 실행 계획 완료 - 블록 전환 진행"),
                *CurrentSelectedBlockTag.ToString());
            AdvanceToNextBlock(); // 여기서만 블록 선택 발생
            return;
        }

        // 아직 실행할 것이 남아있으면 계속 진행
        UE_LOG(LogAICombatPattern, Log, TEXT("적응형 블록(%s) 계속 실행 - 다음 RequestActivateAICombatLoop에서 처리"),
            *CurrentSelectedBlockTag.ToString());
        SetCombatReady(true); // 다음 호출을 위해 준비 상태로
        return;
    }

    // 기존 방식: 인덱스 기반 진행
    CurrentUnitIndex++;

    // 현재 블록의 유닛 정보 가져오기
    const TArray<FAIAbilityBlockUnit>* CurrentUnits = nullptr;
    FString BlockName;

    if (IsExecutingStartBlock())
    {
        CurrentUnits = &CachedCombatSequence.GetStartBlock().GetAbilityBlockUnits();
        BlockName = TEXT("StartBlock");
    }
    else if (IsExecutingAbilityBlocks())
    {
        CurrentUnits = &CachedCombatSequence.GetAbilityBlocks()[CurrentBlockIndex].GetAbilityBlockUnits();
        BlockName = FString::Printf(TEXT("AbilityBlock[%d]"), CurrentBlockIndex);
    }
    else if (IsExecutingExitBlock())
    {
        CurrentUnits = &CachedCombatSequence.GetExitBlock().GetAbilityBlockUnits();
        BlockName = TEXT("ExitBlock");
    }

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("%s: 유닛 진행 %d → %d"),
            *BlockName, PreviousUnitIndex, CurrentUnitIndex);
    }

    // 현재 블록의 모든 유닛을 실행했는지 체크
    if (CurrentUnits && CurrentUnitIndex >= CurrentUnits->Num())
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("%s의 모든 유닛 실행 완료 (%d개) - 블록 전환 진행"),
            *BlockName, CurrentUnits->Num());
        AdvanceToNextBlock(); // 여기서만 블록 선택 발생
    }
}

void UAIAbilitySequencerComponent::AdvanceToNextBlock()
{
    int32 PreviousBlockIndex = CurrentBlockIndex;
    ESequencerExecutionState PreviousState = GetCurrentExecutionState();

    CurrentUnitIndex = 0;

    // 최초 실행 시 (Start Block → AbilityBlocks)
    if (bIsFirstRun)
    {
        if (IsExecutingStartBlock())
        {
            bIsFirstRun = false;

            if (IsAdaptiveLearningEnabled())
            {
                UE_LOG(LogAICombatPattern, Log, TEXT("Start Block 완료 - 첫 번째 적응형 블록 선택 시작"));
                FGameplayTag SelectedBlockTag = SelectNextAbilityBlockWithEpsilonGreedy();

                const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();
                CurrentBlockIndex = 0;

                for (int32 i = 0; i < AbilityBlocks.Num(); ++i)
                {
                    if (AbilityBlocks[i].GetBlockIdentifier().MatchesTag(SelectedBlockTag))
                    {
                        CurrentBlockIndex = i;
                        break;
                    }
                }

                LogBlockTransition(TEXT("StartBlock"), FString::Printf(TEXT("AdaptiveBlock[%d:%s]"),
                    CurrentBlockIndex, *SelectedBlockTag.ToString()));
                UE_LOG(LogAICombatPattern, Log,
                    TEXT("Start Block → 적응형 블록 선택 완료: %s (실행 계획: %d개, Health 추적: 활성)"),
                    *SelectedBlockTag.ToString(), PlannedExecutionCount);
            }
            else
            {
                CurrentBlockIndex = 0;
                LogBlockTransition(TEXT("StartBlock"), FString::Printf(TEXT("AbilityBlock[%d]"), CurrentBlockIndex));
                UE_LOG(LogAICombatPattern, Log, TEXT("Start Block 완료, AbilityBlocks 시작"));
            }

            SetCombatReady(true);
            return;
        }
    }

    // AbilityBlocks 실행 중 - 턴 종료 후 새로운 블록 선택
    if (IsExecutingAbilityBlocks())
    {
        if (IsAdaptiveLearningEnabled())
        {
            UE_LOG(LogAICombatPattern, Log,
                TEXT("적응형 턴 완료 (%s: %d/%d) - Health 추적 종료 및 학습 데이터 처리 시작"),
                *CurrentSelectedBlockTag.ToString(), CompletedExecutionCount, PlannedExecutionCount);

            // === 1. 데미지 추적 종료 ===
            EndDamageTracking();

            // === 2. 모든 학습 데이터 수집 완료 시점 → 가중치 업데이트 실행 ===
            if (CurrentSelectedBlockTag.IsValid())
            {
                UpdateExperienceWeights(CurrentSelectedBlockTag);
                UE_LOG(LogAICombatPattern, Log, TEXT("가중치 업데이트 완료: %s"), *CurrentSelectedBlockTag.ToString());
            }

            // === 3. 새로운 블록 선택 및 실행 계획 수립 ===
            FGameplayTag SelectedBlockTag = SelectNextAbilityBlockWithEpsilonGreedy();

            int32 NewBlockIndex = 0;
            const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();

            for (int32 i = 0; i < AbilityBlocks.Num(); ++i)
            {
                if (AbilityBlocks[i].GetBlockIdentifier().MatchesTag(SelectedBlockTag))
                {
                    NewBlockIndex = i;
                    break;
                }
            }

            CurrentBlockIndex = NewBlockIndex;
            LogBlockTransition(FString::Printf(TEXT("AdaptiveBlock[%d]"), PreviousBlockIndex),
                FString::Printf(TEXT("AdaptiveBlock[%d:%s]"), CurrentBlockIndex, *SelectedBlockTag.ToString()));

            UE_LOG(LogAICombatPattern, Log,
                TEXT("적응형 블록 전환 완료: %d → %d (%s, 실행 계획: %d개, Health 추적: 새로 시작)"),
                PreviousBlockIndex, CurrentBlockIndex, *SelectedBlockTag.ToString(), PlannedExecutionCount);
        }
        else
        {
            // 기존 방식: 순환 구조
            const int32 TotalAbilityBlocks = CachedCombatSequence.GetAbilityBlocks().Num();
            if (CurrentBlockIndex >= TotalAbilityBlocks - 1)
            {
                CurrentBlockIndex = 0;
            }
            else
            {
                CurrentBlockIndex++;
            }

            LogBlockTransition(FString::Printf(TEXT("AbilityBlock[%d]"), PreviousBlockIndex),
                FString::Printf(TEXT("AbilityBlock[%d]"), CurrentBlockIndex));
            UE_LOG(LogAICombatPattern, Log, TEXT("순환 블록 이동: %d → %d"), PreviousBlockIndex, CurrentBlockIndex);
        }

        SetCombatReady(true);
        return;
    }

    // Exit Block 실행 완료 (기존 로직 유지)
    if (IsExecutingExitBlock())
    {
        // === Exit Block 완료 시 Health 추적 종료 (만약 활성화되어 있다면) ===
        if (IsAdaptiveLearningEnabled() && bIsTrackingDamage)
        {
            EndDamageTracking();
            UE_LOG(LogAICombatPattern, Log, TEXT("Exit Block 완료 - 최종 Health 추적 종료"));
        }

        UE_LOG(LogAICombatPattern, Log, TEXT("Exit Block 완료, 전투 패턴 완전 종료"));
        return;
    }

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("블록 전환 완료 - 이전상태: %s, 현재상태: %s"),
            *UEnum::GetValueAsString(PreviousState), *UEnum::GetValueAsString(GetCurrentExecutionState()));
    }
}

bool UAIAbilitySequencerComponent::StartExitBlock()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("Exit Block 시작"));

    LogBlockTransition(TEXT("Current"), TEXT("ExitBlock"));

    // Exit Block 설정
    SetExecutingExitBlock();
    CurrentUnitIndex = 0;
    bWaitingForExitBlock = false;

    // Exit Block의 유닛들 순차 실행
    ExecuteExitBlockUnits();

    return true;
}

void UAIAbilitySequencerComponent::ExecuteExitBlockUnits()
{
    const auto& ExitBlock = CachedCombatSequence.GetExitBlock();
    const auto& ExitBlockUnits = ExitBlock.GetAbilityBlockUnits();

    UE_LOG(LogAICombatPattern, Log, TEXT("Exit Block 유닛 실행 - 현재 유닛: %d/%d"),
        CurrentUnitIndex, ExitBlockUnits.Num());

    // 모든 Exit Block 유닛 실행 완료 확인
    if (CurrentUnitIndex >= ExitBlockUnits.Num())
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("Exit Block의 모든 유닛 실행 완료, 전투 패턴 종료"));

        // 전투 사이클 통계 업데이트
        if (GetWorld())
        {
            float CombatDuration = GetWorld()->GetTimeSeconds() - ExecutionStats.CurrentCombatStartTime;
            ExecutionStats.TotalCombatTime += CombatDuration;
            ExecutionStats.TotalCombatCycles++;

            UE_LOG(LogAICombatPattern, Log, TEXT("전투 사이클 완료 - 지속시간: %.2f초, 총 사이클: %d, 평균시간: %.2f초"),
                CombatDuration, ExecutionStats.TotalCombatCycles, ExecutionStats.GetAverageCombatTime());
        }

        // 컴포넌트 상태 초기화
        ResetSequencerState();

        // AI Controller에 완료 통지
        if (!NotifyCombatPatternExitComplete.IsBound())
        {
            UE_LOG(LogAICombatPattern, Warning, TEXT("Exit Block 완료 통지 델리게이트가 바인딩되지 않음"));
            return;
        }

        const bool bNotifySuccess = NotifyCombatPatternExitComplete.Execute();
        UE_LOG(LogAICombatPattern, Log, TEXT("AI Controller에 Exit Block 완료 통지: %s"),
            bNotifySuccess ? TEXT("성공") : TEXT("실패"));
        return;
    }

    // 현재 유닛 가져오기 및 검증
    const auto& CurrentUnit = ExitBlockUnits[CurrentUnitIndex];

    if (!CurrentUnit.GetAbilityTag().IsValid())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("Exit Block 유닛[%d]의 어빌리티 태그가 유효하지 않음"), CurrentUnitIndex);

        // 다음 유닛으로 이동
        CurrentUnitIndex++;
        ExecuteExitBlockUnits();
        return;
    }

    // 현재 실행할 어빌리티 태그 저장
    CurrentActivatingUnitTag = CurrentUnit.GetAbilityTag();

    LogAbilityExecution(CurrentActivatingUnitTag, TEXT("Exit Block 실행 요청"));

    // 어빌리티 활성화 요청
    if (!RequestActivateAbilityBlockUnit.Execute(CurrentActivatingUnitTag))
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("Exit Block 어빌리티 활성화 요청 실패: Unit[%d] Tag[%s]"),
            CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());

        // 통계 업데이트 (실패)
        UpdateExecutionStatistics(CurrentActivatingUnitTag, false, false, 0.0f);

        // 실패 시 다음 유닛으로
        CurrentActivatingUnitTag = FGameplayTag();
        CurrentUnitIndex++;
        ExecuteExitBlockUnits();
        return;
    }

    // 요청 성공
    UE_LOG(LogAICombatPattern, Log, TEXT("Exit Block 어빌리티 유닛 활성화 요청 성공: Unit[%d] Tag[%s]"),
        CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());

    // 실행 시작 시간 기록
    if (GetWorld())
    {
        CurrentAbilityStartTime = GetWorld()->GetTimeSeconds();
    }
}

void UAIAbilitySequencerComponent::UpdateExperienceWeights(const FGameplayTag& CompletedBlockTag)
{
    if (!IsAdaptiveLearningEnabled())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("적응형 학습이 비활성화됨"));
        return;
    }

    if (!CompletedBlockTag.IsValid())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("유효하지 않은 블록 태그로 가중치 업데이트 불가"));
        return;
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("경험 가중치 업데이트 시작 - 블록: %s"), *CompletedBlockTag.ToString());

    // 1. 보상 계산을 위한 데이터 수집
    // 1-1. 데미지
    float DamageReceived = ExecutionStats.CurrentTurnDamageReceived;
    float DamageDealt = ExecutionStats.CurrentTurnDamageDealt;
    // 1-2. 최적 거리
    float CurrentDistance = CalcualteOptimalCombatDistance();
    // 1-3. 어빌리티 성공률
    bool bAbilitySuccess = CalculateCurrentTurnAbilitySuccess();

    UE_LOG(LogAICombatPattern, Log, TEXT("학습 데이터 - 데미지: %.2f, 거리: %.2f, 성공: %s"),
        DamageReceived, CurrentDistance, bAbilitySuccess ? TEXT("True") : TEXT("False"));

    // 2. 정규화된 보상 계산
    float NormalizedReward = CachedAdaptiveBehaviorConfig.RewardCalculator.CalculateNormalizedReward(
        DamageReceived,      // 받은 데미지 (음수 처리)
        DamageDealt,         // 전달 데미지 (양수 처리) - 추가
        CurrentDistance,
        bAbilitySuccess
    );

    UE_LOG(LogAICombatPattern, Log, TEXT("정규화된 보상: %.3f"), NormalizedReward);

    // 3. 적응적 학습률 적용
    float AdaptiveLearningRate = CachedAdaptiveBehaviorConfig.RewardCalculator.CalculateAdaptiveLearningRate(
        RuntimeLearningState.LearningCount
    );

    // 4. 가중치 변화량 계산
    float WeightChange = NormalizedReward * AdaptiveLearningRate;

    UE_LOG(LogAICombatPattern, Log, TEXT("학습률: %.3f, 가중치 변화량: %.4f"), AdaptiveLearningRate, WeightChange);

    // 5. 현재 턴에서 실행한 블록에 대해 가중치 업데이트
    RuntimeLearningState.UpdateExperienceWeight(
        CompletedBlockTag,
        WeightChange,
        CachedAdaptiveBehaviorConfig.ExperienceWeightConfig,
        CachedAdaptiveBehaviorConfig.SimplifiedPersonality
    );

    // 6. Overdrive 방지 체크
    CheckAndApplyOverdrivePrevention();

    UE_LOG(LogAICombatPattern, Log, TEXT("경험 가중치 업데이트 완료 - 총 학습 횟수: %d"), RuntimeLearningState.LearningCount);

    // 상세 로깅
    if (bEnableVerboseLogging)
    {
        LogDetailedLearningAnalysis(CompletedBlockTag, NormalizedReward, 0.0f, NormalizedReward, WeightChange);
    }
}

void UAIAbilitySequencerComponent::AnalyzeDamagePerformance()
{
    if (!bIsTrackingDamage)
    {
        return;
    }

    // 현재 턴 성과 분석
    float CurrentScore = ExecutionStats.CalculateCurrentTurnDamageScore();
    float Efficiency = ExecutionStats.CalculateCurrentTurnDamageEfficiency();
    float SurvivalScore = ExecutionStats.CalculateCurrentTurnSurvivalPerformance();
    float AttackScore = ExecutionStats.CalculateCurrentTurnAttackPerformance();

    UE_LOG(LogAICombatPattern, Log,
        TEXT("턴 데미지 성과 분석 - 종합점수: %.3f, 효율성: %.3f, 생존: %.3f, 공격: %.3f"),
        CurrentScore, Efficiency, SurvivalScore, AttackScore);

    // 상세 로그 (Verbose 모드에서만)
    if (bEnableVerboseLogging)
    {
        LogCurrentTurnDamagePerformance();
    }

    // 성과에 따른 추가 분석
    if (CurrentScore > 0.5f)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("우수한 데미지 성과 - 현재 전략 유지 권장"));
    }
    else if (CurrentScore < -0.5f)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("저조한 데미지 성과 - 전략 변경 고려 필요"));
    }
}

void UAIAbilitySequencerComponent::StartDamageTracking()
{
    if (bIsTrackingDamage)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("데미지 추적이 이미 활성화되어 있음"));
        return;
    }

    if (ABaseAIController* AIController = Cast<ABaseAIController>(GetOwner()))
    {
        if (UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(AIController->GetAbilitySystemComponent()))
        {
            if (const UBaseAttributeSet* AttributeSet = ASC->GetSet<UBaseAttributeSet>())
            {
                float CurrentHealth = ASC->GetNumericAttribute(AttributeSet->GetHealthAttribute());
                ExecutionStats.ResetCurrentTurnData(CurrentHealth);
                PreviousHealthValue = CurrentHealth;

                bIsTrackingDamage = true;

                UE_LOG(LogAICombatPattern, Log, TEXT("데미지 추적 시작 - 초기 Health: %.2f"), CurrentHealth);
            }
        }
    }
}

void UAIAbilitySequencerComponent::EndDamageTracking()
{
    if (!bIsTrackingDamage)
    {
        return;
    }

    bIsTrackingDamage = false;

    // 턴 완료 처리
    ExecutionStats.CompleteTurn(PreviousHealthValue);

    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 추적 종료 - 턴 데미지 수신: %.2f, 턴 데미지 전달: %.2f"),
        ExecutionStats.CurrentTurnDamageReceived, ExecutionStats.CurrentTurnDamageDealt);

    // 데미지 성과 분석
    AnalyzeDamagePerformance();

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("턴 완료 통계 - %s"), *ExecutionStats.GetDamageResultSummary());
    }
}

float UAIAbilitySequencerComponent::CalcualteOptimalCombatDistance() const
{
    ABaseAIController* AIController = Cast<ABaseAIController>(GetOwner());
    if (AIController)
    {
        // 현재 근접형 AI이므로 MinAttackRange를 최적 거리로 사용
        float OptimalDistance = AIController->GetMinAttackRange();

        // RewardCalculator의 OptimalCombatDistance 값을 동적으로 업데이트
        const_cast<FAIRewardCalculator&>(CachedAdaptiveBehaviorConfig.RewardCalculator).OptimalCombatDistance = OptimalDistance;

        UE_LOG(LogAICombatPattern, Log, TEXT("최적 거리 업데이트: %.2f (MinAttackRange 기준)"), OptimalDistance);

        return OptimalDistance;
    }

    return -1.f;
}

bool UAIAbilitySequencerComponent::CalculateCurrentTurnAbilitySuccess() const
{
    // 현재 턴에서 실행된 어빌리티 성공률 기준
    if (CompletedExecutionCount == 0)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("현재 턴 어빌리티 성공률: 실행된 어빌리티 없음 - false 반환"));
        return false;
    }

    // 현재 턴의 성공률 = 완료된 어빌리티 / 계획된 어빌리티
    float CurrentTurnSuccessRate = static_cast<float>(CompletedExecutionCount) / static_cast<float>(PlannedExecutionCount);

    // 70% 이상이면 성공으로 판정 (임계값은 조정 가능)
    bool bTurnSuccess = CurrentTurnSuccessRate >= 0.7f;

    UE_LOG(LogAICombatPattern, Log, TEXT("현재 턴 어빌리티 성공률: %.2f%% (%d/%d) - %s"),
        CurrentTurnSuccessRate * 100.0f, CompletedExecutionCount, PlannedExecutionCount,
        bTurnSuccess ? TEXT("성공") : TEXT("실패"));

    return bTurnSuccess;
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
bool UAIAbilitySequencerComponent::OnRequestActivateAICombatLoop()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("전투 루프 활성화 요청 수신"));

    // 실행 요청 검증
    if (!ValidateExecutionRequest())
    {
        return false;
    }

    // 현재 실행할 유닛 정보 가져오기
    FString BlockType;
    int32 TotalUnits = 0;
    const FAIAbilityBlockUnit* CurrentUnit = GetCurrentExecutingUnit(BlockType, TotalUnits);

    // 유닛이 없으면 턴 종료 처리
    if (!CurrentUnit)
    {
        if (IsAdaptiveLearningEnabled() && IsExecutingAbilityBlocks())
        {
            // 적응형 모드에서 턴 종료 처리
            bool bEarlyTermination = BlockType.Contains(TEXT("EarlyComplete"));

            if (bEarlyTermination)
            {
                UE_LOG(LogAICombatPattern, Log,
                    TEXT("유닛 부족으로 인한 조기 턴 종료 - 완료: %d/%d, 블록: %s"),
                    CompletedExecutionCount, PlannedExecutionCount, *CurrentSelectedBlockTag.ToString());

                // 부분 완료 통계 업데이트
                UpdatePartialCompletionStatistics(CurrentSelectedBlockTag, CompletedExecutionCount, PlannedExecutionCount);
            }
            else
            {
                UE_LOG(LogAICombatPattern, Log,
                    TEXT("계획된 실행 완료로 인한 정상 턴 종료 - 완료: %d/%d, 블록: %s"),
                    CompletedExecutionCount, PlannedExecutionCount, *CurrentSelectedBlockTag.ToString());
            }

            // 다음 턴 준비 (새로운 블록 선택)
            AdvanceToNextBlock();
            return true;
        }
        else
        {
            UE_LOG(LogAICombatPattern, Warning, TEXT("유효하지 않은 유닛 - BlockType: %s"), *BlockType);
            AdvanceToNextUnit();
            return true;
        }
    }

    // 유닛이 있으면 실행
    return ExecuteAbilityActivationRequest(CurrentUnit, BlockType);
}

bool UAIAbilitySequencerComponent::OnRequestEndCombatPattern()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("전투 패턴 종료 요청 수신 - 현재 상태: %s"),
        *UEnum::GetValueAsString(GetCurrentExecutionState()));

    // 이미 Exit Block을 실행 중인지 확인
    if (IsExecutingExitBlock())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("이미 Exit Block을 실행 중입니다"));
        return false;
    }

    // 현재 진행 중인 유닛이 있는 경우
    if (CurrentActivatingUnitTag.IsValid())
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("현재 실행 중인 블록 유닛 완료 후 Exit Block 시작 대기: %s"),
            *CurrentActivatingUnitTag.ToString());

        // Exit Block 대기 상태로 전환
        bWaitingForExitBlock = true;

        return true;
    }

    // 실행 중인 유닛이 없으면 바로 Exit Block 시작
    return StartExitBlock();
}

void UAIAbilitySequencerComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("OnAbilityActivated: 유효하지 않은 어빌리티"));
        return;
    }

    // 현재 실행 중인 어빌리티가 아니면 무시
    if (!ValidateAbilityTag(Ability))
    {
        return;
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("시퀀서 어빌리티 활성화: %s [%s]"),
        *Ability->GetName(), *CurrentActivatingUnitTag.ToString());

    LogAbilityExecution(CurrentActivatingUnitTag, TEXT("활성화 완료"));
}

void UAIAbilitySequencerComponent::OnAbilityEnded(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("OnAbilityEnded: 유효하지 않은 어빌리티"));
        return;
    }

    // 현재 실행 중인 어빌리티가 아니면 무시
    if (!ValidateAbilityTag(Ability))
    {
        return;
    }

    // 실행 시간 계산
    float ExecutionTime = 0.0f;
    if (GetWorld() && CurrentAbilityStartTime > 0.0f)
    {
        ExecutionTime = GetWorld()->GetTimeSeconds() - CurrentAbilityStartTime;
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("시퀀서 어빌리티 종료: %s [%s] (실행시간: %.2f초)"),
        *Ability->GetName(), *CurrentActivatingUnitTag.ToString(), ExecutionTime);

    LogAbilityExecution(CurrentActivatingUnitTag, TEXT("정상 종료"));

    // 통계 업데이트 (성공)
    UpdateExecutionStatistics(CurrentActivatingUnitTag, true, false, ExecutionTime);

    // 학습 모드에서 완료 카운트 증가
    if (IsAdaptiveLearningEnabled() && IsExecutingAbilityBlocks())
    {
        CompletedExecutionCount++;
        UE_LOG(LogAICombatPattern, Log, TEXT("적응형 실행 완료 카운트 증가: %d/%d (블록: %s, 어빌리티: %s)"),
            CompletedExecutionCount, PlannedExecutionCount,
            *CurrentSelectedBlockTag.ToString(), *CurrentActivatingUnitTag.ToString());
    }

    // 태그 초기화
    FGameplayTag CompletedAbilityTag = CurrentActivatingUnitTag;
    CurrentActivatingUnitTag = FGameplayTag();
    CurrentAbilityStartTime = 0.0f;

    // Exit Block 실행 중이면 다음 Exit 유닛 실행
    if (IsExecutingExitBlock())
    {
        CurrentUnitIndex++;
        ExecuteExitBlockUnits();
    }
    // Exit Block 대기 중이었다면 시작
    else if (bWaitingForExitBlock)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("대기 중이던 Exit Block 시작"));
        bWaitingForExitBlock = false;
        StartExitBlock();
    }
    // 일반 실행 중이면 다음 유닛으로
    else
    {
        AdvanceToNextUnit();
        SetCombatReady(true);
    }

    // 학습 시스템에 알림 (성공)
    NotifyLearningSystem(CompletedAbilityTag, true, false, ExecutionTime);
}

void UAIAbilitySequencerComponent::OnAbilityCancelled(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("OnAbilityCancelled: 유효하지 않은 어빌리티"));
        return;
    }

    // 현재 실행 중인 어빌리티가 아니면 무시
    if (!ValidateAbilityTag(Ability))
    {
        return;
    }

    // 실행 시간 계산
    float ExecutionTime = 0.0f;
    if (GetWorld() && CurrentAbilityStartTime > 0.0f)
    {
        ExecutionTime = GetWorld()->GetTimeSeconds() - CurrentAbilityStartTime;
    }

    UE_LOG(LogAICombatPattern, Warning, TEXT("시퀀서 어빌리티 취소: %s [%s] (실행시간: %.2f초)"),
        *Ability->GetName(), *CurrentActivatingUnitTag.ToString(), ExecutionTime);

    LogAbilityExecution(CurrentActivatingUnitTag, TEXT("취소됨"));

    // 통계 업데이트 (취소)
    UpdateExecutionStatistics(CurrentActivatingUnitTag, false, true, ExecutionTime);

    // 학습 시스템에 알림 (취소)
    FGameplayTag CancelledAbilityTag = CurrentActivatingUnitTag;
    NotifyLearningSystem(CancelledAbilityTag, false, true, ExecutionTime);

    // OnAbilityEnded와 동일한 후처리
    OnAbilityEnded(Ability);
}

void UAIAbilitySequencerComponent::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    // 데미지 추적이 활성화되지 않은 경우 무시
    if (!bIsTrackingDamage)
    {
        return;
    }

    float CurrentHealth = Data.NewValue;
    float HealthChange = CurrentHealth - PreviousHealthValue;

    UE_LOG(LogAICombatPattern, Log, TEXT("Health 변화 감지 (데미지 추적 중): %.2f → %.2f (변화량: %.2f)"),
        PreviousHealthValue, CurrentHealth, HealthChange);

    // Health 감소 = 데미지 받음
    if (HealthChange < 0.0f)
    {
        float DamageReceived = FMath::Abs(HealthChange);
        ExecutionStats.CurrentTurnDamageReceived += DamageReceived;

        UE_LOG(LogAICombatPattern, Log, TEXT("데미지 수신 추적: %.2f (턴 누적: %.2f)"),
            DamageReceived, ExecutionStats.CurrentTurnDamageReceived);
    }
    else if (HealthChange > 0.0f)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("힐링 수신: %.2f"), HealthChange);
    }

    PreviousHealthValue = CurrentHealth;
    ExecutionStats.TurnEndHealth = CurrentHealth;
}

void UAIAbilitySequencerComponent::OnDamageDealtByAI(AActor* SourceActor, AActor* TargetActor, const FGameplayEventData& EventData)
{
    if (!SourceActor || !TargetActor || !EventData.ContextHandle.Get() || !bIsTrackingDamage)
    {
        return;
    }

    // === Source와 Target의 ASC 가져오기 ===
    UBaseAbilitySystemComponent* SourceASC = Cast<UBaseAbilitySystemComponent>(
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));
    UBaseAbilitySystemComponent* TargetASC = Cast<UBaseAbilitySystemComponent>(
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));

    if (!SourceASC || !TargetASC)
    {
        UE_LOG(LogAICombatPattern, Warning,
            TEXT("데미지 계산 실패 - Source ASC: %s, Target ASC: %s"),
            SourceASC ? TEXT("유효") : TEXT("무효"),
            TargetASC ? TEXT("유효") : TEXT("무효"));
        return;
    }

    // === AttributeSet 가져오기 ===
    const UBaseAttributeSet* SourceAttributeSet = SourceASC->GetSet<UBaseAttributeSet>();
    const UBaseAttributeSet* TargetAttributeSet = TargetASC->GetSet<UBaseAttributeSet>();

    if (!SourceAttributeSet || !TargetAttributeSet)
    {
        UE_LOG(LogAICombatPattern, Warning,
            TEXT("데미지 계산 실패 - Source AttributeSet: %s, Target AttributeSet: %s"),
            SourceAttributeSet ? TEXT("유효") : TEXT("무효"),
            TargetAttributeSet ? TEXT("유효") : TEXT("무효"));
        return;
    }

    // === Attribute 값 가져오기 ===
    float SourceOffense = SourceASC->GetNumericAttribute(SourceAttributeSet->GetOffenseAttribute());
    float TargetDefense = TargetASC->GetNumericAttribute(TargetAttributeSet->GetDefenseAttribute());

    // === 실제 데미지 계산 (Offense - Defense, 최소 1) ===
    float ActualDamage = FMath::Max(1.0f, SourceOffense - TargetDefense);

    // === 통계에 데미지 전달량 추가 ===
    ExecutionStats.CurrentTurnDamageDealt += ActualDamage;

    UE_LOG(LogAICombatPattern, Log,
        TEXT("AI 데미지 전달 계산: Offense(%.2f) - Defense(%.2f) = %.2f (턴 누적: %.2f)"),
        SourceOffense, TargetDefense, ActualDamage, ExecutionStats.CurrentTurnDamageDealt);

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log,
            TEXT("데미지 전달 세부사항 - Source: %s(Offense: %.2f), Target: %s(Defense: %.2f), 실제 데미지: %.2f"),
            *SourceActor->GetName(), SourceOffense,
            *TargetActor->GetName(), TargetDefense,
            ActualDamage);

        // HitResult 정보도 로그에 포함
        if (EventData.ContextHandle.IsValid())
        {
            if (const FGameplayEffectContext* Context = EventData.ContextHandle.Get())
            {
                if (const FHitResult* HitResult = Context->GetHitResult())
                {
                    UE_LOG(LogAICombatPattern, Log,
                        TEXT("충돌 정보 - 위치: %s, 본: %s"),
                        *HitResult->ImpactPoint.ToString(),
                        *HitResult->BoneName.ToString());
                }
            }
        }
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UAIAbilitySequencerComponent::ValidateCurrentSequencerState() const
{
    // 기본 상태 검증
    if (CurrentBlockIndex < -2)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("시퀀서 상태 검증 실패: 잘못된 블록 인덱스 %d"), CurrentBlockIndex);
        return false;
    }

    if (CurrentUnitIndex < 0)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("시퀀서 상태 검증 실패: 잘못된 유닛 인덱스 %d"), CurrentUnitIndex);
        return false;
    }

    // 캐시된 전투 시퀀스 검증
    if (!ValidateCombatSequenceIntegrity())
    {
        return false;
    }

    // 상태별 추가 검증
    if (IsExecutingAbilityBlocks())
    {
        const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();
        if (CurrentBlockIndex >= AbilityBlocks.Num())
        {
            UE_LOG(LogAICombatPattern, Error, TEXT("시퀀서 상태 검증 실패: AbilityBlocks 인덱스 범위 초과 %d/%d"),
                CurrentBlockIndex, AbilityBlocks.Num());
            return false;
        }
    }

    return true;
}

bool UAIAbilitySequencerComponent::ValidateAbilityTag(const UGameplayAbility* Ability) const
{
    // 어빌리티나 현재 태그가 유효하지 않으면 실패
    if (!Ability || !CurrentActivatingUnitTag.IsValid())
    {
        return false;
    }

    // 어빌리티가 가진 태그들 중에 현재 활성화된 태그가 있는지 확인
    const FGameplayTagContainer& AbilityTags = Ability->AbilityTags;
    bool bIsValid = AbilityTags.HasTag(CurrentActivatingUnitTag);

    if (bEnableVerboseLogging && !bIsValid)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("어빌리티 태그 검증 실패: 현재태그[%s] vs 어빌리티태그[%s]"),
            *CurrentActivatingUnitTag.ToString(), *AbilityTags.ToString());
    }

    return bIsValid;
}

bool UAIAbilitySequencerComponent::ValidateCombatSequenceIntegrity() const
{
    // Start Block 검증
    const auto& StartBlock = CachedCombatSequence.GetStartBlock();
    if (StartBlock.GetAbilityBlockUnits().Num() == 0)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("전투 시퀀스 검증: Start Block에 유닛이 없음"));
    }

    // Ability Blocks 검증
    const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();
    if (AbilityBlocks.Num() == 0)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("전투 시퀀스 검증 실패: Ability Blocks가 없음"));
        return false;
    }

    for (int32 i = 0; i < AbilityBlocks.Num(); ++i)
    {
        const auto& Block = AbilityBlocks[i];
        if (Block.GetAbilityBlockUnits().Num() == 0)
        {
            UE_LOG(LogAICombatPattern, Warning, TEXT("전투 시퀀스 검증: AbilityBlock[%d]에 유닛이 없음"), i);
        }

        // 블록 식별자 검증
        if (!Block.GetBlockIdentifier().IsValid())
        {
            UE_LOG(LogAICombatPattern, Warning, TEXT("전투 시퀀스 검증: AbilityBlock[%d]의 식별자가 유효하지 않음"), i);
        }
    }

    // Exit Block 검증
    const auto& ExitBlock = CachedCombatSequence.GetExitBlock();
    if (ExitBlock.GetAbilityBlockUnits().Num() == 0)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("전투 시퀀스 검증: Exit Block에 유닛이 없음"));
    }

    return true;
}

const FAIAbilityBlockUnit* UAIAbilitySequencerComponent::GetCurrentExecutingUnit(FString& OutBlockType, int32& OutTotalUnits) const
{
    const FAIAbilityBlockUnit* CurrentUnit = nullptr;
    OutTotalUnits = 0;

    // Start Block
    if (IsExecutingStartBlock())
    {
        const auto& StartBlock = CachedCombatSequence.GetStartBlock();
        const auto& StartBlockUnits = StartBlock.GetAbilityBlockUnits();
        OutTotalUnits = StartBlockUnits.Num();

        if (CurrentUnitIndex < OutTotalUnits)
        {
            CurrentUnit = &StartBlockUnits[CurrentUnitIndex];
            OutBlockType = TEXT("StartBlock");
        }
    }
    // Ability Blocks 실행 중
    else if (IsExecutingAbilityBlocks())
    {
        const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();
        if (CurrentBlockIndex < AbilityBlocks.Num())
        {
            const auto& CurrentAbilityBlock = AbilityBlocks[CurrentBlockIndex];
            const auto& AbilityBlockUnits = CurrentAbilityBlock.GetAbilityBlockUnits();

            // 학습 모드에서는 계획된 실행 개수와 실제 유닛 개수 비교
            if (IsAdaptiveLearningEnabled())
            {
                int32 ActualUnitCount = AbilityBlockUnits.Num();
                OutTotalUnits = FMath::Min(PlannedExecutionCount, ActualUnitCount);

                // 실제 유닛 개수가 계획된 실행 개수보다 적은 경우 감지
                if (ActualUnitCount < PlannedExecutionCount)
                {
                    UE_LOG(LogAICombatPattern, Warning,
                        TEXT("유닛 부족 감지 - 계획: %d개, 실제: %d개, 블록: %s"),
                        PlannedExecutionCount, ActualUnitCount, *CurrentSelectedBlockTag.ToString());
                }

                // 현재 완료된 개수가 실제 유닛 개수에 도달했는지 확인 (유닛 부족으로 인한 조기 종료)
                if (CompletedExecutionCount >= ActualUnitCount)
                {
                    UE_LOG(LogAICombatPattern, Log,
                        TEXT("유닛 부족으로 인한 조기 턴 종료 - 완료: %d/%d, 실제유닛: %d, 블록: %s"),
                        CompletedExecutionCount, PlannedExecutionCount, ActualUnitCount, *CurrentSelectedBlockTag.ToString());

                    OutBlockType = FString::Printf(TEXT("AdaptiveBlock[%d:%s]-EarlyComplete"),
                        CurrentBlockIndex, *CurrentSelectedBlockTag.ToString());
                    return nullptr; // 유닛 부족으로 더 이상 실행 불가
                }

                // 목표 완료 개수에 도달한 경우 (정상 완료)
                if (CompletedExecutionCount >= PlannedExecutionCount)
                {
                    UE_LOG(LogAICombatPattern, Log,
                        TEXT("계획된 실행 개수(%d) 완료, 턴 종료"), PlannedExecutionCount);
                    OutBlockType = FString::Printf(TEXT("AdaptiveBlock[%d:%s]-Complete"),
                        CurrentBlockIndex, *CurrentSelectedBlockTag.ToString());
                    return nullptr;
                }

                // 우선순위 순서로 정렬된 유닛 리스트 생성
                TArray<const FAIAbilityBlockUnit*> SortedUnits;
                for (const auto& Unit : AbilityBlockUnits)
                {
                    SortedUnits.Add(&Unit);
                }

                SortedUnits.Sort([](const FAIAbilityBlockUnit& A, const FAIAbilityBlockUnit& B) {
                    return A.GetPriority() < B.GetPriority();
                    });

                // 완료된 개수를 기준으로 다음 실행할 유닛 선택
                if (CompletedExecutionCount < SortedUnits.Num())
                {
                    CurrentUnit = SortedUnits[CompletedExecutionCount];
                    OutBlockType = FString::Printf(TEXT("AdaptiveBlock[%d:%s]"),
                        CurrentBlockIndex, *CurrentSelectedBlockTag.ToString());

                    UE_LOG(LogAICombatPattern, Log,
                        TEXT("적응형 실행: %s에서 %d번째 유닛 선택 (우선순위: %d, 진행: %d/%d, 실제유닛: %d)"),
                        *CurrentSelectedBlockTag.ToString(), CompletedExecutionCount + 1,
                        CurrentUnit->GetPriority(), CompletedExecutionCount + 1,
                        PlannedExecutionCount, ActualUnitCount);
                }
            }
            else
            {
                // 기존 방식: 순차 실행
                OutTotalUnits = AbilityBlockUnits.Num();
                if (CurrentUnitIndex < OutTotalUnits)
                {
                    CurrentUnit = &AbilityBlockUnits[CurrentUnitIndex];
                    OutBlockType = FString::Printf(TEXT("AbilityBlock[%d]"), CurrentBlockIndex);
                }
            }
        }
    }
    // Exit Block (기존 로직 유지)
    else if (IsExecutingExitBlock())
    {
        const auto& ExitBlock = CachedCombatSequence.GetExitBlock();
        const auto& ExitBlockUnits = ExitBlock.GetAbilityBlockUnits();
        OutTotalUnits = ExitBlockUnits.Num();

        if (CurrentUnitIndex < OutTotalUnits)
        {
            CurrentUnit = &ExitBlockUnits[CurrentUnitIndex];
            OutBlockType = TEXT("ExitBlock");
        }
    }

    if (bEnableVerboseLogging)
    {
        if (IsAdaptiveLearningEnabled() && IsExecutingAbilityBlocks())
        {
            UE_LOG(LogAICombatPattern, Log,
                TEXT("현재 실행 유닛 조회: %s 계획실행:%d, 완료:%d, 실제유닛:%d, %s"),
                *OutBlockType, PlannedExecutionCount, CompletedExecutionCount, OutTotalUnits,
                CurrentUnit ? TEXT("유효함") : TEXT("유효하지 않음"));
        }
        else
        {
            UE_LOG(LogAICombatPattern, Log,
                TEXT("현재 실행 유닛 조회: %s Unit[%d/%d] %s"),
                *OutBlockType, CurrentUnitIndex, OutTotalUnits,
                CurrentUnit ? TEXT("유효함") : TEXT("유효하지 않음"));
        }
    }

    return CurrentUnit;
}

bool UAIAbilitySequencerComponent::ValidateExecutionRequest() const
{
    // 전투 준비 상태 체크
    if (!bCombatReady)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("전투 준비가 되지 않은 상태 - 현재 상태: %s"),
            *UEnum::GetValueAsString(GetCurrentExecutionState()));
        return false;
    }

    // 시퀀서 상태 검증
    if (!ValidateCurrentSequencerState())
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("시퀀서 상태 검증 실패"));
        return false;
    }

    return true;
}

void UAIAbilitySequencerComponent::UpdateExecutionStatistics(const FGameplayTag& AbilityTag, bool bSuccess, bool bCancelled, float ExecutionTime)
{
    ExecutionStats.TotalAbilitiesExecuted++;

    if (bSuccess)
    {
        ExecutionStats.SuccessfulAbilities++;
    }
    else if (bCancelled)
    {
        ExecutionStats.CancelledAbilities++;
    }
    else
    {
        ExecutionStats.FailedAbilities++;
    }

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("통계 업데이트: %s - 성공:%s, 취소:%s, 시간:%.2f초"),
            *AbilityTag.ToString(), bSuccess ? TEXT("Y") : TEXT("N"),
            bCancelled ? TEXT("Y") : TEXT("N"), ExecutionTime);

        UE_LOG(LogAICombatPattern, Log, TEXT("현재 통계 - 총실행:%d, 성공:%d(%.1f%%), 취소:%d(%.1f%%), 실패:%d"),
            ExecutionStats.TotalAbilitiesExecuted, ExecutionStats.SuccessfulAbilities,
            ExecutionStats.GetSuccessRate() * 100.0f, ExecutionStats.CancelledAbilities,
            ExecutionStats.GetCancelRate() * 100.0f, ExecutionStats.FailedAbilities);
    }
}

void UAIAbilitySequencerComponent::UpdatePartialCompletionStatistics(const FGameplayTag& BlockTag, int32 CompletedCount, int32 PlannedCount)
{
    UE_LOG(LogAICombatPattern, Log,
        TEXT("부분 완료 통계 업데이트 - 블록: %s, 완료: %d/%d (%.1f%%)"),
        *BlockTag.ToString(), CompletedCount, PlannedCount,
        (float)CompletedCount / (float)PlannedCount * 100.0f);

    // 부분 완료도 성공으로 간주하되, 완료율을 기록
    float CompletionRatio = PlannedCount > 0 ? (float)CompletedCount / (float)PlannedCount : 0.0f;

    // 학습 시스템에 부분 완료 정보 전달 (새로운 델리게이트 필요할 수 있음)
    if (bEnableLearningSystemIntegration && NotifyAbilityExecutionStats.IsBound())
    {
        // 부분 완료는 성공으로 처리하되, 실행 시간에 완료율 반영
        NotifyAbilityExecutionStats.Execute(BlockTag, true, false, CompletionRatio);

        UE_LOG(LogAICombatPattern, Log,
            TEXT("학습 시스템에 부분 완료 통계 전송 - 블록: %s, 완료율: %.2f"),
            *BlockTag.ToString(), CompletionRatio);
    }
}

void UAIAbilitySequencerComponent::NotifyLearningSystem(const FGameplayTag& AbilityTag, bool bSuccess, bool bCancelled, float ExecutionTime)
{
    if (!bEnableLearningSystemIntegration)
    {
        return;
    }

    if (NotifyAbilityExecutionStats.IsBound())
    {
        NotifyAbilityExecutionStats.Execute(AbilityTag, bSuccess, bCancelled, ExecutionTime);

        UE_LOG(LogAICombatPattern, Log, TEXT("학습 시스템에 통계 전송: %s - 성공:%s, 취소:%s, 시간:%.2f초"),
            *AbilityTag.ToString(), bSuccess ? TEXT("Y") : TEXT("N"),
            bCancelled ? TEXT("Y") : TEXT("N"), ExecutionTime);
    }
    else if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("학습 시스템 델리게이트가 바인딩되지 않음"));
    }
}

void UAIAbilitySequencerComponent::LogCurrentSequencerState() const
{
    ESequencerExecutionState CurrentState = GetCurrentExecutionState();

    UE_LOG(LogAICombatPattern, Log, TEXT("=== 시퀀서 현재 상태 ==="));
    UE_LOG(LogAICombatPattern, Log, TEXT("실행 상태: %s"), *UEnum::GetValueAsString(CurrentState));
    UE_LOG(LogAICombatPattern, Log, TEXT("블록 인덱스: %d, 유닛 인덱스: %d"), CurrentBlockIndex, CurrentUnitIndex);
    UE_LOG(LogAICombatPattern, Log, TEXT("전투 준비: %s, 최초 실행: %s, Exit 대기: %s"),
        bCombatReady ? TEXT("Y") : TEXT("N"),
        bIsFirstRun ? TEXT("Y") : TEXT("N"),
        bWaitingForExitBlock ? TEXT("Y") : TEXT("N"));
    UE_LOG(LogAICombatPattern, Log, TEXT("현재 실행 태그: %s"), *CurrentActivatingUnitTag.ToString());
    UE_LOG(LogAICombatPattern, Log, TEXT("========================"));
}

void UAIAbilitySequencerComponent::LogBlockTransition(const FString& FromBlock, const FString& ToBlock) const
{
    UE_LOG(LogAICombatPattern, Log, TEXT("블록 전환: %s → %s"), *FromBlock, *ToBlock);
}

void UAIAbilitySequencerComponent::LogAbilityExecution(const FGameplayTag& AbilityTag, const FString& Action) const
{
    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("어빌리티 실행: [%s] %s"), *AbilityTag.ToString(), *Action);
    }
}

bool UAIAbilitySequencerComponent::IsAdaptiveLearningEnabled() const
{
    return bEnableLearningSystemIntegration && CachedAdaptiveBehaviorConfig.bEnableAdaptiveLearning;
}

bool UAIAbilitySequencerComponent::ShouldExplore() const
{
    const float EPSILON = 0.05f; // 5% 탐험 확률
    bool bExplore = FMath::FRand() < EPSILON;

    if (bEnableVerboseLogging && bExplore)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("탐험 모드 활성화 (ε=%.2f%%)"), EPSILON * 100.0f);
    }

    return bExplore;
}

FString UAIAbilitySequencerComponent::GetCurrentBlockInfo() const
{
    ESequencerExecutionState CurrentState = GetCurrentExecutionState();

    switch (CurrentState)
    {
    case ESequencerExecutionState::Idle:
        return TEXT("대기 중");
    case ESequencerExecutionState::StartBlock:
        return FString::Printf(TEXT("Start Block (Unit %d/%d)"),
            CurrentUnitIndex, CachedCombatSequence.GetStartBlock().GetAbilityBlockUnits().Num());
    case ESequencerExecutionState::AbilityBlocks:
        return FString::Printf(TEXT("Ability Block %d (Unit %d/%d)"),
            CurrentBlockIndex, CurrentUnitIndex,
            CachedCombatSequence.GetAbilityBlocks().IsValidIndex(CurrentBlockIndex) ?
            CachedCombatSequence.GetAbilityBlocks()[CurrentBlockIndex].GetAbilityBlockUnits().Num() : 0);
    case ESequencerExecutionState::ExitBlock:
        return FString::Printf(TEXT("Exit Block (Unit %d/%d)"),
            CurrentUnitIndex, CachedCombatSequence.GetExitBlock().GetAbilityBlockUnits().Num());
    case ESequencerExecutionState::WaitingExit:
        return TEXT("Exit Block 대기 중");
    default:
        return TEXT("알 수 없는 상태");
    }
}

void UAIAbilitySequencerComponent::PrintSequencerDebugInfo() const
{
    UE_LOG(LogAICombatPattern, Warning, TEXT("=== AI Ability Sequencer 디버그 정보 ==="));

    // 기본 상태 정보
    LogCurrentSequencerState();

    // 통계 정보
    UE_LOG(LogAICombatPattern, Warning, TEXT("=== 실행 통계 ==="));
    UE_LOG(LogAICombatPattern, Warning, TEXT("총 어빌리티 실행: %d"), ExecutionStats.TotalAbilitiesExecuted);
    UE_LOG(LogAICombatPattern, Warning, TEXT("성공: %d (%.1f%%)"), ExecutionStats.SuccessfulAbilities, ExecutionStats.GetSuccessRate() * 100.0f);
    UE_LOG(LogAICombatPattern, Warning, TEXT("취소: %d (%.1f%%)"), ExecutionStats.CancelledAbilities, ExecutionStats.GetCancelRate() * 100.0f);
    UE_LOG(LogAICombatPattern, Warning, TEXT("실패: %d"), ExecutionStats.FailedAbilities);
    UE_LOG(LogAICombatPattern, Warning, TEXT("총 전투 사이클: %d"), ExecutionStats.TotalCombatCycles);
    UE_LOG(LogAICombatPattern, Warning, TEXT("평균 전투 시간: %.2f초"), ExecutionStats.GetAverageCombatTime());

    // 전투 시퀀스 구조 정보
    UE_LOG(LogAICombatPattern, Warning, TEXT("=== 전투 시퀀스 구조 ==="));
    UE_LOG(LogAICombatPattern, Warning, TEXT("Start Block 유닛 수: %d"), CachedCombatSequence.GetStartBlock().GetAbilityBlockUnits().Num());
    UE_LOG(LogAICombatPattern, Warning, TEXT("Ability Blocks 수: %d"), CachedCombatSequence.GetAbilityBlocks().Num());
    UE_LOG(LogAICombatPattern, Warning, TEXT("Exit Block 유닛 수: %d"), CachedCombatSequence.GetExitBlock().GetAbilityBlockUnits().Num());

    UE_LOG(LogAICombatPattern, Warning, TEXT("======================================"));
}

void UAIAbilitySequencerComponent::ResetExecutionStatistics()
{
    UE_LOG(LogAICombatPattern, Log, TEXT("실행 통계 초기화"));

    ExecutionStats = FSequencerStatistics();

    if (GetWorld())
    {
        ExecutionStats.CurrentCombatStartTime = GetWorld()->GetTimeSeconds();
    }
}

bool UAIAbilitySequencerComponent::ValidateSequencerConfiguration() const
{
    UE_LOG(LogAICombatPattern, Warning, TEXT("시퀀서 설정 검증 시작"));

    bool bIsValid = true;

    // 기본 바인딩 검증
    if (!RequestActivateAbilityBlockUnit.IsBound())
    {
        UE_LOG(LogAICombatPattern, Error, TEXT("검증 실패: RequestActivateAbilityBlockUnit 델리게이트가 바인딩되지 않음"));
        bIsValid = false;
    }

    if (!NotifyCombatPatternExitComplete.IsBound())
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("검증 경고: NotifyCombatPatternExitComplete 델리게이트가 바인딩되지 않음"));
    }

    // 전투 시퀀스 무결성 검증
    if (!ValidateCombatSequenceIntegrity())
    {
        bIsValid = false;
    }

    // 현재 상태 검증
    if (!ValidateCurrentSequencerState())
    {
        bIsValid = false;
    }

    UE_LOG(LogAICombatPattern, Warning, TEXT("시퀀서 설정 검증 완료: %s"), bIsValid ? TEXT("성공") : TEXT("실패"));

    return bIsValid;
}

void UAIAbilitySequencerComponent::ForceAdvanceToNextBlock()
{
    UE_LOG(LogAICombatPattern, Warning, TEXT("강제 다음 블록 이동 (디버깅용)"));

    CurrentActivatingUnitTag = FGameplayTag();
    AdvanceToNextBlock();
    SetCombatReady(true);

    LogCurrentSequencerState();
}

void UAIAbilitySequencerComponent::ForceStartExitBlock()
{
    UE_LOG(LogAICombatPattern, Warning, TEXT("강제 Exit Block 시작 (디버깅용)"));

    CurrentActivatingUnitTag = FGameplayTag();
    bWaitingForExitBlock = false;
    StartExitBlock();

    LogCurrentSequencerState();
}

void UAIAbilitySequencerComponent::CheckAndApplyOverdrivePrevention()
{
    bool bShouldApplyReset = false;
    FString ResetReason;

    // === 1. 정기적 회귀 (30회마다 무조건) ===
    if (RuntimeLearningState.LearningCount > 0 &&
        RuntimeLearningState.LearningCount % CachedAdaptiveBehaviorConfig.ExperienceWeightConfig.EarlyStopThreshold == 0)
    {
        bShouldApplyReset = true;
        ResetReason = FString::Printf(TEXT("정기적 회귀: %d회 달성"), RuntimeLearningState.LearningCount);
    }

    // === 2. 조기 회귀 (극단적 변화 감지) ===
    else if (RuntimeLearningState.LearningCount >= 10)
    {
        float MaxWeightDiff = FMath::Max3(
            FMath::Abs(RuntimeLearningState.OpeningSkillsWeight),
            FMath::Abs(RuntimeLearningState.SkillsWeight),
            FMath::Abs(RuntimeLearningState.StrafeOrDodgeWeight)
        );

        if (MaxWeightDiff > CachedAdaptiveBehaviorConfig.ExperienceWeightConfig.MaxWeightChange * 0.8f)
        {
            bShouldApplyReset = true;
            ResetReason = FString::Printf(TEXT("조기 회귀: 극단적 변화 감지 (%.3f)"), MaxWeightDiff);
        }
    }

    // === 3. 성과 하락 회귀 ===
    else if (ExecutionStats.GetOverallDamagePerformance() < -0.7f && RuntimeLearningState.LearningCount > 20)
    {
        bShouldApplyReset = true;
        ResetReason = TEXT("성과 하락 회귀");
    }

    if (bShouldApplyReset)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("%s"), *ResetReason);
        RuntimeLearningState.ApplyAdaptiveReset(CachedAdaptiveBehaviorConfig.ExperienceWeightConfig);
    }
}

void UAIAbilitySequencerComponent::LogDetailedLearningAnalysis(const FGameplayTag& BlockTag, float StandardReward, float DamagePerformance, float CombinedReward, float WeightChange) const
{
    if (!bEnableVerboseLogging)
    {
        return;
    }

    UE_LOG(LogAICombatPattern, Log, TEXT("=== 상세 학습 분석: %s ==="), *BlockTag.ToString());
    UE_LOG(LogAICombatPattern, Log, TEXT("표준 보상: %.3f (데미지: %.2f, 거리: 적절, 성공: %s)"),
        StandardReward, ExecutionStats.CurrentTurnDamageReceived,
        ExecutionStats.GetSuccessRate() > 0.7f ? TEXT("성공") : TEXT("실패"));
    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 성과: %.3f (효율성: %.3f, 생존: %.3f, 공격: %.3f)"),
        DamagePerformance, ExecutionStats.CalculateCurrentTurnDamageEfficiency(),
        ExecutionStats.CalculateCurrentTurnSurvivalPerformance(),
        ExecutionStats.CalculateCurrentTurnAttackPerformance());
    UE_LOG(LogAICombatPattern, Log, TEXT("종합 보상: %.3f (표준 70%% + 데미지 30%%)"), CombinedReward);
    UE_LOG(LogAICombatPattern, Log, TEXT("가중치 변화: %.4f"), WeightChange);

    // 현재 각 카테고리의 선호도 출력
    const FAISimplifiedPersonality& Personality = CachedAdaptiveBehaviorConfig.SimplifiedPersonality;
    float OpeningPref = RuntimeLearningState.GetCurrentActionPreference(
        FGameplayTag::RequestGameplayTag("AbilityBlock.OpeningSkills"), Personality);
    float SkillsPref = RuntimeLearningState.GetCurrentActionPreference(
        FGameplayTag::RequestGameplayTag("AbilityBlock.Skills"), Personality);
    float DodgePref = RuntimeLearningState.GetCurrentActionPreference(
        FGameplayTag::RequestGameplayTag("AbilityBlock.StrafeOrDodge"), Personality);

    UE_LOG(LogAICombatPattern, Log, TEXT("현재 선호도 - Opening: %.1f, Skills: %.1f, Dodge: %.1f"),
        OpeningPref, SkillsPref, DodgePref);

    // 학습 진행 상황
    float LearningProgress = FMath::Min(1.0f, RuntimeLearningState.LearningCount / 50.0f);
    UE_LOG(LogAICombatPattern, Log, TEXT("학습 진행도: %.1f%% (%d/50), 현재 학습률: %.3f"),
        LearningProgress * 100.0f, RuntimeLearningState.LearningCount, RuntimeLearningState.CurrentLearningRate);

    UE_LOG(LogAICombatPattern, Log, TEXT("=========================================="));
}

void UAIAbilitySequencerComponent::LogCurrentTurnDamagePerformance() const
{
    UE_LOG(LogAICombatPattern, Log, TEXT("=== 현재 턴 데미지 성과 상세 분석 ==="));
    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 전달: %.2f"), ExecutionStats.CurrentTurnDamageDealt);
    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 수신: %.2f"), ExecutionStats.CurrentTurnDamageReceived);
    UE_LOG(LogAICombatPattern, Log, TEXT("Health 변화: %.2f → %.2f (변화량: %.2f)"),
        ExecutionStats.TurnStartHealth, ExecutionStats.TurnEndHealth,
        ExecutionStats.TurnEndHealth - ExecutionStats.TurnStartHealth);
    UE_LOG(LogAICombatPattern, Log, TEXT("데미지 교환비: %.3f"),
        ExecutionStats.CurrentTurnDamageReceived > 0.0f ?
        ExecutionStats.CurrentTurnDamageDealt / ExecutionStats.CurrentTurnDamageReceived :
        (ExecutionStats.CurrentTurnDamageDealt > 0.0f ? 99.0f : 1.0f));
    UE_LOG(LogAICombatPattern, Log, TEXT("%s"), *ExecutionStats.GetDamageResultSummary());
    UE_LOG(LogAICombatPattern, Log, TEXT("=========================================="));
}

void UAIAbilitySequencerComponent::GetNormalizedDamageData(float& OutDamageReceived, float& OutDamageDealt, float& OutDamageScore) const
{
    ExecutionStats.GetNormalizedDamageDataForReward(OutDamageReceived, OutDamageDealt, OutDamageScore);

    if (bEnableVerboseLogging)
    {
        UE_LOG(LogAICombatPattern, Log, TEXT("정규화된 데미지 데이터 - 수신: %.3f, 전달: %.3f, 점수: %.3f"),
            OutDamageReceived, OutDamageDealt, OutDamageScore);
    }
}

float UAIAbilitySequencerComponent::GetCurrentTurnDamageEfficiency() const
{
    return ExecutionStats.CalculateCurrentTurnDamageEfficiency();
}

float UAIAbilitySequencerComponent::GetCurrentDistanceToTarget() const
{
    ABaseAIController* AIController = Cast<ABaseAIController>(GetOwner());
    if (!AIController)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("AI Controller를 찾을 수 없음"));
        return 300.0f; // 기본값
    }

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    AActor* Target = BlackboardComp ? Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor"))) : nullptr;

    if (!Target)
    {
        UE_LOG(LogAICombatPattern, Warning, TEXT("타겟을 찾을 수 없음"));
        return 300.0f; // 기본값
    }

    float CurrentDistance = FVector::Dist(GetOwner()->GetActorLocation(), Target->GetActorLocation());
    float MinAttackRange = AIController->GetMinAttackRange();
    float MaxAttackRange = AIController->GetMaxAttackRange();

    UE_LOG(LogAICombatPattern, Log, TEXT("거리 측정 - 현재: %.2f, 최소사정거리: %.2f, 최대사정거리: %.2f"),
        CurrentDistance, MinAttackRange, MaxAttackRange);

    return CurrentDistance;
}

#pragma endregion