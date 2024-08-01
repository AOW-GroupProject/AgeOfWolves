// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "08_UI/UICollection.h"

#include "UIComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All)

class UUserWidget;
class UUIManagerSubsystem;
class UBaseInputComponent;

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestInitializationByUIComp);

//@HUD의 초기화 완료 이벤트
DECLARE_DELEGATE(FUIsForInventoryReady);
//@State Bars의 초기화 완료 이벤트
DECLARE_DELEGATE(FUIsForAttributeSetReady);

//@TODO: 각 System UI에 옮길 예정
//@System UI Event
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifySystemUIInputActivation, const FGameplayTag&);
//@TODO: 각 Interaction UI에 옮길 예정
//@Interaction UI Event
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyInteractionUIInputActivation, const FGameplayTag&);

//@TODO: Inventory UI로 옮길 예정
//@아이템 제거 알림 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyItemRemovalFromInventory, const FGuid&);
//@아이템 업데이트 알림 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyInventoryItemUpdated, const FGuid&);
//@아이템 활성화 알림 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyInventoryItemActivation, const FGuid&);

/*
* UUIComponent
* 
* 사용자의 UI를 관리하는 컴포넌트입니다.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UUIComponent : public UActorComponent
{
	GENERATED_BODY()

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
	//@내부 바인딩
	void InternalBindToHUDUI();
	void InternalBindToSystemUI();
public:
	//@초기화
	UFUNCTION()
		void InitializeUIComponent();
protected:
	void CheckUIsForInventoryReady();
	void CheckAllUIsForAttributeSetReady();
#pragma endregion

#pragma region UI
protected:
	void CreateAndSetupWidget(APlayerController* PC, EUICategory UICategory, const FUIInformation& UIInfo, UEnum* EnumPtr);
	//@HUD UI
	bool bQuickSlotsReadyForLoading = false;
	bool bStateBarsReadyForLoading = false;
	void SetupHUDUI(UUserWidget* NewWidget);
	//@System UI
	bool bInventoryUIReadyForLoading = false;
	void SetupSystemUI(UUserWidget* NewWidget);
	//@Interaction UI
	bool bInteractionUIsInitFinished = false;
	void SetupInteractionUI(const FGameplayTag& UITag, UUserWidget* NewWidget);

protected:
	//@Widget을 화면에 나타냅니다.
    UFUNCTION(BlueprintCallable, Category = "UI")
        void ShowUI(EUICategory UICategory, const FGameplayTag& UITag);
	//@Widget을 화면에서 숨깁니다.
    UFUNCTION(BlueprintCallable, Category = "UI")
        void HideUI(EUICategory UICategory, const FGameplayTag& UITag);

protected:
	//@HUD
    UPROPERTY()
        TObjectPtr<UUserWidget> HUDUI;
	//@System
	UPROPERTY()
		TObjectPtr<UUserWidget> SystemUI;
	//@Interaction
	UPROPERTY()
		TMap<FGameplayTag, UUserWidget*> MInteractionUIs;
private:
	FString FindUICategoryFromInputTag(const FGameplayTag& InputTag);
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
        UUserWidget* GetUI(EUICategory UICategory, const FGameplayTag& UITag = FGameplayTag()) const;
	UFUNCTION(BlueprintCallable, Category = "UI")
		TArray<UUserWidget*> GetCategoryUIs(EUICategory UICategory) const;
#pragma endregion

#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestInitializationByUIComp RequestInitializationByUIComp;
public:
	//@Inventory 로딩 준비 완료 이벤트
	FUIsForInventoryReady UIsForInventoryReady;
	//@Attriburte Set 로딩 준비 완료 이벤트
	FUIsForAttributeSetReady UIsForAttributeSetReady;
public:
	//@TODO: 각 System UI에 옮길 예정
	//@System 관련 사용자 입력 Trigger 이벤트
	FNotifySystemUIInputActivation NotifySystemUIInputActivation;
	//@TODO: 각 Interaction UI에 옮길 예정
	//@Interaction 관련 사용자 입력 Trigger 이벤트
	FNotifyInteractionUIInputActivation NotifyInteractionUIInputActivation;

//@TODO: Inventory UI 제작 시 아래 Delegate들을 Inventory UI로 옮겨줍니다.
public:
	//@인벤토리 UI 아이템 제거 알림 이벤트
	FNotifyItemRemovalFromInventory NotifyItemRemovalFromInventory;
	//@인벤토리 UI 아이템 업데이트 알림 이벤트
	FNotifyInventoryItemUpdated NotifyInventoryItemUpdated;
	//@인벤토리 UI 아이템 활성화 알림 이벤트
	FNotifyInventoryItemActivation NotifyInventoryItemActivation;
#pragma endregion

#pragma region Callbacks
public:
	UFUNCTION()
		void StateBarsInitFinishedNotified();
	//@Callback: HUD의 Quick Slots 초기와 완료를 알리는 이벤트 호출
	UFUNCTION()
		void QuickSlotsInitFinishedNotified();
	//@Callback: System UI의 Inventory UI 초기화 완료를 알리는 이벤트 호출
	UFUNCTION()
		void InventoryUIInitFinishedNotified();
#pragma endregion

};
