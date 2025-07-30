#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "AOWGameState.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameState, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
class ABasePlayerController;
struct FQuestDataInfo;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerRespawnCompleted, APlayerController*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, const FQuestDataInfo&)
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
	friend class AAgeOfWolvesGameMode;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	AAOWGameState();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@Game Mode에서 호출할 함수 - 리스폰 완료 알림 처리
	UFUNCTION()
	void NotifyPlayerRespawnCompleted(APlayerController* RespawnedPlayerController);

	//@Game Mode에서 호출할 함수 - 퀘스트 완료 알림 처리
	UFUNCTION()
	void NotifyPlayerQuestCompleted(const FQuestDataInfo& QuestData);
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@플레이어 리스폰 완료 이벤트
	FPlayerRespawnCompleted PlayerRespawnCompleted;

	//@퀘스트 완료 이벤트
	FOnQuestCompleted OnQuestCompleted;
	
#pragma endregion

//@Callbacks
#pragma region Callbacks

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
