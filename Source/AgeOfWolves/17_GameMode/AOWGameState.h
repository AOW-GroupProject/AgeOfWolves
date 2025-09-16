#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayTagContainer.h"
#include "Engine/Engine.h"

#include "AOWGameState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameState, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
struct FQuestDataInfo;
class APlayerController;
#pragma endregion

//@열거형
#pragma region Enums
/**
 * 게임의 현재 상태를 나타내는 열거형
 * 기존 GameMode에서 사용하던 EGameModeState를 GameState로 이동
 * 싱글플레이어 게임의 주요 상태들을 명확히 구분
 */
UENUM(BlueprintType)
enum class EGameModeState : uint8
{
    // 일반적인 게임플레이 상태 - 플레이어가 자유롭게 조작 가능
    Normal          UMETA(DisplayName = "Normal"),

    // 플레이어 사망 처리 중 - 리스폰 과정이 진행되는 동안
    PlayerDeath     UMETA(DisplayName = "Player Death"),

    // 레벨 전환 처리 중 - 새로운 레벨로 이동하는 과정
    LevelTransition UMETA(DisplayName = "Level Transition")
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 * 리스폰과 관련된 모든 정보를 담는 구조체
 * 기존 GameMode에서 개별 변수로 관리하던 것들을 체계적으로 정리
 * 스레드 안전성과 데이터 일관성을 보장하기 위해 구조화
 */
USTRUCT(BlueprintType)
struct FRespawnInfo
{
    GENERATED_BODY()

    // 사망한 플레이어의 컨트롤러 (기존 CachedDeadPlayerController)
    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<APlayerController> DeadPlayerController;

    // 미리 계산된 리스폰 위치 (기존 CachedRespawnTransform)
    UPROPERTY(BlueprintReadOnly)
    FTransform RespawnTransform;

    // 리스폰 위치가 유효한지 여부 (기존 bHasValidRespawnTransform)
    UPROPERTY(BlueprintReadOnly)
    bool bHasValidRespawnTransform = false;

    // 리스폰 정보가 생성된 시간 (디버깅 및 타이밍 분석용)
    UPROPERTY(BlueprintReadOnly)
    float CreationTime = 0.0f;

    // 리스폰 타입 구분 (죽음으로 인한 리스폰 vs 레벨 전환으로 인한 리스폰)
    UPROPERTY(BlueprintReadOnly)
    bool bIsDeathRespawn = true;

    FRespawnInfo()
    {
        DeadPlayerController = nullptr;
        RespawnTransform = FTransform::Identity;
        bHasValidRespawnTransform = false;
        CreationTime = 0.0f;
        bIsDeathRespawn = true;
    }

    // 리스폰 정보가 유효한지 확인하는 헬퍼 함수
    bool IsValid() const
    {
        return DeadPlayerController.IsValid() && bHasValidRespawnTransform;
    }

    // 리스폰 정보를 초기화하는 함수
    void Reset()
    {
        DeadPlayerController.Reset();
        RespawnTransform = FTransform::Identity;
        bHasValidRespawnTransform = false;
        CreationTime = 0.0f;
        bIsDeathRespawn = true;
    }

    // 디버그 정보를 문자열로 반환
    FString ToString() const
    {
        return FString::Printf(TEXT("DeadPlayer: %s, Position: %s, Valid: %s, Type: %s"),
            DeadPlayerController.IsValid() ? *DeadPlayerController->GetName() : TEXT("None"),
            *RespawnTransform.GetLocation().ToString(),
            bHasValidRespawnTransform ? TEXT("Yes") : TEXT("No"),
            bIsDeathRespawn ? TEXT("Death") : TEXT("Level Transition"));
    }
};

/**
 * 레벨 전환과 관련된 정보를 담는 구조체
 * 복잡한 레벨 전환 과정에서 필요한 모든 데이터를 체계적으로 관리
 */
USTRUCT(BlueprintType)
struct FLevelTransitionInfo
{
    GENERATED_BODY()

    // 다음에 이동할 레벨의 태그 (기존 CachedNextLevelTag)
    UPROPERTY(BlueprintReadOnly)
    FGameplayTag NextLevelTag;

    // 레벨 전환을 시작한 시간
    UPROPERTY(BlueprintReadOnly)
    float TransitionStartTime = 0.0f;

    // 레벨 전환을 트리거한 플레이어
    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<APlayerController> InitiatingPlayer;

    // 레벨 전환이 활성 상태인지 여부
    UPROPERTY(BlueprintReadOnly)
    bool bIsTransitionActive = false;

    FLevelTransitionInfo()
    {
        NextLevelTag = FGameplayTag();
        TransitionStartTime = 0.0f;
        InitiatingPlayer = nullptr;
        bIsTransitionActive = false;
    }

    // 레벨 전환 정보가 유효한지 확인
    bool IsValid() const
    {
        return NextLevelTag.IsValid() && bIsTransitionActive;
    }

