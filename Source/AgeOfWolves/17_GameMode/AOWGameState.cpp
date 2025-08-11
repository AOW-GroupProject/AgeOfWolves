#include "AOWGameState.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "GameFramework/PlayerController.h"
#include "00_GameInstance/AOWGameInstance.h"
#include "03_Player/PlayerStateBase.h"


DEFINE_LOG_CATEGORY(LogAOWGameState)

//@기본 설정
#pragma region Default Setting
AAOWGameState::AAOWGameState()
{
    // 싱글플레이어 게임이므로 리플리케이션 비활성화
    bReplicates = false;

    // 초기 상태 설정
    CurrentGameModeState = EGameModeState::Normal;
    GameStartTime = 0.0f;

    // 상태 이력 배열 초기화 (메모리 효율성을 위해 미리 공간 할당)
    StateChangeHistory.Reserve(50);

    UE_LOGFMT(LogAOWGameState, Log, "AOWGameState 생성 완료 - 싱글플레이어 모드, 초기 상태: Normal");
}

void AAOWGameState::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시간 기록 - 상대적 시간 계산의 기준점
    GameStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

    // 게임 시작을 상태 이력에 기록
    AddToStateHistory(TEXT("Game BeginPlay - 게임 시작"));

    UE_LOGFMT(LogAOWGameState, Log, "AOWGameState BeginPlay 완료 - 게임 시작 시간: {0}", GameStartTime);
}

void AAOWGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOGFMT(LogAOWGameState, Log, "AOWGameState EndPlay 시작 - 종료 이유: {0}",
        static_cast<int32>(EndPlayReason));

    // 정리 작업 - 모든 뮤텍스를 동시에 잠궈서 일관된 상태로 종료
    {
        FScopeLock StateLock(&GameModeStateMutex);
        FScopeLock RespawnLock(&RespawnInfoMutex);
        FScopeLock TransitionLock(&LevelTransitionMutex);

        // 모든 상태를 초기값으로 리셋
        CurrentGameModeState = EGameModeState::Normal;
        CurrentRespawnInfo.Reset();
        CurrentLevelTransitionInfo.Reset();

        AddToStateHistory(TEXT("Game EndPlay - 모든 상태 초기화"));
    }

    Super::EndPlay(EndPlayReason);
}
#pragma endregion

//@GameMode 전용 상태 업데이트 함수들
#pragma region GameMode State Update Functions
void AAOWGameState::SetGameModeState(EGameModeState NewState, const FString& Reason)
{
    FScopeLock Lock(&GameModeStateMutex);

    // 현재 상태와 동일한 경우 중복 처리 방지
    if (CurrentGameModeState == NewState)
    {
        UE_LOGFMT(LogAOWGameState, VeryVerbose, "상태 변경 요청이 현재와 동일함: {0}",
            static_cast<int32>(NewState));
        return;
    }

    // 상태 전환 유효성 검사
    if (!IsValidStateTransition(CurrentGameModeState, NewState))
    {
        UE_LOGFMT(LogAOWGameState, Warning, "유효하지 않은 상태 전환 시도: {0} -> {1}",
            static_cast<int32>(CurrentGameModeState), static_cast<int32>(NewState));
        return;
    }

    EGameModeState PreviousState = CurrentGameModeState;
    CurrentGameModeState = NewState;

    // 상태 변경을 이력에 기록
    FString PreviousStateName = PreviousState == EGameModeState::Normal ? TEXT("Normal") :
        PreviousState == EGameModeState::PlayerDeath ? TEXT("PlayerDeath") : TEXT("LevelTransition");
    FString NewStateName = NewState == EGameModeState::Normal ? TEXT("Normal") :
        NewState == EGameModeState::PlayerDeath ? TEXT("PlayerDeath") : TEXT("LevelTransition");

    FString HistoryEntry = TEXT("상태 변경: ") + PreviousStateName + TEXT(" -> ") + NewStateName + TEXT(" (이유: ") + Reason + TEXT(")");
    AddToStateHistory(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, Log, "게임 모드 상태 변경: {0} -> {1} (이유: {2})",
        static_cast<int32>(PreviousState), static_cast<int32>(NewState), *Reason);

    // 상태 변경 이벤트 브로드캐스트 - 다른 시스템들이 이를 감지할 수 있음
    GameModeStateChanged.Broadcast(PreviousState, NewState);
}

