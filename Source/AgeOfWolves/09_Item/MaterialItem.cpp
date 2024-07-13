// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialItem.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "14_Subsystem/ItemManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogMaterialItem)
// UE_LOGFMT(LogToolItem, Warning, "");

AMaterialItem::AMaterialItem(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    ItemType = EItemType::Material;
}

bool AMaterialItem::ActivateItem_Implementation()
{
    if (Super::ActivateItem_Implementation())
    {
        //@활성화 로직 작성
        return true;
    }

	return false;
}

const FMaterialItemInformation* AMaterialItem::GetItemInformation()
{
    //@Game Instance
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        UE_LOGFMT(LogMaterialItem, Error, "GameInstance를 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Manager
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogMaterialItem, Error, "ItemManagerSubsystem을 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Tag
    if (!ItemTag.IsValid())
    {
        UE_LOGFMT(LogMaterialItem, Error, "ItemTag가 유효하지 않습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Info
    const FMaterialItemInformation* ItemInfo = ItemManager->GetItemInformation<FMaterialItemInformation>(EItemType::Material, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogMaterialItem, Error, "ItemInformation을 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }

    return ItemInfo;
}