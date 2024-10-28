// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolItem.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "14_Subsystem/ItemManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogToolItem)
// UE_LOGFMT(LogToolItem, Warning, "");

AToolItem::AToolItem(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    ItemType = EItemType::Tool;
}

bool AToolItem::TryActivateItem_Implementation()
{
    if (Super::TryActivateItem_Implementation())
    {
        UE_LOGFMT(LogToolItem, Log, "{0}: Tool Item 활성화 시작!", ItemTag.ToString());

        //@GA활성화 로직 작성
        return true;
    }

    return false;
}

const FToolItemInformation* AToolItem::GetItemInformation()
{
    //@Game Instance
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        UE_LOGFMT(LogToolItem, Error, "GameInstance를 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@ItemManager
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogToolItem, Error, "ItemManagerSubsystem을 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Tag
    if (!ItemTag.IsValid())
    {
        UE_LOGFMT(LogToolItem, Error, "ItemTag가 유효하지 않습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }
    //@Item Info
    const FToolItemInformation* ItemInfo = ItemManager->GetItemInformation<FToolItemInformation>(EItemType::Tool, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogToolItem, Error, "ItemInformation을 가져올 수 없습니다: {0} {1}", __FUNCTION__, __LINE__);
        return nullptr;
    }

    return ItemInfo;
}
