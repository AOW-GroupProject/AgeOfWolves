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
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    UPROPERTY()
    FGameplayTag CachedNextLevelTag;

public:
    // PlayerController 기반으로 변경된 public 함수
    void HandlePlayerDeath(APlayerController* PlayerController);

private:
    //@레벨 전환 시작을 Game State에 알리는 내부 함수
    void NotifyStartLevelTransition();
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
    //@구조물(늑대 상) 상호작용 발생 이벤트 구독
    UFUNCTION()
        void OnStructureInteractionActtivated(const FStructureData& StructureData);

protected:
    //@UI Manager로부터 로딩 UI Fade-In 완료 이벤트 수신
    UFUNCTION()
    void OnLoadingUIFadeInComplete();

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
#pragma endregion
};