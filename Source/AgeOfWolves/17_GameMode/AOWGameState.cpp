#include "AOWGameState.h"
#include "Logging/StructuredLog.h"

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
void AAOWGameState::NotifyRequestShowLoadingUI()
{
    //@Loading UI 그리기 요청 이벤트
    RequestShowLoadingUI.ExecuteIfBound();

    UE_LOGFMT(LogAOWGameState, Log, "로딩 UI 표시 요청 처리 완료");
}

void AAOWGameState::NotifyRequestStartLevelTransition(const FGameplayTag& NextLevelTag)
{
    UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 요청 받음: {0}", *NextLevelTag.ToString());

    //@다음 레벨 태그 유효성 검사
    if (!NextLevelTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameState, Error, "레벨 전환 실패: 유효하지 않은 레벨 태그입니다");
        return;
    }

    UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 유효성 검사 통과 - 목적지: {0}",
        *NextLevelTag.ToString());

    //@레벨 전환 시작 이벤트 브로드캐스트
    RequestStartLevelTransition.ExecuteIfBound(NextLevelTag);

    UE_LOGFMT(LogAOWGameState, Log, "레벨 전환 시작 이벤트 브로드캐스트 완료: {0}", *NextLevelTag.ToString());

}

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
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

