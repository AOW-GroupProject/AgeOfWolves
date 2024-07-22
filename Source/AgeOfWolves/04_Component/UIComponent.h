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

//@HUD UI Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIHUDInputEvent, const FGameplayTag&, InputTag);
//@System UI Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUISystemInputEvent, const FGameplayTag&, InputTag);
//@Interaction UI Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIInteractionInputEvent, const FGameplayTag&, InputTag);
//@Item Removed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemovalRequested, const FGuid&, UniqueItemID);
//@Item Activated
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemActivated, const FGuid&, UniqueItemID);

//@Item QuickSlot Assigned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FRequestItemAssignmentToQuickSlot, int32, SlotNum, const FGuid&, UniqueItemID, EItemType, ItemType, const FGameplayTag&, ItemTag, int32, ItemCount);
//@Quick Slot Item Removal
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FRequestQuickSlotItemRemoval, int32, SlotNum, const FGuid&, UniqueItemID, EItemType, ItemType, const FGameplayTag&, ItemTag, int32, ItemCount);
//@QuickSlot Itme Updated 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FRequestQuickSlotItemUpdate, int32, SlotNum, const FGuid&, UniqueItemID, EItemType, ItemType, const FGameplayTag&, ItemTag, int32, ItemCount);

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
#pragma endregion

#pragma region UI
public:
	//@Widget 생성
    void CreateUI(const UUIManagerSubsystem& UIManagerSubsystem, EUICategory Category);
	//@Widget 정보 Load
    bool LoadUI();
	//@Binding 작업
	void BindUIToUIComponent();
	void BindToInventoryComponent(const APawn* OwningPawn);
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

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
        UUserWidget* GetUI(EUICategory UICategory, const FGameplayTag& UITag) const;
	UFUNCTION(BlueprintCallable, Category = "UI")
		TArray<UUserWidget*> GetCategoryUIs(EUICategory UICategory) const;
#pragma endregion

#pragma region Input Component
private:
	UPROPERTY()
		UBaseInputComponent* BaseInputComponent;
public:
	bool LoadUIInputComponent();
	void BindInputComponentToInputActions();
private:
	void DestroyUIInputComponent();
	void EnterSystemUIMode();
	void ExitSystemUIMode();

public:
	void SetUIInputPriority(int32 NewPriority);
	void SetUIInputBlocking(bool bShouldBlock);

private:
	FString FindUICategoryFromInputTag(const FGameplayTag& InputTag);
#pragma endregion

#pragma region Delegates
	//@Delegate: HUD
	UPROPERTY(BlueprintAssignable, Category = "UI|Input")
		FOnUIHUDInputEvent OnUIHUDInputTriggered;
	//@Delegate: HUD
	UPROPERTY(BlueprintAssignable, Category = "UI|Input")
		FOnUIHUDInputEvent OnUIHUDInputReleased;
	//@Delegate: System
	UPROPERTY(BlueprintAssignable, Category = "UI|Input")
		FOnUISystemInputEvent OnUISystemInputTriggered;
	//@Delegate: System
	UPROPERTY(BlueprintAssignable, Category = "UI|Input")
		FOnUISystemInputEvent OnUISystemInputReleased;
	//@Delegate: Interaction
	UPROPERTY(BlueprintAssignable, Category = "UI|Input")
		FOnUIInteractionInputEvent OnUIInteractionInputTriggered;
	//@Delegate: Interaction
	UPROPERTY(BlueprintAssignable, Category = "UI|Input")
		FOnUIInteractionInputEvent OnUIInteractionInputReleased;

public:
	//@Delegate: Item을 Quick Slot 추가 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FRequestItemAssignmentToQuickSlot RequestItemAssignmentToQuickSlots;
	//@Delegate: Quick Slot Item의 제거 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FRequestQuickSlotItemRemoval RequestQuickSlotItemRemoval;
	//@Delegate: Quick Slot Item의 업데이트 요청 
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FRequestQuickSlotItemUpdate RequestQuickSlotItemUpdate;
	//@Delegate: Item 제거 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FOnItemRemovalRequested OnItemRemovalRequested;
	//@Delegate: Item 활성화 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FOnItemActivated OnItemActivated;
#pragma endregion

#pragma region Callbacks
public:
	//@Callback: Input Comp
	UFUNCTION()
		void OnUIInputTagTriggered(const FGameplayTag& InputTag);
	//@Callback: Input Comp
	UFUNCTION()
		void OffUIInputTagReleased(const FGameplayTag& InputTag);
	//@Callback: Inventory UI
	UFUNCTION()
		void QuickSlotAssignmentRequested(
			int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
		);
	//@Callback: Inventory UI
		//@Callback: Inventory Comp
	UFUNCTION()
		void QuickSlotItemRemovalRequested(
			int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
		);

	//@Callback: Inventory Comp
	UFUNCTION()
		void QuickSlotItemUpdateRequested(
			int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
		);
#pragma endregion

};
