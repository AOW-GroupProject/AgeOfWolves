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
 * @목적: HUD 구성 요소 중 하나로 사용자가 활용 가능한 Quick Slot을 나타내는 UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API UQuickSlots : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UQuickSlots(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
#pragma endregion

#pragma  region Quick Slot
private:
    void CreateQuickSlots();
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Quick Slot")
        TSubclassOf<UQuickSlot> QuickSlotClass;
    //@3개의 Quick Slot
    UPROPERTY()
        TArray<UQuickSlot*> QuickSlots;
    //@왼쪽 Quick Slot List
    UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
        UVerticalBox* QuickSlotList1;
    const int QuickSlotList1MaxSize = 1;
    //@오른쪽 Quick Slot List
    UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
        UVerticalBox* QuickSlotList2;
    const int QuickSlotList2MaxSize = 2;
#pragma endregion

#pragma region Callbacks
public:
    UFUNCTION()
        void OnRequestItemAssignment(
            int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
        );
    UFUNCTION()
        void OnRequestItemUpdate(
            int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
        );
    UFUNCTION()
        void OnRequestItemRemoval(
            int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
        );
#pragma endregion

};
