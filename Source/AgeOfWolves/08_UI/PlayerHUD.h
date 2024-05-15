// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerHUD.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHUD, Log, All)

class UQuickSlots;
class UStateBars;
class UScaleBox;

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	UPlayerHUD(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
#pragma endregion

#pragma region Subwidgets
protected:
	UPROPERTY(BlueprintReadWrite, Category = "HUD | State Bars", meta = (BindWidget))
		UScaleBox* StateBarsBox;
	UPROPERTY(EditDefaultsOnly, category = "HUD | State Bars")
		TSubclassOf<UStateBars> StateBarsClass;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "HUD | Quick Slots", meta = (BindWidget))
		UScaleBox* QuickSlotsBox;
	UPROPERTY(EditDefaultsOnly, category = "HUD | Quick Slots")
		TSubclassOf<UQuickSlots> QuickSlotsClass;
#pragma endregion
};
