// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlots.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "08_UI/QuickSlot.h"

#include "Kismet/GameplayStatics.h"
#include "04_Component/UIComponent.h"
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

	//@Init
	CreateQuickSlots();

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
#pragma endregion

#pragma  region Quick Slot
void UQuickSlots::CreateQuickSlots()
{
    check(QuickSlotClass);

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
}
#pragma endregion

#pragma region Callbacks
void UQuickSlots::OnRequestItemAssignment(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
    UE_LOGFMT(LogQuickSlots, Log, "OnRequestItemAssignment 호출됨: SlotNum={0}, UniqueItemID={1}, ItemType={2}, ItemTag={3}, ItemCount={4}",
        SlotNum, UniqueItemID.ToString(), StaticEnum<EItemType>()->GetNameStringByValue(static_cast<int64>(ItemType)), ItemTag.ToString(), ItemCount);

    // 여기에 실제 아이템 할당 로직을 구현합니다.
    if (QuickSlots.IsValidIndex(SlotNum - 1))  // SlotNum은 1부터 시작하므로 -1 해줍니다.
    {
        UQuickSlot* QuickSlot = QuickSlots[SlotNum - 1];
        if (QuickSlot)
        {
            // 아이템 정보 업데이트
            // 예: Slot->UpdateSlot(UniqueItemID, ItemTag, ItemCount);
            UE_LOGFMT(LogQuickSlots, Log, "QuickSlot {0}에 아이템이 할당되었습니다.", SlotNum);
        }
        else
        {
            UE_LOGFMT(LogQuickSlots, Error, "SlotNum {0}에 해당하는 QuickSlot이 null입니다.", SlotNum);
        }
    }
    else
    {
        UE_LOGFMT(LogQuickSlots, Error, "유효하지 않은 SlotNum: {0}", SlotNum);
    }
}

void UQuickSlots::OnRequestItemUpdate(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{

    if (QuickSlots.IsValidIndex(SlotNum - 1))  // SlotNum은 1부터 시작하므로 -1 해줍니다.
    {
        UQuickSlot* QuickSlot = QuickSlots[SlotNum - 1];
        UEnum* EnumPtr = StaticEnum<EItemType>();
        //@Quick Slot
        if (QuickSlot)
        {
            //@Game Instance
            UGameInstance* GameInstance = GetGameInstance();
            if (!GameInstance)
            {
                UE_LOGFMT(LogQuickSlots, Error, "OnQuickSlotItemUpdated: GameInstance가 null입니다");
                return;
            }
            //@Item Manager Subsystem
            UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
            if (!ItemManager)
            {
                UE_LOGFMT(LogQuickSlots, Error, "OnQuickSlotItemUpdated: ItemManagerSubsystem이 null입니다");
                return;
            }
            //@FItemInformation
            const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(ItemType, ItemTag);
            if (ItemInfo)
            {
                //@Slot Image
                if (ItemInfo->ItemSlotImage.IsValid())
                {
                    UTexture2D* SlotImage = ItemInfo->ItemSlotImage.LoadSynchronous();
                    QuickSlot->SetSlotImage(SlotImage);
                }
                else
                {
                    UE_LOGFMT(LogQuickSlots, Warning, "OnQuickSlotItemUpdated: 아이템 슬롯 이미지가 없습니다. ItemTag: {0}", ItemTag.ToString());
                }
                //@Item Num - bStackable?
                bool IsStackable = ItemInfo->bStackable && ItemCount > 1;
                QuickSlot->SetIsStackable(IsStackable);

                if (IsStackable)
                {
                    QuickSlot->SetSlotItemNum(static_cast<float>(ItemCount));
                }

                UE_LOGFMT(LogQuickSlots, Log, "퀵슬롯 {0} 업데이트: 아이템 타입: {1}, 아이템 태그: {2}, 아이템 개수: {3}",
                    SlotNum, EnumPtr->GetNameStringByValue(static_cast<int64>(ItemType)), ItemTag.ToString(), ItemCount);
            }
            else
            {
                UE_LOGFMT(LogQuickSlots, Error, "OnQuickSlotItemUpdated: 아이템 정보를 찾을 수 없습니다. ItemType: {0}, ItemTag: {1}",
                    EnumPtr->GetNameStringByValue(static_cast<int64>(ItemType)), ItemTag.ToString());
            }
        }
    }
    else
    {
        UE_LOGFMT(LogQuickSlots, Warning, "OnQuickSlotItemUpdated: 유효하지 않은 슬롯 번호 {0}", SlotNum);
    }

}

void UQuickSlots::OnRequestItemRemoval(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
}
#pragma endregion
