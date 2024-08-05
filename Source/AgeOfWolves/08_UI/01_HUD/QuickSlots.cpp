// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlots.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "08_UI/01_HUD/QuickSlot.h"

#include "04_Component/InventoryComponent.h"
#include "04_Component/UIComponent.h"

#include "Kismet/GameplayStatics.h"
#include "03_Player/BasePlayerController.h"
#include "14_Subsystem/ItemManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogQuickSlots)
// UE_LOGFMT(LogQuickSlots, Log, "");

#pragma region Default Setting
UQuickSlots::UQuickSlots(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	QuickSlotList1 = nullptr;
	QuickSlotList2 = nullptr;
	QuickSlots.Reset();
}

void UQuickSlots::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(QuickSlotClass);

    //@External Binding
    ExternalBindToUIComponent();
    ExternalBindToInventoryComponent();

}

void UQuickSlots::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UQuickSlots::NativeConstruct()
{
	Super::NativeConstruct();

}

void UQuickSlots::NativeDestruct()
{
	Super::NativeDestruct();
}

void UQuickSlots::ExternalBindToInventoryComponent()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogQuickSlots, Error, "QuickSlots::NativeOnInitialized: World is null");
        return;
    }
    //@Player Controller
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogQuickSlots, Error, "QuickSlots::NativeOnInitialized: PlayerController is null");
        return;
    }
    //@Pawn
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogQuickSlots, Error, "Pawn을 찾을 수 없습니다 : {0}", __FUNCTION__);
        return;
    }
    //@Inventory Component
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogQuickSlots, Error, "QuickSlots::NativeOnInitialized: Inventory Component not found on PlayerCharacter");
        return;
    }
    //@Binding
    InvenComp->QuickSlotItemsLoaded.BindUFunction(this, "OnQuickSlotItemsLoaded");
    InvenComp->QuickSlotItemUpdated.BindUFunction(this, "OnQuickSlotItemUpdated");


    UE_LOGFMT(LogQuickSlots, Log, "Inventory Component에 성공적으로 바인딩 되었습니다 : {0}", __FUNCTION__);
}

void UQuickSlots::ExternalBindToUIComponent()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogQuickSlots, Error, "QuickSlots::NativeOnInitialized: World is null");
        return;
    }
    //@Player Controller
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogQuickSlots, Error, "QuickSlots::NativeOnInitialized: PlayerController is null");
        return;
    }
    //@Input Comp
    auto UIComp = PC->FindComponentByClass<UUIComponent>();
    if (!UIComp)
    {
        UE_LOGFMT(LogQuickSlots, Error, "{0}: UI Component가 유효하지 않습니다", __FUNCTION__);
    }
    
    //@Binding
    UIComp->NotifyQuickSlotInputTriggered.BindUFunction(this, "QuickSlotInputTriggeredNotified");
    UIComp->NotifyQuickSlotInputReleased.BindUFunction(this, "QuickSlotInputReleasedNotified");

}

void UQuickSlots::InitializeQuickSlots()
{
    if (!QuickSlotClass->IsValidLowLevel())
    {
        UE_LOGFMT(LogQuickSlots, Error, "유효하지 않은 Quick Slot Class 유형입니다.");
        return;
    }

    //@QuickSlotList1 (1번 슬롯)
    for (int32 i = 0; i < QuickSlotList1MaxSize; ++i)
    {
        UQuickSlot* QuickSlot = CreateWidget<UQuickSlot>(this, QuickSlotClass);
        if (IsValid(QuickSlot))
        {
            //@bStackable
            QuickSlot->SetIsStackable(true);
            if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(QuickSlotList1->AddChildToVerticalBox(QuickSlot)))
            {
                //@Size
                VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                //@Padding
                VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 20.f));
            }
            //@QuickSlots
            QuickSlots.Add(QuickSlot);
        }
    }
    //@QuickSlotList2 (2번, 3번 슬롯)
    for (int32 i = 0; i < QuickSlotList2MaxSize; ++i)
    {
        UQuickSlot* QuickSlot = CreateWidget<UQuickSlot>(this, QuickSlotClass);
        if (IsValid(QuickSlot))
        {
            //@bStackable
            QuickSlot->SetIsStackable(false);
            if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(QuickSlotList2->AddChild(QuickSlot)))
            {
                //@Size
                VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                //@Padding
                if (i < QuickSlotList2MaxSize - 1)
                {
                    VerticalBoxSlot->SetPadding(FMargin(0, 0, 0, 20.f));
                }
            }
            //@Quick Slots
            QuickSlots.Add(QuickSlot);
        }
    }

    UE_LOGFMT(LogQuickSlots, Log, "Quick Slot의 초기화 작업이 완료되었습니다 : {0}", __FUNCTION__);

    //@Delegate
    QuickSlotsInitFinished.ExecuteIfBound();
}
#pragma endregion

