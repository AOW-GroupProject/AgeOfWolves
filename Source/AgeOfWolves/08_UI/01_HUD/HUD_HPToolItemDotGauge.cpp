// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_HPToolItemDotGauge.h"
#include "Logging/StructuredLog.h"

#include "04_Component/InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogHPToolItemDotGauge)

UHUD_HPToolItemDotGauge::UHUD_HPToolItemDotGauge(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UHUD_HPToolItemDotGauge::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ExternalBindToInventoryComp();
}

void UHUD_HPToolItemDotGauge::ExternalBindToInventoryComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogHPToolItemDotGauge, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogHPToolItemDotGauge, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawns
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogHPToolItemDotGauge, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Inven Comp
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogHPToolItemDotGauge, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@External Binding
    InvenComp->ItemAssignedToInventory.AddUFunction(this, "OnItemAssignedToInventory");
    InvenComp->ItemRemovedFromInventory.AddUFunction(this, "OnItemRemovedFromInventory");
    InvenComp->InventoryItemUpdated.AddUFunction(this, "OnInventoryItemUpdated");
}

void UHUD_HPToolItemDotGauge::InitializeHorizontalDotGauge()
{
    Super::InitializeHorizontalDotGauge();
}

void UHUD_HPToolItemDotGauge::OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag)
{
    //@아이템 타입 체크
    if (Type != EItemType::Tool)
    {
        return;
    }

    //@HPPotion 태그 체크
    if (!ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Item.Tool.Potion.HP"))))
    {
        return;
    }

    //@Max Count 증가
    int32 NewMaxCount = FMath::Clamp(CurrentGaugeUnitCount + 1, 0, GaugeSettings.MaxCount);
    UpdateMaxCount(NewMaxCount);

    UE_LOGFMT(LogHPToolItemDotGauge, Log, "HP 포션 아이템이 추가되어 Max Count가 증가했습니다. 현재 Max Count: {0}", NewMaxCount);
}

void UHUD_HPToolItemDotGauge::OnItemRemovedFromInventory(const FGuid& UniqueItemID, EItemType Type)
{
    //@아이템 타입 체크
    if (Type != EItemType::Tool)
    {
        return;
    }

    //@Max Count 감소
    int32 NewMaxCount = FMath::Clamp(CurrentGaugeUnitCount - 1, 0, GaugeSettings.MaxCount);
    UpdateMaxCount(NewMaxCount);

    UE_LOGFMT(LogHPToolItemDotGauge, Log, "HP 포션 아이템이 제거되어 Max Count가 감소했습니다. 현재 Max Count: {0}", NewMaxCount);
}

void UHUD_HPToolItemDotGauge::OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag, int32 UpdatedItemCount)
{
    //@아이템 타입 체크
    if (Type != EItemType::Tool)
    {
        return;
    }

    //@HPPotion 태그 체크
    if (!ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Item.Tool.Potion.HP"))))
    {
        return;
    }

    //@Fill Count 업데이트
    UpdateFilledCount(UpdatedItemCount);

    UE_LOGFMT(LogHPToolItemDotGauge, Log, "HP 포션 아이템이 업데이트되어 Fill Count가 변경되었습니다. 현재 Fill Count: {0}", UpdatedItemCount);
}

