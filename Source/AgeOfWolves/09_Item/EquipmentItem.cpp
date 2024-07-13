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
		//@Ȱ��ȭ ���� �ۼ�
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
        UE_LOGFMT(LogEquipmentItem, Error, "GameInstance�� ������ �� �����ϴ�: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogEquipmentItem, Error, "ItemManagerSubsystem�� ������ �� �����ϴ�: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Tag
    if (!ItemTag.IsValid())
    {
        UE_LOGFMT(LogEquipmentItem, Error, "ItemTag�� ��ȿ���� �ʽ��ϴ�: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Info
    const FEquipmentItemInformation* ItemInfo = ItemManager->GetItemInformation<FEquipmentItemInformation>(EItemType::Equipment, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogEquipmentItem, Error, "ItemInformation�� ������ �� �����ϴ�: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }

    return ItemInfo;
}