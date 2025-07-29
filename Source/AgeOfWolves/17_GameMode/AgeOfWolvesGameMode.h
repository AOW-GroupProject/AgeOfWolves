#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"

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
//@상태 구분을 위한 열거형
UENUM()
enum class EGameModeState : uint8
{
    Normal,
    PlayerDeath,
    LevelTransition
};
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

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩...
    void ExternalBindingToUIManager();
    void ExternalBindingToLevelManager();

protected:
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    //@플레이어 시작 위치 선택 (레벨별 커스터마이징)
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
    virtual void BeginPlay() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    mutable FCriticalSection RespawnDataMutex;

    UPROPERTY()
    FGameplayTag CachedNextLevelTag;

    //@현재 상태 및 죽은 플레이어 정보
    EGameModeState CurrentState = EGameModeState::Normal;

    UPROPERTY()
    TWeakObjectPtr<APlayerController> CachedDeadPlayerController;

    // 캐시된 리스폰 위치 정보
    FTransform CachedRespawnTransform;
    bool bHasValidRespawnTransform = false;

private:
    //@플레이어 리스폰 처리 (두 가지 상황을 지원)
    void PlayerRespawn();

public:
    //@사용자 죽음
    void HandlePlayerDeath(APlayerController* PlayerController);

    //@구조물의 활성화
    void HandleFirstStructureActivation(const FStructureData& StructureData);

private:
    bool PreCacheDeathRespawnLocation(APlayerController* PlayerController);

    bool PreCacheLevelTransitionRespawnLocation(const FGameplayTag& TargetLevelTag);

private:
    //@플레이어 텔레포트 실행
    bool PerformPlayerTeleport(APawn* PlayerPawn, const FTransform& TargetTransform);

private:
    //@플레이어 게임플레이 상태 초기화
    void ResetPlayerGameplayState(APlayerController* PlayerController);
#pragma endregion

//@Delegates
#pragma region Delegates
private:
    //@UI Manager 이벤트 바인딩을 위한 델리게이트 핸들
    FDelegateHandle LoadingUIFadeInCompleteHandle;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@UI Manager로부터 로딩 UI Fade-In 완료 이벤트 수신
    UFUNCTION()
    void OnLoadingUIShown();
    UFUNCTION()
    void OnLoadingUIHidden();

protected:
    //@Level Manager로부터 레벨 전환 완료 이벤트 수신
    UFUNCTION()
    void OnLevelTransitionCompleted(const FGameplayTag& CompletedLevelTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@죽음 규칙 결정
    bool DetermineDeathRules(APlayerController* PlayerController);

private:
    //@죽음으로 인한 리스폰 위치 찾기
    bool FindDeathRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform);

    //@레벨 전환으로 인한 리스폰 위치 찾기  
    bool FindLevelTransitionRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform);
#pragma endregion
};