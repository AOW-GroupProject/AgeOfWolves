// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerHUD.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHUD, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UQuickSlots;
class UStateBars;
class UHUD_StatusUI;
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
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByHUD);
//@HUD 초기화 완료 이벤트
DECLARE_DELEGATE(FHUDInitFinished);
#pragma endregion

//@State Bars 초기화 완료 이벤트
DECLARE_DELEGATE(FNotifyStateBarsInitFinished);
//@Quick Slot 초기화 완료 이벤트
DECLARE_DELEGATE(FNotifyQuickSlotsInitFinished);
#pragma endregion

/**
 * UPlayerHUD
 * 
 * 인 게임에서 캐릭터 관련 HUD를 나타내는 UI 입니다.
 * 
 */
UCLASS()
class AGEOFWOLVES_API UPlayerHUD : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UPlayerHUD(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
protected:
	//@외부 바인딩
	void ExternalBindingToUIComponent();

protected:
	//@내부 바인딩
	void InternalBindToStatusUI(UHUD_StatusUI* StatusUI);
	void InternalBindToQuickSlots(UQuickSlots* QuickSlots);

public:
	//@초기화
	UFUNCTION()
		void InitializePlayerHUD();

protected:
	bool bStatusUIInitFinished = false;
	bool bQuickSlotsInitFinished = false;
	void CheckAllUIsInitFinsiehd();

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@Status UI 생성
	void CreateStatusUI();
	//@Quick Slot UI 생성
	void CreateQuickSlotUI();
	//@HP Potion UI 생성
	void CreateHPPotionUI();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "HUD | Status UI", meta = (BindWidget))
		UOverlay* StatusUIOverlay;
	UPROPERTY(EditDefaultsOnly, category = "HUD | Status UI")
		TSubclassOf<UUserWidget> StatusUIClass;

	UPROPERTY(BlueprintReadWrite, Category = "HUD | Quick Slot", meta = (BindWidget))
		UOverlay* QuickSlotUIOverlay;
	UPROPERTY(EditDefaultsOnly, category = "HUD | Quick Slot")
		TSubclassOf<UUserWidget> QuickSlotUIClass;

	UPROPERTY(BlueprintReadWrite, Category = "HUD | HP Potion UI", meta = (BindWidget))
		UOverlay* HPPotionUIOverlay;
	UPROPERTY(EditDefaultsOnly, category = "HUD | HP Potion UI")
		TSubclassOf<UUserWidget> HPPotionUIClass;

protected:
	TObjectPtr<UHUD_StatusUI> StatusUIRef;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestStartInitByHUD RequestStartInitByHUD;
	//@초기화 완료 이벤트
	FHUDInitFinished HUDInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@UI의 가시성 변화 이벤트에 바인딩 되는 콜백
	UFUNCTION(BlueprintNativeEvent)
		void OnUIVisibilityChanged(UUserWidget* Widget, bool bVisible);
	virtual void OnUIVisibilityChanged_Implementation(UUserWidget* Widget, bool bVisible);

protected:
	//@퀵슬롯 초기화 완료 이벤트에 등록하는 콜백
	UFUNCTION()
		void OnQuickSlotsInitFinished();
	//@상태 창 초기화 완료 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnStatusUIInitFinished();
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
