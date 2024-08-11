// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ItemSlots.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlots, Log, All)

//@�ʱ�ȭ �Ϸ� �̺�Ʈ(�ʱ�ȭ �۾� �񵿱�ȭ)
DECLARE_DELEGATE(FItemSlotsInitFinished);

/**
 * @UItemSlots
 * 
 * Inventory UI���� Item ����� �����ִ� UI�Դϴ�.
 */
UCLASS()
class AGEOFWOLVES_API UItemSlots : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
    UItemSlots(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
private:
    //@�ܺ� ���ε�

public:
    //@�ʱ�ȭ
    UFUNCTION()
        void InitializeItemSlots();
#pragma endregion

#pragma region Delegate
public:
    FItemSlotsInitFinished ItemSlotsInitFinished;
#pragma endregion

};
