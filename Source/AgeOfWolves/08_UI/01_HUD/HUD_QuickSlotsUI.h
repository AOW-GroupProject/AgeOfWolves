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
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
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

protected:
	//@내부 바인딩

public:
	//@초기화
	UFUNCTION()
		void InitializeQuickSlotsUI();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	void CreateToolItemQuickSlots();

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

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 주술", meta = (BindWidget))
		UHorizontalBox* JujutsuQuickSlotBox;

protected:
	//@Tool Item Slot 목록
	TArray<UItemSlot*> ToolItemSlots;

#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 완료 이벤트
	FQuickSlotsInitFinished QuickSlotsInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
