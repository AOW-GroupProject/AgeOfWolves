#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AgeOfWolvesGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameMode, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
class AAOWGameStateBase;
#pragma endregion

UCLASS(minimalapi)
class AAgeOfWolvesGameMode : public AGameModeBase
{
    GENERATED_BODY()

    //@Defualt Setting
#pragma region Default Setting
public:
    AAgeOfWolvesGameMode();
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    // PlayerController 기반으로 변경된 public 함수
    void HandlePlayerDeath(APlayerController* PlayerController);

private:
    //@테스트용 Player Start 리스폰 처리 (통합 함수)
    bool TestRespawnToPlayerStart(APlayerController* PlayerController);

private:
    //@Game State에 리스폰 요청
    void NotifyRespawnCompleteViaGameState(APlayerController* PlayerController);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@죽음 규칙 결정
    bool DetermineDeathRules(APlayerController* PlayerController);
#pragma endregion
};