#pragma region Quick Slot
void UQuickSlots::StartActivation(const FGameplayTag& InputTag)
{
    //@TODO: Triggered와 Released 각각에 대한 처리
    //@Slot Index
    int32 SlotIndex = -1;
    if (InputTag == FGameplayTag::RequestGameplayTag(FName("Input.UI.HUD.QuickSlot1")))
    {
        SlotIndex = 0;
    }
    else if (InputTag == FGameplayTag::RequestGameplayTag(FName("Input.UI.HUD.QuickSlot2")))
    {
        SlotIndex = 1;
    }
    else if (InputTag == FGameplayTag::RequestGameplayTag(FName("Input.UI.HUD.QuickSlot3")))
    {
        SlotIndex = 2;
    }

    if (SlotIndex != -1 && SlotIndex < QuickSlots.Num())
    {
        //@Quick Slot
        UQuickSlot* TargetQuickSlot = QuickSlots[SlotIndex];
        if (TargetQuickSlot)
        {
            //@FGuid
            FGuid ItemID = TargetQuickSlot->GetUniqueItemID();
            //@Item Count
            int32 CurrentCount = TargetQuickSlot->GetSlotItemNum();
            if (CurrentCount > 0)
            {
                //@TODO: ASC에서 활성화 가능성 여부 검사(공격, 회피, 달리기, 피격 반응, 그로기 반응, 죽음 등)
                
                //@Delegate: Inventory에서 Item 활성화 여부를 먼저 확인하고, UI를 업데이트합니다.
                if (QuickSlotItemActivated.IsBound() && QuickSlotItemActivated.Execute(ItemID, SlotIndex + 1))
                {
                    //@Item Count-1
                    int32 NewCount = CurrentCount - 1;
                    TargetQuickSlot->UpdateItemCount(NewCount);

                    UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 {0} 아이템 사용. 남은 갯수: {1}", SlotIndex + 1, NewCount);
                    //@Remove
                    if (NewCount == 0)
                    {
                        //@bRemovable == true
                        if (TargetQuickSlot->IsRemovable())
                        {
                            TargetQuickSlot->ClearAssignedItem();
                            UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 {0}의 아이템이 모두 소진되어 슬롯이 초기화되었습니다.", SlotIndex + 1);
                        }
                        //bRemovable == false
                        else
                        {
                            UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 {0}의 아이템이 모두 소진되었지만, 제거 불가능한 아이템이므로 슬롯이 유지됩니다.", SlotIndex + 1);
                        }
                    }
                }
            }
            else
            {
                UE_LOGFMT(LogQuickSlots, Warning, "퀵슬롯 {0}의 아이템 갯수가 0입니다. 활성화할 수 없습니다.", SlotIndex + 1);
            }
        }
        else
        {
            UE_LOGFMT(LogQuickSlots, Error, "퀵슬롯 {0}이 유효하지 않습니다.", SlotIndex + 1);
        }
    }
    else
    {
        UE_LOGFMT(LogQuickSlots, Warning, "인식할 수 없는 퀵슬롯 입력입니다: {0}", InputTag.ToString());
    }
}

void UQuickSlots::EndActivation(const FGameplayTag& InputTag)
{
    
}
#pragma endregion

