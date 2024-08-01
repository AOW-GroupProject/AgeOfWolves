// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All)

DECLARE_DELEGATE(FInventoryUIInitFinished);

/**
 * 
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
	void ExternalBindingToInventoryComp();
	void ExternalBindingToInputComp();

public:
	UFUNCTION()
		void InitializeInventoryUI();
#pragma endregion

#pragma region Delegates
public:
	FInventoryUIInitFinished InventoryUIInitFinished;
#pragma endregion

};
