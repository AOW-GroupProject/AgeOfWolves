// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "QuickSlots.generated.h"

class UVerticalBox;
class UQuickSlot;

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlots, Log, All)

/**
 * UQuickSlots
 * 
 * @����: HUD ���� ��� �� �ϳ��� ����ڰ� Ȱ�� ������ Quick Slot�� ��Ÿ���� UI�Դϴ�.
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
	virtual void NativeOnInitialized(); // �޸� �Ҵ� �Ϸ�, ȭ�鿡 �����Ǳ� ���� ȣ���
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // ȭ�鿡 �����Ǳ� ������ ȣ���
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
#pragma endregion

#pragma region SubWidgets
protected:
	// Quick Slot ����
	UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
		TSubclassOf<UQuickSlot> QuickSlotClass;
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UVerticalBox* PotionSlotList;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
		int32 PotionSlotNum;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot | Tool", meta = (BindWidget))
		UVerticalBox* ToolSlotList;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
		int32 ToolSlotNum;
#pragma endregion

#pragma region Input Action
	// @TODO : Key �Է� �̺�Ʈ�� ����� �ݹ� �Լ� �ʿ�
#pragma endregion 

};