    // 레벨 전환 정보를 초기화
    void Reset()
    {
        NextLevelTag = FGameplayTag();
        TransitionStartTime = 0.0f;
        InitiatingPlayer.Reset();
        bIsTransitionActive = false;
    }

    // 디버그 정보를 문자열로 반환
    FString ToString() const
    {
        return FString::Printf(TEXT("NextLevel: %s, Active: %s, StartTime: %.2f"),
            *NextLevelTag.ToString(),
            bIsTransitionActive ? TEXT("Yes") : TEXT("No"),
            TransitionStartTime);
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, const FQuestDataInfo&)
 // 게임 모드 상태가 변경될 때 호출되는 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FGameModeStateChanged, EGameModeState /* PreviousState */, EGameModeState /* NewState */)

// 리스폰 정보가 업데이트될 때 호출되는 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FRespawnInfoUpdated, const FRespawnInfo& /* RespawnInfo */)

// 레벨 전환 정보가 업데이트될 때 호출되는 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FLevelTransitionInfoUpdated, const FLevelTransitionInfo& /* TransitionInfo */)

// 플레이어 리스폰이 완료되었을 때 호출되는 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerRespawnCompleted, APlayerController* /* RespawnedPlayer */)
#pragma endregion

/**
 * AOWGameState - 싱글플레이어 최적화된 게임 상태 관리 클래스
 *
 * 주요 설계 철학:
 * 1. GameMode가 모든 상태를 직접 업데이트함 (GameMode 주도권)
 * 2. GameState는 순수한 데이터 저장소 역할 (수동적 역할)
 * 3. 상태 변경 시 이벤트를 통해 다른 시스템들에게 알림
 * 4. 싱글플레이어에 최적화 (네트워크 복제 없음)
 *
 * 기존 GameMode에서 이동된 상태 정보들:
 * - EGameModeState CurrentState -> CurrentGameModeState
 * - CachedDeadPlayerController -> RespawnInfo.DeadPlayerController
 * - CachedRespawnTransform -> RespawnInfo.RespawnTransform
 * - bHasValidRespawnTransform -> RespawnInfo.bHasValidRespawnTransform
 * - CachedNextLevelTag -> LevelTransitionInfo.NextLevelTag
 */
UCLASS()
class AGEOFWOLVES_API AAOWGameState : public AGameStateBase
{
    //@친구 클래스 - GameMode만 상태를 직접 수정할 수 있도록 제한
#pragma region Friend Class
    friend class AAgeOfWolvesGameMode;
#pragma endregion

    GENERATED_BODY()

//@기본 설정
#pragma region Default Setting
public:
    AAOWGameState();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion

//@속성 및 상태 정보
#pragma region Property or State Information
private:
    /**
     * 스레드 안전성을 위한 뮤텍스들
     * 싱글플레이어 게임이지만 AsyncTask 등에서 백그라운드 스레드 사용 가능성을 고려
     */
    mutable FCriticalSection GameModeStateMutex;
    mutable FCriticalSection RespawnInfoMutex;
    mutable FCriticalSection LevelTransitionMutex;

protected:
    //@Game Mode에서 호출할 함수 - 퀘스트 완료 알림 처리
    UFUNCTION()
    void NotifyPlayerQuestCompleted(FGameplayTag AreaTag, const FQuestDataInfo& QuestData);

protected:
    /**
     * 핵심 게임 상태 정보들 - GameMode에서만 수정 가능
     */

     // 현재 게임 모드 상태 (기존 GameMode의 CurrentState)
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    EGameModeState CurrentGameModeState = EGameModeState::Normal;

    // 리스폰 관련 정보 (기존 GameMode의 여러 캐시 변수들을 통합)
    UPROPERTY(BlueprintReadOnly, Category = "Respawn Management")
    FRespawnInfo CurrentRespawnInfo;

    // 레벨 전환 관련 정보 (기존 GameMode의 CachedNextLevelTag 확장)
    UPROPERTY(BlueprintReadOnly, Category = "Level Transition")
    FLevelTransitionInfo CurrentLevelTransitionInfo;

    // 게임 시작 시간 (상대적 시간 계산용)
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    float GameStartTime = 0.0f;

    // 상태 변경 이력 (디버깅용) - 최근 상태 변경들을 추적
    UPROPERTY(BlueprintReadOnly, Category = "Debug")
    TArray<FString> StateChangeHistory;

public:
    /**
     * GameMode 전용 상태 업데이트 함수들
     * 이 함수들은 friend class인 GameMode에서만 호출 가능
     * 각 함수는 상태를 업데이트하고 적절한 이벤트를 브로드캐스트함
     */

     // 게임 모드 상태 변경 (기존 GameMode의 CurrentState 설정 로직)
    void SetGameModeState(EGameModeState NewState, const FString& Reason = TEXT(""));

