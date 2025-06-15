#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "AOWGameState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameState, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
class ABasePlayerController;
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

/**
 *	@AAOWGameState
 * 
 *	인 게임 모드에서 동작하는 게임 상태 클래스
 */
UCLASS()
class AGEOFWOLVES_API AAOWGameState : public AGameStateBase
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	AAOWGameState();

protected:
	virtual void BeginPlay() override;

protected:
	void InternalBindToPlayerState();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@호출되는 리스폰 요청
	UFUNCTION()
	void ProcessPlayerRespawn(APlayerStateBase* DeadPlayerState);

protected:
	// 리스폰 지연 시간
	UPROPERTY(EditDefaultsOnly, Category = "리스폰")
	float RespawnDelay = 2.0f;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
	// Player State에서 호출되는 죽음 이벤트 콜백
	UFUNCTION()
	void OnPlayerCharacterDeathEvent(APlayerStateBase* DeadPlayerState);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
