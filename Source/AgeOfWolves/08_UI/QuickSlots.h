// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "QuickSlots.generated.h"

class UVerticalBox;
class UQuickSlot;

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlots, Log, All)

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UQuickSlots : public UUserWidget
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	UQuickSlots(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
#pragma endregion

#pragma region SubWidgets
	// #0. Quick Slot 유형
	UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
		TSubclassOf<UQuickSlot> QuickSlotClass;
	// #1. Potion Slot List
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UVerticalBox* PotionSlotList;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
		int32 PotionSlotNum;
	// #2. Tool  Slot List
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot | Tool", meta = (BindWidget))
		UVerticalBox* ToolSlotList;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
		int32 ToolSlotNum;

#pragma endregion

#pragma region Input Action
	// @TODO : Key 입력 이벤트에 등록할 콜백 함수 필요
#pragma endregion 

};
