#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "AOWGameState.h"

#include "AgeOfWolvesGameMode.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAOWGameMode, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
class AAOWGameStateBase;
struct FQuestDataInfo;
class UUIManagerSubsystem;
class ULevelManagerSubsystem;
struct FStructureData;
#pragma endregion

/**
 * AAgeOfWolvesGameMode - 싱글플레이어 최적화된 게임 모드
 *
 * 새로운 설계 철학:
 * 1. GameState의 상태를 직접 관리 (주도권을 가짐)
 * 2. Subsystem들의 기능을 직접 호출 (복잡한 바인딩 제거)
 * 3. 게임 흐름 제어에 집중 (순수한 제어 로직)
 * 4. 상태 저장은 GameState에 위임
 */
UCLASS(minimalapi)
class AAgeOfWolvesGameMode : public AGameModeBase
{
    GENERATED_BODY()

//@기본 설정
#pragma region Default Setting
public:
    AAgeOfWolvesGameMode();

protected:
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void BeginPlay() override;
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    // Subsystem 참조 캐싱 (매번 GetSubsystem 호출 방지)
    void CacheSubsystemReferences();

public:
    // 플레이어 사망 처리 - GameState 업데이트 + UI 표시
    void HandlePlayerDeath(APlayerController* PlayerController);

    // 구조물 활성화 처리 - 레벨 전환 시작
    void HandleFirstStructureActivation(const FStructureData& StructureData);

private:
    //@Game State에 리스폰 요청
    void NotifyRespawnCompleteViaGameState(APlayerController* PlayerController);

    // 플레이어 리스폰 실행 (GameState 정보 기반)
    void ExecutePlayerRespawn();

    // 플레이어 텔레포트 수행
    bool PerformPlayerTeleport(APawn* PlayerPawn, const FTransform& TargetTransform);

    // 플레이어 게임플레이 상태 초기화
    void ResetPlayerGameplayState(APlayerController* PlayerController);

    // 리스폰 위치 미리 계산 (GameState에 캐싱)
    bool PreCalculateRespawnLocation(APlayerController* PlayerController, bool bIsDeathRespawn);

private:
    // 레벨 전환 시작 (LevelManager 직접 호출)
    void StartLevelTransition(const FGameplayTag& TargetLevelTag);

private:
    // 로딩 UI 표시 (UIManager 직접 호출)
    void ShowLoadingUI();

    // 로딩 UI 숨김 (UIManager 직접 호출)
    void HideLoadingUI();

public:
    // 퀘스트 완료 확인
    void HandleAreaQuestCompletion(const FQuestDataInfo& QuestData);

    //@Game State에 퀘스트 완료 알림
    void NotifyQuestCompleteViaGameState(const FQuestDataInfo& QuestData);
#pragma endregion

//@Callbacks
#pragma region Callbacks
    // 로딩 UI 표시 완료 후 콜백
    UFUNCTION()
    void OnLoadingUIShown();

    // 로딩 UI 숨김 완료 후 콜백
    UFUNCTION()
    void OnLoadingUIHidden();

private:
    // LevelManager로부터 레벨 전환 완료 알림을 받는 콜백
    UFUNCTION()
    void OnLevelTransitionCompleted(const FGameplayTag& CompletedLevelTag);
#pragma endregion

//@유틸리티
#pragma region Utility
private:
    UPROPERTY()
    TObjectPtr<UUIManagerSubsystem> CachedUIManager;

    UPROPERTY()
    TObjectPtr<ULevelManagerSubsystem> CachedLevelManager;

    UPROPERTY()
    TObjectPtr<AAOWGameState> CachedGameState;

private:
    // 죽음 규칙 결정
    bool DetermineDeathRules(APlayerController* PlayerController);

    // 죽음으로 인한 리스폰 위치 계산
    bool CalculateDeathRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform);

    // 레벨 전환으로 인한 리스폰 위치 계산
    bool CalculateLevelTransitionRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform);

public:
    // 디버그용 상태 출력
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void PrintGameModeDebugInfo();
#pragma endregion
};