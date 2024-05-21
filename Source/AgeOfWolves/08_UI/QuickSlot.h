// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/EditableText.h"
#include "GameplayTagContainer.h"

#include "QuickSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlot, Log, All)

/**
 * UQuickSlot
 * 
 * @목적: HUD 구성 요소 중 QuickSlots의 구성 요소로 사용자가 활용 가능한 아이템을 표시하는 UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API UQuickSlot : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	UQuickSlot(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
#pragma endregion

#pragma region SubWidgets
protected:
	/*
	* @목적 : HUD의 Quick Slot들 중 단일 Slot을 구성하는 Widget입니다.
	* @설명 : Quick Slot 목록 중 한 Slot을 나타내며, Overlay를 통해 UImage 와 UEditableText를 하나로 묶어줍니다.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UOverlay* SlotOverlay;
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UImage* SlotImage;
	bool bStackable; // ItemNum 표시 여부를 결정합니다.
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UEditableText* SlotItemNum;

public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetSlotImage(UTexture2D* InTexture) { SlotImage->SetBrushFromTexture(InTexture); }
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetIsStackable(bool InBool) { bStackable = InBool; }
	UFUNCTION(BlueprintCallable, meta = (EditCondition = "bStackable == true"))
		FORCEINLINE void SetSlotItemNum(float InFloat) { SlotItemNum->SetText(FText::AsNumber(InFloat)); }
#pragma endregion

// @TODO : 특정 키 입력 이벤트에 해당 Slot 활성화 콜백 함수를 등록해야 합니다.
#pragma region Input
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		FGameplayTag MappingInputTag;
#pragma endregion


};