void AAOWGameState::SetDeathRespawnInfo(APlayerController* DeadPlayer, const FTransform& RespawnTransform)
{
    FScopeLock Lock(&RespawnInfoMutex);

    if (!IsValid(DeadPlayer))
    {
        UE_LOGFMT(LogAOWGameState, Warning, "유효하지 않은 PlayerController로 데스 리스폰 정보 설정 시도");
        return;
    }

    // 리스폰 정보 설정
    CurrentRespawnInfo.DeadPlayerController = DeadPlayer;
    CurrentRespawnInfo.RespawnTransform = RespawnTransform;
    CurrentRespawnInfo.bHasValidRespawnTransform = true;
    CurrentRespawnInfo.CreationTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    CurrentRespawnInfo.bIsDeathRespawn = true; // 죽음으로 인한 리스폰임을 명시

    FString HistoryEntry = TEXT("데스 리스폰 정보 설정: ") + GetNameSafe(DeadPlayer) + TEXT(" -> ") + RespawnTransform.GetLocation().ToString();
    AddToStateHistory(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, Log, "데스 리스폰 정보 설정 완료: {0} -> {1}",
        GetNameSafe(DeadPlayer), *RespawnTransform.GetLocation().ToString());

    // 리스폰 정보 업데이트 이벤트 브로드캐스트
    RespawnInfoUpdated.Broadcast(CurrentRespawnInfo);
}

void AAOWGameState::SetLevelTransitionRespawnInfo(APlayerController* Player, const FTransform& RespawnTransform)
{
    FScopeLock Lock(&RespawnInfoMutex);

    if (!IsValid(Player))
    {
        UE_LOGFMT(LogAOWGameState, Warning, "유효하지 않은 PlayerController로 레벨 전환 리스폰 정보 설정 시도");
        return;
    }

    // 리스폰 정보 설정 (레벨 전환용)
    CurrentRespawnInfo.DeadPlayerController = Player;
    CurrentRespawnInfo.RespawnTransform = RespawnTransform;
    CurrentRespawnInfo.bHasValidRespawnTransform = true;
    CurrentRespawnInfo.CreationTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    CurrentRespawnInfo.bIsDeathRespawn = false; // 레벨 전환으로 인한 리스폰임을 명시

    FString HistoryEntry = TEXT("레벨 전환 리스폰 정보 설정: ") + GetNameSafe(Player) + TEXT(" -> ") + RespawnTransform.GetLocation().ToString();
    AddToStateHistory(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 리스폰 정보 설정 완료: {0} -> {1}",
        GetNameSafe(Player), *RespawnTransform.GetLocation().ToString());

    RespawnInfoUpdated.Broadcast(CurrentRespawnInfo);
}

void AAOWGameState::SetLevelTransitionInfo(const FGameplayTag& NextLevelTag, APlayerController* InitiatingPlayer)
{
    FScopeLock Lock(&LevelTransitionMutex);

    if (!NextLevelTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameState, Warning, "유효하지 않은 레벨 태그로 레벨 전환 정보 설정 시도");
        return;
    }

    // 레벨 전환 정보 설정
    CurrentLevelTransitionInfo.NextLevelTag = NextLevelTag;
    CurrentLevelTransitionInfo.InitiatingPlayer = InitiatingPlayer;
    CurrentLevelTransitionInfo.TransitionStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    CurrentLevelTransitionInfo.bIsTransitionActive = true;

    FString HistoryEntry = TEXT("레벨 전환 정보 설정: 목적지 = ") + NextLevelTag.ToString();
    AddToStateHistory(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 정보 설정 완료: 목적지 = {0}, 시작자 = {1}",
        *NextLevelTag.ToString(), GetNameSafe(InitiatingPlayer));

    // 레벨 전환 정보 업데이트 이벤트 브로드캐스트
    LevelTransitionInfoUpdated.Broadcast(CurrentLevelTransitionInfo);
}

void AAOWGameState::CompletePlayerRespawn(APlayerController* RespawnedPlayer)
{
    if (!IsValid(RespawnedPlayer))
    {
        UE_LOGFMT(LogAOWGameState, Warning, "유효하지 않은 PlayerController로 리스폰 완료 처리 시도");
        return;
    }

    FString HistoryEntry = TEXT("플레이어 리스폰 완료: ") + GetNameSafe(RespawnedPlayer);
    AddToStateHistory(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, Log, "플레이어 리스폰 완료 이벤트 브로드캐스트: {0}",
        GetNameSafe(RespawnedPlayer));

    // 플레이어 리스폰 완료 이벤트 브로드캐스트
    PlayerRespawnCompleted.Broadcast(RespawnedPlayer);
}

