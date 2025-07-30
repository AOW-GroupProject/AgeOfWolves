#include "AOWGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "03_Player/PlayerStateBase.h"


DEFINE_LOG_CATEGORY(LogAOWGameState)

//@Defualt Setting
#pragma region Default Setting
AAOWGameState::AAOWGameState()
{
    //@Game State는 모든 클라이언트에 복제되어야 하므로 Replication 설정
    bReplicates = true;

    UE_LOGFMT(LogAOWGameState, Log, "Game State 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAOWGameState::NotifyPlayerRespawnCompleted(APlayerController* RespawnedPlayerController)
{
    //@기본 유효성 검증
    if (!IsValid(RespawnedPlayerController))
    {
        UE_LOGFMT(LogAOWGameState, Warning, "리스폰 완료 알림 실패: 유효하지 않은 PlayerController");
        return;
    }

    UE_LOGFMT(LogAOWGameState, Log, "플레이어 리스폰 완료 이벤트 브로드캐스트 시작: {0}",
        GetNameSafe(RespawnedPlayerController));

    PlayerRespawnCompleted.Broadcast(RespawnedPlayerController);

    UE_LOGFMT(LogAOWGameState, Log, "플레이어 리스폰 완료 이벤트 브로드캐스트 완료");
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

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

