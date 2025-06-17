#include "AOWGameState.h"
#include "Logging/StructuredLog.h"

#include "03_Player/PlayerStateBase.h"

DEFINE_LOG_CATEGORY(LogAOWGameState)

//@Defualt Setting
#pragma region Default Setting
AAOWGameState::AAOWGameState()
{}

void AAOWGameState::BeginPlay()
{

}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAOWGameState::ProcessPlayerRespawn(APlayerStateBase* DeadPlayerState)
{

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void AAOWGameState::OnPlayerCharacterDeathEvent(APlayerStateBase* DeadPlayerState)
{
    UE_LOGFMT(LogAOWGameState, Log, "플레이어 캐릭터 죽음 이벤트 처리 시작");

    //@비동기 리스폰 처리 시작
    AsyncTask(ENamedThreads::GameThread, [this, DeadPlayerState]()
        {
            //@지연 시간 후 리스폰 처리
            FTimerHandle RespawnTimer;
            GetWorldTimerManager().SetTimer(RespawnTimer, [this, DeadPlayerState]()
                {
                    ProcessPlayerRespawn(DeadPlayerState);
                }, RespawnDelay, false);

            UE_LOGFMT(LogAOWGameState, Log, "비동기 리스폰 타이머 시작: {0}초", RespawnDelay);
        });
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