void AAOWGameState::CompleteLevelTransition()
{
    FScopeLock Lock(&LevelTransitionMutex);

    FString CompletedLevelName = CurrentLevelTransitionInfo.NextLevelTag.ToString();

    // 레벨 전환 정보 초기화
    CurrentLevelTransitionInfo.Reset();

    FString HistoryEntry = TEXT("레벨 전환 완료: ") + CompletedLevelName;
    AddToStateHistory(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 완료: {0}", *CompletedLevelName);

    // 레벨 전환 정보 업데이트 이벤트 브로드캐스트 (빈 정보로 전환 완료를 알림)
    LevelTransitionInfoUpdated.Broadcast(CurrentLevelTransitionInfo);
}

void AAOWGameState::ResetToNormalState()
{
    UE_LOGFMT(LogAOWGameState, Log, "모든 상태를 Normal로 초기화 시작");

    // 모든 뮤텍스를 일관된 순서로 잠궈서 데드락 방지
    FScopeLock StateLock(&GameModeStateMutex);
    FScopeLock RespawnLock(&RespawnInfoMutex);
    FScopeLock TransitionLock(&LevelTransitionMutex);

    // 상태 초기화
    EGameModeState PreviousState = CurrentGameModeState;
    CurrentGameModeState = EGameModeState::Normal;

    // 모든 정보 초기화
    CurrentRespawnInfo.Reset();
    CurrentLevelTransitionInfo.Reset();

    AddToStateHistory(TEXT("모든 상태 Normal로 초기화"));

    UE_LOGFMT(LogAOWGameState, Log, "모든 상태 Normal로 초기화 완료");

    // 상태 변경 이벤트 브로드캐스트
    if (PreviousState != EGameModeState::Normal)
    {
        GameModeStateChanged.Broadcast(PreviousState, EGameModeState::Normal);
    }
}

void AAOWGameState::ClearRespawnInfo()
{
    FScopeLock Lock(&RespawnInfoMutex);

    if (CurrentRespawnInfo.IsValid())
    {
        CurrentRespawnInfo.Reset();
        AddToStateHistory(TEXT("리스폰 정보 초기화"));

        UE_LOGFMT(LogAOWGameState, Log, "리스폰 정보 초기화 완료");

        // 초기화된 리스폰 정보로 이벤트 브로드캐스트
        RespawnInfoUpdated.Broadcast(CurrentRespawnInfo);
    }
}

void AAOWGameState::ClearLevelTransitionInfo()
{
    FScopeLock Lock(&LevelTransitionMutex);

    if (CurrentLevelTransitionInfo.IsValid())
    {
        CurrentLevelTransitionInfo.Reset();
        AddToStateHistory(TEXT("레벨 전환 정보 초기화"));

        UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 정보 초기화 완료");

        // 초기화된 레벨 전환 정보로 이벤트 브로드캐스트
        LevelTransitionInfoUpdated.Broadcast(CurrentLevelTransitionInfo);
    }
}

void AAOWGameState::NotifyPlayerQuestCompleted(const FQuestDataInfo& QuestData)
{
    OnQuestCompleted.Broadcast(QuestData);
    
    const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOGFMT(LogAOWGameState, Warning, "GameInstance가 유효하지 않음");
        return;
    }

    GameInstance->UpdateQuestProgress(QuestData);
    UE_LOGFMT(LogAOWGameState, Log, "퀘스트 완료 이벤트 브로드캐스트 완료");
}
#pragma endregion

//@내부 유틸리티 함수들
#pragma region Internal Utility Functions
void AAOWGameState::AddToStateHistory(const FString& StateChangeDescription)
{
    // 상태 이력 크기 제한 (메모리 사용량 관리)
    const int32 MaxHistorySize = 100;

    if (StateChangeHistory.Num() >= MaxHistorySize)
    {
        // 가장 오래된 항목 제거 (FIFO 방식)
        StateChangeHistory.RemoveAt(0);
    }

    // 타임스탬프와 함께 이력 추가
    float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    FString HistoryEntry = FString::Printf(TEXT("[%.2f] %s"), CurrentTime, *StateChangeDescription);

    StateChangeHistory.Add(HistoryEntry);

    UE_LOGFMT(LogAOWGameState, VeryVerbose, "상태 이력 추가: {0}", *HistoryEntry);
}

