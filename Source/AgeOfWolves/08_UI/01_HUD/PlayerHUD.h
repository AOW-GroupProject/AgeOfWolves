// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerHUD.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHUD, Log, All)

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByHUD);
//@State Bars 초기화 완료 이벤트
DECLARE_DELEGATE(FNotifyStateBarsInitFinished);
//@Quick Slot 초기화 완료 이벤트
DECLARE_DELEGATE(FNotifyQuickSlotsInitFinished);

class UScaleBox;
class UQuickSlots;
class UStateBars;

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
protected:
	//@내부 바인딩
	void InternalBindToStateBars(UStateBars* StateBars) ;
	void InternalBindToQuickSlots(UQuickSlots* QuickSlots) ;
public:
	//@초기화
	UFUNCTION()
		void InitializePlayerHUD();
#pragma endregion

#pragma region Subwidgets
private:
	void CreateStateBars();
	void CreateQuickSlots();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "HUD | State Bars", meta = (BindWidget))
		UScaleBox* StateBarsBox;
	UPROPERTY(EditDefaultsOnly, category = "HUD | State Bars")
		TSubclassOf<UStateBars> StateBarsClass;
public:
	UFUNCTION(BlueprintCallable, Category = "HUD | State Bars")
		UStateBars* GetStateBarsUI() const;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "HUD | Quick Slots", meta = (BindWidget))
		UScaleBox* QuickSlotsBox;
	UPROPERTY(EditDefaultsOnly, category = "HUD | Quick Slots")
		TSubclassOf<UQuickSlots> QuickSlotsClass;
public:
	UFUNCTION(BlueprintCallable, Category = "HUD | Quick Slots")
		UQuickSlots* GetQuickSlotsUI() const;

#pragma endregion

#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestStartInitByHUD RequestStartInitByHUD;
	//@State Bars 초기화 완료 알림 이벤트
	FNotifyStateBarsInitFinished NotifyStateBarsInitFinished;
	//@Quick Slots 초기화 완료 알림 이벤트
	FNotifyQuickSlotsInitFinished NotifyQuickSlotsInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
	UFUNCTION()
		void OnQuickSlotsInitFinished();
	UFUNCTION()
		void OnStateBarsInitFinished();
#pragma endregion

};
