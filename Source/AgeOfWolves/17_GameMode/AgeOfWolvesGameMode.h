#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "AgeOfWolvesGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameMode, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
class AAOWGameStateBase;

struct FStructureData;
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

//@Default Setting
#pragma region Default Setting
public:
    AAgeOfWolvesGameMode();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    // PlayerController 기반으로 변경된 public 함수
    void HandlePlayerDeath(APlayerController* PlayerController);

    // 실제 리스폰을 처리하는 함수 (UI나 다른 시스템에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Respawn")
    void ExecutePlayerRespawn(APlayerController* PlayerController);

private:
    //@테스트용 Player Start 리스폰 처리 (통합 함수)
    bool TestRespawnToPlayerStart(APlayerController* PlayerController);

private:
    //@Game State에 리스폰 요청
    void NotifyRespawnCompleteViaGameState(APlayerController* PlayerController);

    //@구조물(늑대 상) 상호작용시 발생
public:
    void OnStructureInteractionActtivated(const FStructureData& StructureData);
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@죽음 규칙 결정
    bool DetermineDeathRules(APlayerController* PlayerController);
#pragma endregion
};