#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameplayTagContainer.h"

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
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerRespawnCompleted, APlayerController*)

DECLARE_DELEGATE(FRequestShowLoadingUI)

DECLARE_DELEGATE(FRequestHideLoadingUI)

//@레벨 전환 요청 이벤트
DECLARE_DELEGATE_OneParam(FRequestStartLevelTransition, const FGameplayTag&)
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
	friend class UUIManagerSubsystem;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	AAOWGameState();

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩
	
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@Game Mode에서 호출할 함수 - 로딩 UI 표시 요청 처리
	UFUNCTION()
	void NotifyRequestShowLoadingUI();
	UFUNCTION()
	void NotifyRequestHideLoadingUI();

protected:
	//@레벨 전환 시작을 요청하는 함수
	void NotifyRequestStartLevelTransition(const FGameplayTag& NextLevelTag);

protected:
	//@Game Mode에서 호출할 함수 - 리스폰 완료 알림 처리
	UFUNCTION()
	void NotifyPlayerRespawnCompleted(APlayerController* RespawnedPlayerController);
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@Loading UI 렌더 요청 이벤트
	FRequestShowLoadingUI RequestShowLoadingUI;
	//@Loading UI 숨기기 요청 이벤트
	FRequestHideLoadingUI RequestHideLoadingUI;

public:
	//@레벨 전환 시작 요청 이벤트
	FRequestStartLevelTransition RequestStartLevelTransition;

public:
	//@플레이어 리스폰 완료 이벤트
	FPlayerRespawnCompleted PlayerRespawnCompleted;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
