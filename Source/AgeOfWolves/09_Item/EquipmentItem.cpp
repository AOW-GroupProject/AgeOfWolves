// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentItem.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "14_Subsystem/ItemManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogEquipmentItem)
// UE_LOGFMT(LogToolItem, Warning, "");

AEquipmentItem::AEquipmentItem(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
    ItemType = EItemType::Equipment;
}

bool AEquipmentItem::ActivateItem_Implementation()
{
	if (Super::ActivateItem_Implementation())
	{
		//@활성화 로직 작성
		return true;
	}

	return false;
}


const FEquipmentItemInformation* AEquipmentItem::GetItemInformation()
{
    //@Game Instance
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        UE_LOGFMT(LogEquipmentItem, Error, "GameInstance를 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogEquipmentItem, Error, "ItemManagerSubsystem을 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Tag
    if (!ItemTag.IsValid())
    {
        UE_LOGFMT(LogEquipmentItem, Error, "ItemTag가 유효하지 않습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Info
    const FEquipmentItemInformation* ItemInfo = ItemManager->GetItemInformation<FEquipmentItemInformation>(EItemType::Equipment, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogEquipmentItem, Error, "ItemInformation을 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }

    return ItemInfo;
}