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
	//@내부 바인딩...
	InternalBindToPlayerState();

}

void AAOWGameState::InternalBindToPlayerState()
{
    //@Player State 목록
    for (auto PlayerState : PlayerArray)
    {
        if (auto PlayerStateBase = Cast<APlayerStateBase>(PlayerState))
        {
            PlayerStateBase->NotifyPlayerDeathEvent.BindUFunction(this, "OnPlayerCharacterDeathEvent");

            UE_LOGFMT(LogAOWGameState, Log, "PlayerState 죽음 이벤트 바인딩 완료");
        }
    }
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
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

