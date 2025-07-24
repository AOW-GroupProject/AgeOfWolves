#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "08_UI/UICollection.h"

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
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@로딩 UI Fade-In 완료 이벤트
DECLARE_MULTICAST_DELEGATE(FLoadingUIFadeInComplete);

//@로딩 UI Fade-Out 완료 이벤트  
DECLARE_MULTICAST_DELEGATE(FLoadingUIFadeOutStart);
#pragma endregion


/**
 * UUIManagerSubsystem
 * 
 * UI 관련 정보를 관리하는 Subsystem입니다.
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
	//@외부 바인딩...
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

protected:
	UPROPERTY()
		TObjectPtr<UUICollection> UICollection;
#pragma endregion

//@Delegates
#pragma region Delegates
protected:
	//@델리게이트 핸들을 저장해서 나중에 해제할 수 있도록 함
	FDelegateHandle WorldBeginPlayHandle;

public:
	//@Fade-In 완료 이벤트
	FLoadingUIFadeInComplete LoadingUIFadeInComplete;
	//@Fade-Out 시작 이벤트
	FLoadingUIFadeOutStart LoadingUIFadeOutStart;
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
	//@World BeginPlay 시점에 호출될 함수
	void OnWorldBeginPlay();

protected:
	//@로딩 UI 그리기 요청 이벤트를 구독
	UFUNCTION()
	void OnRequestShowLoadingUI();
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
#pragma endregion

};