#pragma region Callbacks
void UQuickSlots::OnQuickSlotItemsLoaded(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
    UEnum* ItemTypeEnum = StaticEnum<EItemType>();
    FString ItemTypeString = ItemTypeEnum ? ItemTypeEnum->GetNameStringByValue(static_cast<int64>(ItemType)) : TEXT("Unknown");

    UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 아이템 할당 요청: 슬롯 번호 {0}, 아이템 ID {1}, 아이템 유형 {2}, 아이템 태그 {3}, 아이템 수량 {4}",
        SlotNum, UniqueItemID.ToString(), ItemTypeString, ItemTag.ToString(), ItemCount);

    // 슬롯 번호가 유효한지 확인
    if (SlotNum < 0 || SlotNum >= QuickSlots.Num())
    {
        UE_LOGFMT(LogQuickSlots, Error, "유효하지 않은 퀵슬롯 번호입니다: {0}", SlotNum);
        return;
    }

    // 해당 슬롯의 QuickSlot 위젯 가져오기
    UQuickSlot* TargetQuickSlot = QuickSlots[SlotNum-1];
    if (!TargetQuickSlot)
    {
        UE_LOGFMT(LogQuickSlots, Error, "퀵슬롯 위젯을 찾을 수 없습니다: {0}", SlotNum);
        return;
    }

    // QuickSlot에 아이템 정보 설정
    TargetQuickSlot->SetUniqueItemID(UniqueItemID);

    // ItemManagerSubsystem을 통해 아이템 정보 가져오기
    UItemManagerSubsystem* ItemManager = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    if (ItemManager)
    {
        const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(ItemType, ItemTag);
        if (!ItemInfo)
        {
            UE_LOGFMT(LogQuickSlots, Error, "아이템 정보를 찾을 수 없습니다: {0}", ItemTag.ToString());
            return;
        }

        UTexture2D* ItemTexture = ItemInfo->ItemSlotImage.LoadSynchronous();
        if (!ItemTexture)
        {
            UE_LOGFMT(LogQuickSlots, Warning, "아이템 이미지를 로드할 수 없습니다: {0}", ItemTag.ToString());
        }

        // QuickSlot에 새 아이템 할당
        TargetQuickSlot->AssignNewItem(UniqueItemID, ItemTexture, ItemInfo->bStackable, ItemCount, ItemInfo->bRemovable);

        UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 {0}번에 아이템이 성공적으로 할당되었습니다: {1}", SlotNum, ItemInfo->ItemName.ToString());
    }
}

void UQuickSlots::OnQuickSlotItemUpdated(int32 SlotNum, const FGuid& UniqueItemID, int32 Num)
{
    if (SlotNum <= 0 || SlotNum > QuickSlots.Num())
    {
        UE_LOGFMT(LogQuickSlots, Error, "유효하지 않은 퀵슬롯 번호입니다: {0}", SlotNum);
        return;
    }

    UQuickSlot* TargetQuickSlot = QuickSlots[SlotNum - 1];
    if (!TargetQuickSlot)
    {
        UE_LOGFMT(LogQuickSlots, Error, "퀵슬롯 위젯을 찾을 수 없습니다: {0}", SlotNum);
        return;
    }

    if (TargetQuickSlot->GetUniqueItemID() == UniqueItemID)
    {
        TargetQuickSlot->UpdateItemCount(Num);

        if (Num == 0 && TargetQuickSlot->IsRemovable())
        {
            TargetQuickSlot->ClearAssignedItem();
            UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 {0}번의 아이템이 모두 소진되어 슬롯이 초기화되었습니다.", SlotNum);
        }
    }
    else
    {
        UE_LOGFMT(LogQuickSlots, Warning, "퀵슬롯 {0}번의 아이템 ID가 일치하지 않습니다. 예상: {1}, 실제: {2}",
            SlotNum, UniqueItemID.ToString(), TargetQuickSlot->GetUniqueItemID().ToString());
    }
}

void UQuickSlots::QuickSlotInputTriggeredNotified(const FGameplayTag& InputTag)
{
    StartActivation(InputTag);
}

void UQuickSlots::QuickSlotInputReleasedNotified(const FGameplayTag& InputTag)
{
}
#pragma endregion
