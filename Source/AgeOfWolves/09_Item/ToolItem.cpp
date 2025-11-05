// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolItem.h"

#include "03_Player/PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "14_Subsystem/ItemManagerSubsystem.h"
#include "04_Component/InventoryComponent.h"
#include "08_UI/01_HUD/HUD_StatusUI.h"

DEFINE_LOG_CATEGORY(LogToolItem)
// UE_LOGFMT(LogToolItem, Warning, "");

AToolItem::AToolItem(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    ItemType = EItemType::Tool;
}

bool AToolItem::TryActivateItem_Implementation(UGameInstance* GameInstance)
{
    if (Super::TryActivateItem_Implementation(GameInstance))
    {
        UE_LOGFMT(LogToolItem, Log, "{0}: Tool Item 활성화 시작!", ItemTag.ToString());

        //@World
        UWorld* World = GameInstance->GetWorld();
        if (!World)
        {
            UE_LOGFMT(LogToolItem, Error, "{0}: World is null", __FUNCTION__);
            return false;
        }
        //@PC
        APlayerController* PC = World->GetFirstPlayerController();
        if (!PC)
        {
            UE_LOGFMT(LogToolItem, Error, "{0}: PlayerController is null", __FUNCTION__);
            return false;
        }
        //@Pawns
        APawn* OwningPawn = PC->GetPawn();
        if (!OwningPawn)
        {
            UE_LOGFMT(LogToolItem, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
            return false;
        }
        //@Inven Comp
        UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
        if (!InvenComp)
        {
            UE_LOGFMT(LogToolItem, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
            return false;
        }

        FInventoryItem Item;
        if (InvenComp->GetInventoryItem(GetItemTag(), Item))
        {
            //@아이템 갯수 0 이하 이면 사용 불가
            if (Item.ItemCount <= 0)
            {
                UE_LOGFMT(LogToolItem, Error, "toolItem : {0}가 소유갯수 없습니다 , itemCount : {1}", *GetItemTag().ToString(), Item.ItemCount);
                return false;
            }
        }

        APlayerStateBase* PlayerState = Cast<APlayerStateBase>(PC->PlayerState);

   

        //@ASC
        UAbilitySystemComponent* asc  = PlayerState->GetAbilitySystemComponent();
        if (!asc)
        {
            // UE_LOGFMT(LogStatusUI, Error, "어빌리티 시스템 컴포넌트가 유효하지 않습니다.");
            return false;
        }
        //@AttributeSet
        const UAttributeSet* AttributeSet = asc->GetAttributeSet(UBaseAttributeSet::StaticClass());
        if (!AttributeSet)
        {
            return false;
        }
        //@BaseAttributeSet
        const auto BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
        if (!BaseAttributeSet)
        {
            return false;
        }
        //@FGmaeplayAttribute::Health
        float MaxHealthAttribute = BaseAttributeSet->GetMaxHealth();
        float HealthAttribute = BaseAttributeSet->GetHealth();

        if (HealthAttribute >= MaxHealthAttribute)
            return false;

        UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
        if (!ItemManager)
        {
            UE_LOGFMT(LogToolItem, Error, "ItemManagerSubsystem이 유효하지 않음");
            return false;
        }

        const FToolItemInformation* ItemInfo = ItemManager->GetItemInformation<FToolItemInformation>(EItemType::Tool, ItemTag);
        if (!ItemInfo)
        {
            UE_LOGFMT(LogToolItem, Error, "SpecUpItemInformation 가져오기 실패");
            return false;
        }

        //@ toolItem(물약)은 패시브형 어빌리티로 어빌리티 부여되도록 호출하자 (부여되고 바로 어빌리티 활성됨)
        bool bResult = false;
        bResult = PlayerState->ProcessItemAbilities(ItemInfo->Abilities, ItemTag, true);
        UE_LOGFMT(LogToolItem, Log, "{0}: 어빌리티 영구 등록 요청 (AbilityGrant)", ItemTag.ToString());

        return bResult;
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
