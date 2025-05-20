// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "09_Item/Item.h"

#include "ItemManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemManager, Log, All)


//@전방 선언
#pragma region Forward Declaration
class UDataTable;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *  @UItemManagerSubsystem
 * 
 *  Item 리소스 관리를 수행하는 Subsystem 정의
 */
UCLASS()
class AGEOFWOLVES_API UItemManagerSubsystem : public UGameInstanceSubsystem
{
    //@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

        //@Defualt Setting
#pragma region Default Setting
public:
    UItemManagerSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@Tool Item 정보를 담은 Data Table
    UPROPERTY()
        TObjectPtr<UDataTable> ToolItemTable = nullptr;
    //@Equipment Item 정보를 담은 Data Table
    UPROPERTY()
        TObjectPtr<UDataTable> EquipmentItemTable = nullptr;
    //@Material Item 정보를 담은 Data Table
    UPROPERTY()
        TObjectPtr<UDataTable> MaterialItemTable = nullptr;

public:
    //@Default Item 들을 반환합니다.(아이템 갯수, 아이템 클래스)
    TArray<TPair<int32, TSubclassOf<AItem>>> GetAllDefaultItems();
    //@특정 Item에 대응되는 Item Information을 반환합니다.
    template<typename StructType>
    const StructType* GetItemInformation(const EItemType ItemType, const FGameplayTag& ItemTag) const
    {
        UDataTable* TargetTable = nullptr;

        switch (ItemType)
        {
        case EItemType::Tool:
            TargetTable = ToolItemTable;
            break;
        case EItemType::Equipment:
            TargetTable = EquipmentItemTable;
            break;
        case EItemType::Material:
            TargetTable = MaterialItemTable;
            break;
        default:
            UE_LOG(LogItemManager, Warning, TEXT("불분명한 ItemType입니다."));
            return nullptr;
        }

        if (!TargetTable)
        {
            UE_LOG(LogItemManager, Warning, TEXT("해당 Item Type %d 에 대응되는 Data table이 존재하지 않습니다."), static_cast<int32>(ItemType));
            return nullptr;
        }

        const StructType* RowData = TargetTable->FindRow<StructType>(ItemTag.GetTagName(), TEXT(""));
        if (!RowData)
        {
            UE_LOG(LogItemManager, Warning, TEXT("Item information not found for ItemTag: %s in ItemType: %d"),
                *ItemTag.ToString(), static_cast<int32>(ItemType));
        }

        return RowData;
    }
#pragma endregion


};