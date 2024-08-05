// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "QuickSlots.generated.h"

class UVerticalBox;
class UQuickSlot;
class ABasePlayerController;

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlots, Log, All)

//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FQuickSlotsInitFinished);
//@퀵슬롯 아이템 활성화 이벤트(반환 값)
DECLARE_DELEGATE_RetVal_TwoParams(bool, FQuickSlotItemActivated, const FGuid&, int32);

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
private:
    //@외부 바인딩
    void ExternalBindToInventoryComponent();
    void ExternalBindToUIComponent();
public:
    //@초기화
    UFUNCTION()
        void InitializeQuickSlots();
#pragma endregion

#pragma  region Quick Slot
protected:
    //@HUD 관련 사용자 입력의 Trigger 이벤트 구독
     void StartActivation(const FGameplayTag& InputTag);
    //@HUD 관련 사용자 입력의 Released 이벤트 구독
   void EndActivation(const FGameplayTag& InputTag);
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

#pragma region Delegate
public:
    //@초기화 완료 이벤트
    FQuickSlotsInitFinished QuickSlotsInitFinished;
public:
    //@퀵슬롯 아이템 활성화 이벤트
    FQuickSlotItemActivated QuickSlotItemActivated;
#pragma endregion

#pragma region Callbacks
protected:
    //@새로운 아이템 추가 이벤트 구독
    UFUNCTION()
        void OnQuickSlotItemsLoaded(
            int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount
        );
    //@기존 아이템 업데이트 이벤트 구독
    UFUNCTION()
        void OnQuickSlotItemUpdated(int32 SlotNum, const FGuid& UniqueItemID, int32 Num);
protected:
    UFUNCTION()
        void QuickSlotInputTriggeredNotified(const FGameplayTag& InputTag);
    UFUNCTION()
        void QuickSlotInputReleasedNotified(const FGameplayTag& InputTag);
#pragma endregion

};
