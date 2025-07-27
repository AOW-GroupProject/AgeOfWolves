#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "08_UI/UICollection.h"
#include "Engine/World.h"
#include "Async/Future.h"
#include "HAL/PlatformProcess.h"

#include "UIManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUIManager, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
//@최소 표시 시간 관련 정보를 저장하는 구조체
USTRUCT()
struct FMinimumDisplayTimeInfo
{
	GENERATED_BODY()

	//@최소 표시 시간이 활성화되어 있는지 여부
	UPROPERTY()
	bool bIsActive = false;

	//@최소 표시 시간 (초)
	UPROPERTY()
	int32 MinimumTimeSeconds = 5;

	//@표시 시작 시간
	UPROPERTY()
	double ShowStartTime = 0.0;

	//@숨기기 요청이 대기 중인지 여부
	UPROPERTY()
	bool bHasPendingHideRequest = false;

	//@최소 시간이 경과했는지 확인하는 함수
	bool HasMinimumTimePassed(double CurrentTime) const
	{
		if (!bIsActive) return true;
		return (CurrentTime - ShowStartTime) >= MinimumTimeSeconds;
	}

	//@정보 초기화 함수
	void Reset()
	{
		bIsActive = false;
		MinimumTimeSeconds = 5;
		ShowStartTime = 0.0;
		bHasPendingHideRequest = false;
	}

	//@디버그용 정보 출력
	FString ToString() const
	{
		return FString::Printf(TEXT("Active: %s, MinTime: %ds, StartTime: %.2f, Pending: %s"),
			bIsActive ? TEXT("Yes") : TEXT("No"),
			MinimumTimeSeconds,
			ShowStartTime,
			bHasPendingHideRequest ? TEXT("Yes") : TEXT("No"));
	}
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@로딩 UI Fade-In 완료 이벤트
DECLARE_MULTICAST_DELEGATE(FLoadingUIShown);

//@로딩 UI Fade-Out 완료 이벤트  
DECLARE_MULTICAST_DELEGATE(FLoadingUIHidden);
#pragma endregion


/**
 * UUIManagerSubsystem
 *
 * UI 관련 정보를 관리하는 Subsystem입니다.
 * 특히 System UI들의 생명주기와 최소 표시 시간을 보장합니다.
 */
UCLASS()
class AGEOFWOLVES_API UUIManagerSubsystem : public UGameInstanceSubsystem
{

	//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

	//@Defualt Setting
#pragma region Default Setting
public:
	UUIManagerSubsystem();

protected:
	//@외부 바인딩 - GameState와의 이벤트 연결
	void ExternalBindinToGameState();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
	//@System UI들을 초기화 시점에 생성 및 캐싱하는 함수
	void CreateAndCacheSystemUIs();

	//@특정 System UI를 생성하고 초기 설정하는 헬퍼 함수
	UUserWidget* CreateSystemUIWidget(const FUIInformation& UIInfo);

private:
	//@System UI들을 미리 생성하고 캐싱하는 맵 (UITag -> Widget Instance)
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UUserWidget>> CachedSystemUIs;

	//@최소 표시 시간 정보를 저장하는 맵 (UITag -> MinimumDisplayTimeInfo)
	UPROPERTY()
	TMap<FGameplayTag, FMinimumDisplayTimeInfo> MinimumDisplayTimeInfoMap;

	//@UI별 최소 표시 시간 Promise를 저장하는 맵 (UITag -> Promise)
	//@각 UI당 최초 표시시 한 번만 생성되어 최소 시간 경과를 추적합니다
	TMap<FGameplayTag, TSharedPtr<TPromise<void>>> UIMinimumTimePromises;

protected:
	//@특정 System UI를 표시하는 함수
	UFUNCTION(BlueprintCallable, Category = "UI Management")
	bool ShowSystemUI(const FGameplayTag& UITag);

	//@특정 System UI를 숨기는 함수 (Collapsed 상태로 설정)
	UFUNCTION(BlueprintCallable, Category = "UI Management")
	bool HideSystemUI(const FGameplayTag& UITag);

	//@모든 System UI를 숨기는 함수 (긴급 상황이나 씬 전환 시 사용)
	UFUNCTION(BlueprintCallable, Category = "UI Management")
	void HideAllSystemUIs();

private:
	//@최소 표시 시간 보장을 위한 핵심 함수들

	//@UI 표시시 최소 표시 시간 설정 및 Promise/Timer 생성
	void SetupMinimumDisplayTimeForUI(const FGameplayTag& UITag, const FUIInformation& UIInfo);

	//@Promise가 완료된(최소 시간이 경과한) UI의 pending 요청을 처리
	void ProcessPendingHideRequestIfExists(const FGameplayTag& UITag);

	//@UI를 즉시 숨길 수 있는지 Future 상태를 확인하여 판단
	bool CanHideUIImmediately(const FGameplayTag& UITag) const;

	//@UI 관련 Promise 및 시간 정보를 정리
	void CleanupUITimeTrackingInfo(const FGameplayTag& UITag);

protected:
	//@UI Collection 데이터 에셋
	UPROPERTY()
	TObjectPtr<UUICollection> UICollection;
#pragma endregion

	//@Delegates
#pragma region Delegates
protected:
	//@델리게이트 핸들을 저장해서 나중에 해제할 수 있도록 함
	FDelegateHandle WorldBeginPlayHandle;

public:
	//@Loading UI 표시 완료 이벤트 (GameMode가 구독)
	FLoadingUIShown LoadingUIShown;

	//@Loading UI 숨김 시작 이벤트 (GameMode가 구독)
	FLoadingUIHidden LoadingUIHidden;
#pragma endregion

	//@Callbacks
#pragma region Callbacks
public:
	//@World BeginPlay 시점에 호출될 함수
	void OnWorldBeginPlay();

protected:
	//@GameState로부터 로딩 UI 표시 요청을 받는 콜백
	UFUNCTION()
	void OnRequestShowLoadingUI();

	//@GameState로부터 로딩 UI 숨김 요청을 받는 콜백
	UFUNCTION()
	void OnRequestHideLoadingUI();
#pragma endregion

	//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	//@System UI 캐시 상태를 검증하는 헬퍼 함수
	bool ValidateSystemUICache() const;

public:
	//@특정 System UI가 현재 표시되고 있는지 확인하는 함수
	UFUNCTION(BlueprintCallable, Category = "UI Management")
	bool IsSystemUIVisible(const FGameplayTag& UITag) const;

public:
	//@특정 UI Category에 해당되는 모든 UI 정보를 제공하는 함수
	const TArray<FUIInformation>* GetUICategoryInformations(const EUICategory& UICategory) const;

	//@특정 UI의 정보를 제공하는 함수
	const FUIInformation* GetUIInformation(const EUICategory& UICategory, const FGameplayTag& UITag) const;

public:
	//@캐싱된 System UI 위젯에 직접 접근하는 함수 (고급 사용자용)
	UFUNCTION(BlueprintCallable, Category = "UI Management")
	UUserWidget* GetCachedSystemUI(const FGameplayTag& UITag) const;

	//@디버그용: 현재 추적 중인 UI들의 최소 표시 시간 정보 출력
	UFUNCTION(BlueprintCallable, Category = "UI Management | Debug")
	void PrintMinimumDisplayTimeDebugInfo() const;
#pragma endregion

};