    // 리스폰 정보 설정 (죽음으로 인한 리스폰)
    void SetDeathRespawnInfo(APlayerController* DeadPlayer, const FTransform& RespawnTransform);

    // 리스폰 정보 설정 (레벨 전환으로 인한 리스폰)
    void SetLevelTransitionRespawnInfo(APlayerController* Player, const FTransform& RespawnTransform);

    // 레벨 전환 정보 설정
    void SetLevelTransitionInfo(const FGameplayTag& NextLevelTag, APlayerController* InitiatingPlayer = nullptr);

    // 리스폰 완료 처리 (GameMode에서 리스폰 작업 완료 후 호출)
    void CompletePlayerRespawn(APlayerController* RespawnedPlayer);

    // 레벨 전환 완료 처리
    void CompleteLevelTransition();

    // 모든 상태를 정상 상태로 초기화
    void ResetToNormalState();

    // 리스폰 정보만 초기화 (상태는 유지)
    void ClearRespawnInfo();

    // 레벨 전환 정보만 초기화 (상태는 유지)
    void ClearLevelTransitionInfo();

private:
    /**
     * 내부 유틸리티 함수들
     */

     // 상태 변경 이력에 추가 (디버깅 목적)
    void AddToStateHistory(const FString& StateChangeDescription);

    // 상태 전환이 유효한지 검사
    bool IsValidStateTransition(EGameModeState FromState, EGameModeState ToState) const;
#pragma endregion

//@이벤트 델리게이트
#pragma region Event Delegates
public:
    /**
     * 상태 변경 알림 이벤트들
     * 다른 시스템들(UI, Audio, 기타 매니저들)이 구독하여 상태 변경에 반응할 수 있음
     */

     // 게임 모드 상태 변경 이벤트
    FGameModeStateChanged GameModeStateChanged;

    // 리스폰 정보 업데이트 이벤트
    FRespawnInfoUpdated RespawnInfoUpdated;

    // 레벨 전환 정보 업데이트 이벤트
    FLevelTransitionInfoUpdated LevelTransitionInfoUpdated;

    // 플레이어 리스폰 완료 이벤트
    FPlayerRespawnCompleted PlayerRespawnCompleted;

    //@퀘스트 완료 이벤트
    FOnQuestCompleted OnQuestCompleted;
#pragma endregion

//@상태 조회 함수들 (읽기 전용)
#pragma region State Query Functions
public:
    /**
     * 외부에서 현재 상태를 조회할 수 있는 함수들
     * 모든 함수는 const로 선언하여 상태 변경을 방지
     * 스레드 안전성을 위해 뮤텍스 사용
     */

     // 현재 게임 모드 상태 조회
    UFUNCTION(BlueprintPure, Category = "Game State")
    EGameModeState GetCurrentGameModeState() const;

    // 특정 상태인지 확인
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool IsInState(EGameModeState StateToCheck) const;

    // 정상 상태인지 확인
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool IsInNormalState() const { return GetCurrentGameModeState() == EGameModeState::Normal; }

    // 플레이어 사망 상태인지 확인
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool IsInPlayerDeathState() const { return GetCurrentGameModeState() == EGameModeState::PlayerDeath; }

    // 레벨 전환 상태인지 확인
    UFUNCTION(BlueprintPure, Category = "Game State")
    bool IsInLevelTransitionState() const { return GetCurrentGameModeState() == EGameModeState::LevelTransition; }

    // 현재 리스폰 정보 조회
    UFUNCTION(BlueprintPure, Category = "Respawn Management")
    const FRespawnInfo& GetCurrentRespawnInfo() const;

    // 리스폰 정보가 유효한지 확인
    UFUNCTION(BlueprintPure, Category = "Respawn Management")
    bool HasValidRespawnInfo() const;

    // 현재 사망한 플레이어 조회
    UFUNCTION(BlueprintPure, Category = "Respawn Management")
    APlayerController* GetCurrentDeadPlayer() const;

    // 현재 레벨 전환 정보 조회
    UFUNCTION(BlueprintPure, Category = "Level Transition")
    const FLevelTransitionInfo& GetCurrentLevelTransitionInfo() const;

    // 레벨 전환이 진행 중인지 확인
    UFUNCTION(BlueprintPure, Category = "Level Transition")
    bool IsLevelTransitionInProgress() const;

    // 다음 레벨 태그 조회
    UFUNCTION(BlueprintPure, Category = "Level Transition")
    FGameplayTag GetNextLevelTag() const;

    // 게임 진행 시간 조회
    UFUNCTION(BlueprintPure, Category = "Game State")
    float GetGameDuration() const { return GetWorld() ? GetWorld()->GetTimeSeconds() - GameStartTime : 0.0f; }

    // 디버그 정보 출력
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void PrintGameStateDebugInfo() const;

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void PrintStateChangeHistory() const;
#pragma endregion
};