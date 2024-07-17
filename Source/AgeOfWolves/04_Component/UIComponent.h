// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "08_UI/UICollection.h"

#include "UIComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All)

class UUserWidget;
class UUIManagerSubsystem;

//@Item Removed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemovalRequested, const FGuid&, UniqueItemID);
//@Item Activated
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemActivated, const FGuid&, UniqueItemID);
//@Item QuickSlot Assigned
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuickSlotAssigned, int32, SlotIndex, const FGuid&, UniqueItemID);


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
    void LoadUI();
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
        TMap<FGameplayTag,UUserWidget*> MHUDUIs;
	//@System
	UPROPERTY()
		TMap<FGameplayTag, UUserWidget*> MSystemUIs;
	//@Interaction
	UPROPERTY()
		TMap<FGameplayTag, UUserWidget*> MInteractionUIs;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
        UUserWidget* GetUI(EUICategory UICategory, const FGameplayTag& UITag) const;
	UFUNCTION(BlueprintCallable, Category = "UI")
		TArray<UUserWidget*> GetCategoryUIs(EUICategory UICategory) const;
#pragma endregion

#pragma region UI Input
public:
	UFUNCTION()
		void OnUIInputTagTriggered(const FGameplayTag& InputTag);
	UFUNCTION()
		void OffUIInputTagReleased(const FGameplayTag& InputTag);
#pragma endregion

#pragma region Inventory UI

#pragma region Inventory UI - Delegates
public:
	//@Delegate: Item 제거 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FOnItemRemovalRequested OnItemRemovalRequested;
	//@Delegate: Item 활성화 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FOnItemActivated OnItemActivated;
	//@Delegate: Item을 Quick Slot 추가 요청
	UPROPERTY(BlueprintAssignable, Category = "UI|Inventory")
		FOnQuickSlotAssigned OnQuickSlotAssigned;
private:
	//@Item Removal
	void RequestItemRemoval(const FGuid& UniqueItemID);
	//@Item Activation
	void RequestItemActivation(const FGuid& UniqueItemID);
	//@Item QuickSlot Assigned
	void RequestQuickSlotAssignment(int32 SlotIndex, const FGuid& UniqueItemID);
#pragma endregion

#pragma region Inventory UI - Callbacks
public:
	//@Callbacks: Inventory에 새로운 아이템 추가 시 호출되는 콜백
	UFUNCTION()
		void OnInventoryItemAdded(const FGuid& UniqueItemID);
	//@Callbacks: Inventory에서 기존 아이템 제거 시 호출되는 콜백
	UFUNCTION()
		void OnInventoryItemRemoved(const FGuid& UniqueItemID, const FGameplayTag& ItemTag);
#pragma endregion

#pragma endregion

};
