// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "08_UI/UICollection.h"

#include "UIComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UUserWidget;
class UUIManagerSubsystem;
class UBaseInputComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestInitializationByUIComp);
//@IMC 변경 요청 이벤트
DECLARE_DELEGATE_OneParam(FRequestSwapIMC, const FGameplayTag&);

//@HUD의 초기화 완료 이벤트
DECLARE_DELEGATE(FUIsForInventoryReady);
//@State Bars의 초기화 완료 이벤트
DECLARE_DELEGATE(FUIsForAttributeSetReady);

//@Menu UI관련 IA 트리거 알림 이벤트
DECLARE_DELEGATE_OneParam(FNotifyMenuUIInputTriggered, const FGameplayTag&);
//@Menu UI관련 IA 해제 알림 이벤트
DECLARE_DELEGATE_OneParam(FNotifyMenuUIInputReleased, const FGameplayTag&);

//@Widget의 Visibility 관련 알림 이벤트: Animation
DECLARE_MULTICAST_DELEGATE_TwoParams(FWidgetVisibilityChanged, UUserWidget*, bool);

//@Interaction UI Event
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyInteractionUIInputActivation, const FGameplayTag&);
#pragma endregion

/*
* UUIComponent
* 
* 사용자의 UI를 관리하는 컴포넌트입니다.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UUIComponent : public UActorComponent
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:	
	UUIComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//~UActorComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End of UActorComponent Interface
private:
	//@외부 바인딩
	void ExternalBindingToInputComponent(const APlayerController* PlayerController);
private:
	//@내부 바인딩
	void InternalBindToHUDUI();
	void InternalBindToMenuUI();
public:
	//@초기화
	UFUNCTION()
		void InitializeUIComponent();
protected:
	//@Inventory의 관련 UI들의 초기화 작업 완료를 체크합니다.
	bool bQuickSlotsReadyForLoading = false;
	bool bInventoryUIReadyForLoading = false;
	void CheckAllUIsForInventoryReady();

	//@Player State Base의 AttributeSet 관련 UI들의 초기화 작업 완료를 체크합니다.
	bool bStateBarsReadyForLoading = false;
	void CheckAllUIsForAttributeSetReady();

	//@Player HUD, Menu UI들의 초기화 작업 완료를 체크합니다.
	bool bHUDInitFinished = false;
	bool bMenuUIInitFinished = false;
	void CheckAllUIsForDefaultVisibilitySetting();

	//@TODO: 모든 Interaction UI 초기화 완료 체크 함수 작성...

#pragma endregion

//@Property/Info...etc
#pragma region UI
private:
	//@모든 UI 상태를 리셋합니다.
	void ResetUIs();
	//@카테고리 별 Reset 작업 수행
	void ResetCategoryUI(EUICategory UICategory, UUIManagerSubsystem* UIManagerSubsystem);

protected:
	//@Create Widget
	void CreateAndSetupWidget(APlayerController* PC, EUICategory UICategory, const FUIInformation& UIInfo, UEnum* EnumPtr);
	//@HUD UI
	void SetupHUDUI(UUserWidget* NewWidget);
	//@Menu UI
	void SetupMenuUI(UUserWidget* NewWidget);
	//@Interaction UI
	void SetupInteractionUI(const FGameplayTag& UITag, UUserWidget* NewWidget);

protected:
	//@Widget을 화면에 나타냅니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowUI(EUICategory UICategory, const FGameplayTag& UITag);
	UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowAllUI(EUICategory UICategory);
	//@Widget을 화면에서 숨깁니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		void HideUI(EUICategory UICategory, const FGameplayTag& UITag);
	UFUNCTION(BlueprintCallable, Category = "UI")
		void HideAllUI(EUICategory UICategory);

protected:
	//@HUD
    UPROPERTY()
        TObjectPtr<UUserWidget> HUDUI;
	//@Menu
	UPROPERTY()
		TObjectPtr<UUserWidget> MenuUI;
	//@Interaction
	UPROPERTY()
		TMap<FGameplayTag, UUserWidget*> MInteractionUIs;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestInitializationByUIComp RequestInitializationByUIComp;
	//@IMC 변경 오청 이벤트
	FRequestSwapIMC RequestSwapIMC;

public:
	//@Inventory 로딩 준비 완료 이벤트
	FUIsForInventoryReady UIsForInventoryReady;
	//@Attriburte Set 로딩 준비 완료 이벤트
	FUIsForAttributeSetReady UIsForAttributeSetReady;

public:
	//@UI 가시성 변화 이벤트
	FWidgetVisibilityChanged WidgetVisibilityChanged;

public:
	//@Menu UI 입력 이벤트
	FNotifyMenuUIInputTriggered NotifyMenuUIInputTriggered;
	FNotifyMenuUIInputReleased NotifyMenuUIInputReleased;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Player HUD의 초기화 완료 이벤트 구독
	UFUNCTION()
		void OnHUDInitFinished();
	UFUNCTION()
		void OnMenuUIInitFinished();

protected:
	//@Player HUD의 State Bar의 초기화 완료 이벤트 구독
	UFUNCTION()
		void StateBarsInitFinishedNotified();

protected:
	//@Player HUD의 Quick Slots의 초기화 완료 이벤트 구독
	UFUNCTION()
		void QuickSlotsInitFinishedNotified();
	//@Menu UI의 Inventory UI 초기화 완료를 알리는 이벤트 호출
	UFUNCTION()
		void InventoryUIInitFinishedNotified();

protected:
	//@UI Input Tag 활성화 이벤트 구독
	UFUNCTION()
		void OnUIInputTriggered(const FGameplayTag& InputTag);
	//@UI Input Tag 해제 이벤트 구독
	UFUNCTION()
		void OnUIInputReleased(const FGameplayTag& InputTag);

protected:
	//@Menu UI 키 입력을 통한 Menu UI 닫기 요청 이벤트 구독
	UFUNCTION()
		void OnRequestCloseMenuUI();
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	//@Input Tag를 통해 UI를 찾습니다.
	FString FindUICategoryFromInputTag(const FGameplayTag& InputTag);

public:
	//@UICategory와 UI Tag를 통해 UI를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		UUserWidget* GetUI(EUICategory UICategory, const FGameplayTag& UITag = FGameplayTag()) const;
	//@UI Category 관련 UI들을 모두 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		TArray<UUserWidget*> GetCategoryUIs(EUICategory UICategory) const;
#pragma endregion

};
