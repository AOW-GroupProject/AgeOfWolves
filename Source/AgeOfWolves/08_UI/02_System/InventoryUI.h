// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All)

DECLARE_DELEGATE(FInventoryUIInitFinished);

/**
 * UInventoryUI
 * 
 * Inventory UI를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	UInventoryUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
protected:
	//@외부 바인딩
	void ExternalBindingToInventoryComp();
	void ExternalBindingToInputComp();
public:
	//@초기화
	UFUNCTION()
		void InitializeInventoryUI();
#pragma endregion

#pragma region Delegates
public:
	FInventoryUIInitFinished InventoryUIInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
	//@UI Input Tag 활성화 이벤트에 바인딩 되는 콜백: Input Comp
	UFUNCTION()
		void OnUIInputTagTriggered(const FGameplayTag& InputTag);
	//@UI Input Tag 해제 이벤트에 바인딩 되는 콜백: Input Comp
	UFUNCTION()
		void OnUIInputTagReleased(const FGameplayTag& InputTag);
protected:
	//@새로운 아이템 추가 이벤트에 바인딩 되는 콜백: Inven Comp
	UFUNCTION()
		void OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag);
	//@기존 아이템 제거 이벤트에 바인딩 되는 콜백: Inven Comp
	UFUNCTION()
		void OnItemRemovedFromInventory(const FGuid& UniqueItemID);
	//@기존 아이템 업데이트 이벤트에 바인딩 되는 콜백: Inven Comp
	UFUNCTION()
		void OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 UpdatedItemCount);
#pragma endregion

};
