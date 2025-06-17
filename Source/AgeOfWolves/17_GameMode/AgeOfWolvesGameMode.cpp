#include "AgeOfWolvesGameMode.h"
#include "Logging/StructuredLog.h"
#include "UObject/ConstructorHelpers.h"

#include "01_Character/CharacterBase.h"
#include "03_Player/PlayerStateBase.h"

DEFINE_LOG_CATEGORY(LogAOWGameMode)

//@Defualt Setting
#pragma region Default Setting
AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAgeOfWolvesGameMode::HandlePlayerDeath(APlayerStateBase* DeadPlayerState)
{
    //@PS
    if (!IsValid(DeadPlayerState))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 죽음 처리 - 유효하지 않은 PlayerState입니다");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 죽음 처리 시작: {0}", GetNameSafe(DeadPlayerState));

    DetermineDeathRules(DeadPlayerState);
}

void AAgeOfWolvesGameMode::RequestRespawnToGameState(APlayerStateBase* DeadPlayerState)
{
    // TODO: Game State에 리스폰 요청 로직 구현

    UE_LOGFMT(LogAOWGameMode, Log, "GameState에 리스폰 요청: {0}",
        GetNameSafe(DeadPlayerState));
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void AAgeOfWolvesGameMode::DetermineDeathRules(APlayerStateBase* DeadPlayerState)
{
    UE_LOGFMT(LogAOWGameMode, Log, "죽음 규칙 결정 중: {0}",
        GetNameSafe(DeadPlayerState));

    //@리스폰 요청
    RequestRespawnToGameState(DeadPlayerState);
}
#pragma endregion