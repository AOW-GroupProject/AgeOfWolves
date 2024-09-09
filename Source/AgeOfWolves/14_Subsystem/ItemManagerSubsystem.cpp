// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "Engine/DataTable.h"

DEFINE_LOG_CATEGORY(LogItemManager)
// UE_LOGFMT(LogItemManager, Log, "");

UItemManagerSubsystem::UItemManagerSubsystem()
{}

void UItemManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@Tool Data Table
    ToolItemTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/09_Item/DT_ToolItem"));
    if (ToolItemTable)
    {
        UE_LOGFMT(LogItemManager, Log, "Tool Item 관련 정보 로드 성공");
    }
    else
    {
        UE_LOGFMT(LogItemManager, Error, "Tool Item 데이터 테이블 로드 실패");
    }
    //@Equipment Data Table
    EquipmentItemTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/09_Item/DT_EquipmentItem"));
    if (EquipmentItemTable)
    {
        UE_LOGFMT(LogItemManager, Log, "Equipment Item 관련 정보 로드 성공");
    }
    else
    {
        UE_LOGFMT(LogItemManager, Error, "Equipment Item 데이터 테이블 로드 실패");
    }
    //@Material Data Table
    MaterialItemTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/09_Item/DT_MaterialItem"));
    if (MaterialItemTable)
    {
        UE_LOGFMT(LogItemManager, Log, "Material Item 관련 정보 로드 성공");
    }
    else
    {
        UE_LOGFMT(LogItemManager, Error, "Material Item 데이터 테이블 로드 실패");
    }

}

TArray<TPair<int32, TSubclassOf<AItem>>> UItemManagerSubsystem::GetAllDefaultItems()
{
    TArray<TPair<int32, TSubclassOf<AItem>>> AllItems;

    //@Data Tables
    if (!ToolItemTable || !EquipmentItemTable || !MaterialItemTable)
    {
        UE_LOGFMT(LogItemManager, Error, "Item Data Table을 찾지 못했습니다.");
        return AllItems;
    }
    //@Rows
    TArray<FName> ToolRowNames = ToolItemTable->GetRowNames();
    //@TODO: Equipment Item 관련 Data Table 추가 이후, 아래 코드 주석 해제
    //TArray<FName> EquipmentRowNames = EquipmentItemTable->GetRowNames();
    //@TODO: Material Item 관련 Data Table 작성 이후, 아래 주석 해제
    TArray<FName> MaterialRowNames = MaterialItemTable->GetRowNames();
    if (ToolRowNames.Num() == 0 /*|| EquipmentRowNames.Num() == 0 || MaterialRowNames.Num() == 0*/)
    {
        UE_LOGFMT(LogItemManager, Warning, "Item 데이터 테이블이 비어있습니다!");
        return AllItems;
    }
    //@Default Tool Items
    {
        for (const FName& RowName : ToolRowNames)
        {
            FToolItemInformation* ItemInfo = ToolItemTable->FindRow<FToolItemInformation>(RowName, TEXT(""));
            if (!ItemInfo)
            {
                UE_LOGFMT(LogItemManager, Error, "Failed to find item information for row: {0}", *RowName.ToString());
                continue;
            }
            if (!ItemInfo->bDefault)
            {
                continue;
            }
            int32 ItemCount = FMath::Min(ItemInfo->DefaultGivenStack, ItemInfo->MaxStack);
            AllItems.Add(TPair<int32, TSubclassOf<AItem>>(ItemCount, ItemInfo->ItemClass));
        }
    }
    //@Default Equipment Items
    {
        //for (const FName& RowName : EquipmentRowNames)
        //{
        //    FEquipmentItemInformation* ItemInfo = EquipmentItemTable->FindRow<FEquipmentItemInformation>(RowName, TEXT(""));
        //    if (!ItemInfo)
        //    {
        //        UE_LOGFMT(LogItemManager, Error, "Failed to find item information for row: {0}", *RowName.ToString());
        //        continue;
        //    }
        //    if (!ItemInfo->bDefault)
        //    {
        //        continue;
        //    }
        //    int32 ItemCount = FMath::Min(ItemInfo->DefaultGivenStack, ItemInfo->MaxStack);
        //    AllItems.Add(TPair<int32, TSubclassOf<AItem>>(ItemCount, ItemInfo->Item));
        //}
    }
    //@Default Material Items
    {
        for (const FName& RowName : MaterialRowNames)
        {
            FMaterialItemInformation* ItemInfo = MaterialItemTable->FindRow<FMaterialItemInformation>(RowName, TEXT(""));
            if (!ItemInfo)
            {
                UE_LOGFMT(LogItemManager, Error, "Failed to find item information for row: {0}", *RowName.ToString());
                continue;
            }
            if (!ItemInfo->bDefault)
            {
                continue;
            }
            int32 ItemCount = FMath::Min(ItemInfo->DefaultGivenStack, ItemInfo->MaxStack);
            AllItems.Add(TPair<int32, TSubclassOf<AItem>>(ItemCount, ItemInfo->ItemClass));
        }
    }

    return AllItems;
}