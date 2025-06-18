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

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
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
	// PlayerState에서 직접 호출할 수 있는 public 함수
	void HandlePlayerDeath(APlayerStateBase* DeadPlayerState);

private:
	//@테스트용 Player Start 리스폰 처리 (통합 함수)
	bool TestRespawnToPlayerStart(APlayerStateBase* DeadPlayerState);

private:
	//@Game State에 리스폰 요청
	void NotifyRespawnCompleteViaGameState(APlayerStateBase* DeadPlayerState);
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	//@죽음 규칙 결정
	bool DetermineDeathRules(APlayerStateBase* DeadPlayerState);
#pragma endregion
};