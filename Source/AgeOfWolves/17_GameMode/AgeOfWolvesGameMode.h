#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "AgeOfWolvesGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameMode, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
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
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	AAgeOfWolvesGameMode();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc

private:
	// 죽음 규칙 결정
	void DetermineDeathRules(APlayerStateBase* DeadPlayerState);

	// Game State에 리스폰 요청
	void RequestRespawnToGameState(APlayerStateBase* DeadPlayerState);
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};



