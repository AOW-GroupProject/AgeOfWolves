// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "HUD_QuickSlotsUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlotsUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UHorizontalBox;
class UVerticalBox;
class UItemSlot;
class UHUD_QuickSlotsUI_AbilitySlot;
class UBaseAbilitySystemComponent;
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
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByQuickSlots);

//@초기화 완료 이벤트
DECLARE_DELEGATE(FQuickSlotsInitFinished);
#pragma endregion


/**
 *	@UHUD_QuickSlotsUI
 * 
 *	HUD 구성 중 활용 가능한 퀵슬롯 UI
 */
UCLASS()
class AGEOFWOLVES_API UHUD_QuickSlotsUI : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UHUD_QuickSlotsUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized();
	//~ End UUserWidget Interface

protected:
	//@외부 바인딩
	void ExternalBindToASC();

protected:
	//@내부 바인딩
	void InternalBindToToolItemSlots(UItemSlot* ItemSlot, bool bLast = false);
	void InternalBindToBattoujutsuAbilitySlot(UHUD_QuickSlotsUI_AbilitySlot* AbilitySlot);
	void InternalBindToJujutsuAbilitySlots(UHUD_QuickSlotsUI_AbilitySlot* AbilitySlot);

public:
	//@초기화
	UFUNCTION()
		void InitializeQuickSlotsUI();

protected:
	//@초기화 완료 체크
	bool bToolItemSlotInitFinished = false;
	bool bBattouJutsuAbilitySlotInitFinished = false;
	bool bJujutsuAbilitySlotsInitFinished = false;
	void CheckAllUIsInitFinished();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@Tool Item Slots 생성
	void CreateToolItemQuickSlots();
	//@발도술 슬롯 생성
	void CreateBattouJutsuAbilitySlot();
	//@주술 슬롯 생성
	void CreateJujutsuAbilitySlot();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯", meta = (BindWidget))
		UVerticalBox* QuickSlotsBox;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 도구 아이템", meta = (BindWidget))
		UHorizontalBox* ToolItemQuickSlotBox;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 도구 아이템")
		TSubclassOf<UItemSlot> ToolItemSlotClass;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 발도술", meta = (BindWidget))
		UHorizontalBox* BattoujutsuQuickSlotBox;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 발도술 슬롯 BP 클래스")
		TSubclassOf<UHUD_QuickSlotsUI_AbilitySlot> BattoujutsuAbilitySlotClass;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 주술", meta = (BindWidget))
		UHorizontalBox* JujutsuQuickSlotBox;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 주술 슬롯 BP 클래스")
		TSubclassOf<UHUD_QuickSlotsUI_AbilitySlot> JujutsuAbilitySlotClass;

protected:
	//@Tool Item Slot 목록
	TArray<UItemSlot*> ToolItemSlots;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestStartInitByQuickSlots RequestStartInitByQuickSlots;

	//@초기화 완료 이벤트
	FQuickSlotsInitFinished QuickSlotsInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	UFUNCTION()
		void OnToolItemSlotsInitFinished();
	UFUNCTION()
		void OnBattoujutsuAbilitySlotInitFinished();
	UFUNCTION()
		void OnJujutsuAbilitySlotsInitFinished();

protected:
	UFUNCTION()
		void OnAbilitySpecGiven(FGameplayAbilitySpec AbilitySpec);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
