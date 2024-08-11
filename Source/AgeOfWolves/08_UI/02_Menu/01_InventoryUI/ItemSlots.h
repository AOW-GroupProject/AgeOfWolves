// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ItemSlots.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlots, Log, All)

//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FItemSlotsInitFinished);

/**
 * @UItemSlots
 * 
 * Inventory UI에서 Item 목록을 보여주는 UI입니다.
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
    //@외부 바인딩

public:
    //@초기화
    UFUNCTION()
        void InitializeItemSlots();
#pragma endregion

#pragma region Delegate
public:
    FItemSlotsInitFinished ItemSlotsInitFinished;
#pragma endregion

};