bool AAOWGameState::IsValidStateTransition(EGameModeState FromState, EGameModeState ToState) const
{
    // 싱글플레이어 게임의 상태 전환 규칙
    // 실제 게임 로직에 맞게 더 정교한 규칙을 추가할 수 있음

    switch (FromState)
    {
    case EGameModeState::Normal:
        // Normal 상태에서는 PlayerDeath나 LevelTransition으로 전환 가능
        return (ToState == EGameModeState::PlayerDeath ||
            ToState == EGameModeState::LevelTransition);

    case EGameModeState::PlayerDeath:
        // PlayerDeath 상태에서는 Normal로만 전환 가능 (리스폰 완료 후)
        return (ToState == EGameModeState::Normal);

    case EGameModeState::LevelTransition:
        // LevelTransition 상태에서는 Normal로만 전환 가능 (전환 완료 후)
        return (ToState == EGameModeState::Normal);

    default:
        return false;
    }
}
#pragma endregion

//@상태 조회 함수들
#pragma region State Query Functions
EGameModeState AAOWGameState::GetCurrentGameModeState() const
{
    FScopeLock Lock(&GameModeStateMutex);
    return CurrentGameModeState;
}

bool AAOWGameState::IsInState(EGameModeState StateToCheck) const
{
    return GetCurrentGameModeState() == StateToCheck;
}

const FRespawnInfo& AAOWGameState::GetCurrentRespawnInfo() const
{
    FScopeLock Lock(&RespawnInfoMutex);
    return CurrentRespawnInfo;
}

bool AAOWGameState::HasValidRespawnInfo() const
{
    FScopeLock Lock(&RespawnInfoMutex);
    return CurrentRespawnInfo.IsValid();
}

APlayerController* AAOWGameState::GetCurrentDeadPlayer() const
{
    FScopeLock Lock(&RespawnInfoMutex);
    return CurrentRespawnInfo.DeadPlayerController.Get();
}

const FLevelTransitionInfo& AAOWGameState::GetCurrentLevelTransitionInfo() const
{
    FScopeLock Lock(&LevelTransitionMutex);
    return CurrentLevelTransitionInfo;
}

bool AAOWGameState::IsLevelTransitionInProgress() const
{
    FScopeLock Lock(&LevelTransitionMutex);
    return CurrentLevelTransitionInfo.IsValid();
}

FGameplayTag AAOWGameState::GetNextLevelTag() const
{
    FScopeLock Lock(&LevelTransitionMutex);
    return CurrentLevelTransitionInfo.NextLevelTag;
}
#pragma endregion

//@디버그 함수들
#pragma region Debug Functions
void AAOWGameState::PrintGameStateDebugInfo() const
{
    UE_LOGFMT(LogAOWGameState, Log, "=== GameState Debug Information ===");

    // 기본 상태 정보
    EGameModeState CurrentState = GetCurrentGameModeState();
    FString StateString = CurrentState == EGameModeState::Normal ? TEXT("Normal") :
        CurrentState == EGameModeState::PlayerDeath ? TEXT("PlayerDeath") :
        TEXT("LevelTransition");

    UE_LOGFMT(LogAOWGameState, Log, "Current Game Mode State: {0}", *StateString);
    UE_LOGFMT(LogAOWGameState, Log, "Game Duration: {0} seconds", GetGameDuration());

    // 리스폰 정보
    const FRespawnInfo& RespawnInfo = GetCurrentRespawnInfo();
    if (RespawnInfo.IsValid())
    {
        UE_LOGFMT(LogAOWGameState, Log, "Respawn Info: {0}", *RespawnInfo.ToString());
    }
    else
    {
        UE_LOGFMT(LogAOWGameState, Log, "Respawn Info: Not Available");
    }

    // 레벨 전환 정보
    const FLevelTransitionInfo& TransitionInfo = GetCurrentLevelTransitionInfo();
    if (TransitionInfo.IsValid())
    {
        UE_LOGFMT(LogAOWGameState, Log, "Level Transition Info: {0}", *TransitionInfo.ToString());
    }
    else
    {
        UE_LOGFMT(LogAOWGameState, Log, "Level Transition Info: Not Active");
    }

    UE_LOGFMT(LogAOWGameState, Log, "=== End Debug Information ===");
}

void AAOWGameState::PrintStateChangeHistory() const
{
    UE_LOGFMT(LogAOWGameState, Log, "=== State Change History ({0} entries) ===",
        StateChangeHistory.Num());

    for (int32 i = 0; i < StateChangeHistory.Num(); ++i)
    {
        UE_LOGFMT(LogAOWGameState, Log, "[{0}] {1}", i, *StateChangeHistory[i]);
    }

    UE_LOGFMT(LogAOWGameState, Log, "=== End State History ===");
}
#pragma endregion