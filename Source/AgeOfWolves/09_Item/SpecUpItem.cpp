#include "SpecUpItem.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "03_Player/PlayerStateBase.h"
#include "14_Subsystem/ItemManagerSubsystem.h"
#include "09_Item/Item.h"

DEFINE_LOG_CATEGORY(LogSpecUpItem)

//@기본 설정
#pragma region Default Setting
ASpecUpItem::ASpecUpItem(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ItemType = EItemType::SpecUp;
}

void ASpecUpItem::BeginPlay()
{
    Super::BeginPlay();
    UE_LOGFMT(LogSpecUpItem, Log, "{0}: SpecUp 아이템 BeginPlay", ItemTag.ToString());
}

void ASpecUpItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOGFMT(LogSpecUpItem, Log, "{0}: SpecUp 아이템 EndPlay", ItemTag.ToString());
    Super::EndPlay(EndPlayReason);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
bool ASpecUpItem::TryActivateItem_Implementation(UGameInstance* GameInstance)
{
    //@GameInstance 유효성 검사
    if (!GameInstance)
    {
        GameInstance = UGameplayStatics::GetGameInstance(this);
        if (!GameInstance)
        {
            UE_LOGFMT(LogSpecUpItem, Error, "{0}: GameInstance 가져오기 실패", ItemTag.ToString());
            return false;
        }
    }

    //@ItemManager 가져오기
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: ItemManagerSubsystem 가져오기 실패", ItemTag.ToString());
        return false;
    }

    //@아이템 정보 가져오기
    const auto* ItemInfo = GetItemInformation(GameInstance);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: ItemInformation 가져오기 실패", ItemTag.ToString());
        return false;
    }

    if (!ItemInfo->bInstantActivation)
    {
        UE_LOGFMT(LogSpecUpItem, Log, "{0}: 즉시 활성화 비활성화됨", ItemTag.ToString());
        return true;
    }

    //@PlayerState 가져오기
    auto* PlayerState = GetPlayerState(GameInstance);
    if (!PlayerState)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: PlayerState 가져오기 실패", ItemTag.ToString());
        return false;
    }

    if (ItemInfo->Abilities.IsEmpty())
    {
        UE_LOGFMT(LogSpecUpItem, Warning, "{0}: 처리할 어빌리티가 없습니다", ItemTag.ToString());
        return false;
    }

    bool bResult = false;
    switch (ItemInfo->SpecUpType)
    {
    case ESpecUpItemType::AbilityGrant:
        bResult = PlayerState->ProcessItemAbilities(ItemInfo->Abilities, ItemTag, ItemInfo->bAllowDuplicate);
        UE_LOGFMT(LogSpecUpItem, Log, "{0}: 어빌리티 영구 등록 요청 (AbilityGrant)", ItemTag.ToString());
        break;

    case ESpecUpItemType::AttributeBoost:
        bResult = PlayerState->ProcessItemAbilityActivation(ItemInfo->Abilities, ItemTag, ItemInfo->bAllowDuplicate);
        UE_LOGFMT(LogSpecUpItem, Log, "{0}: 어빌리티 일회성 활성화 요청 (AttributeBoost)", ItemTag.ToString());
        break;

    default:
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: 알 수 없는 SpecUpType: {1}",
            ItemTag.ToString(), *UEnum::GetValueAsString(ItemInfo->SpecUpType));
        return false;
    }

    //@부모 클래스 활성화 호출 (GameInstance 전달)
    if (!Super::TryActivateItem_Implementation(GameInstance))
    {
        UE_LOGFMT(LogSpecUpItem, Warning, "{0}: 부모 클래스 활성화 실패", ItemTag.ToString());
        return false;
    }

    UE_LOGFMT(LogSpecUpItem, Log, "{0}: SpecUp 처리 완료 (결과: {1})",
        ItemTag.ToString(), bResult ? TEXT("성공") : TEXT("실패"));

    return bResult;
}
#pragma endregion

//@Utility
#pragma region Utility
const FSpecUpItemInformation* ASpecUpItem::GetItemInformation(UGameInstance* GameInstance)
{
    if (!GameInstance)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "GameInstance가 유효하지 않음");
        return nullptr;
    }

    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "ItemManagerSubsystem이 유효하지 않음");
        return nullptr;
    }

    if (!ItemTag.IsValid())
    {
        UE_LOGFMT(LogSpecUpItem, Error, "ItemTag가 유효하지 않음");
        return nullptr;
    }

    const FSpecUpItemInformation* ItemInfo = ItemManager->GetItemInformation<FSpecUpItemInformation>(EItemType::SpecUp, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "SpecUpItemInformation 가져오기 실패");
        return nullptr;
    }

    return ItemInfo;
}

bool ASpecUpItem::IsAbilityGrantType() const
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        return false;
    }

    const FSpecUpItemInformation* ItemInfo = const_cast<ASpecUpItem*>(this)->GetItemInformation(GameInstance);
    return ItemInfo ? ItemInfo->IsAbilityGrantType() : false;
}

bool ASpecUpItem::IsAttributeBoostType() const
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        return false;
    }

    const FSpecUpItemInformation* ItemInfo = const_cast<ASpecUpItem*>(this)->GetItemInformation(GameInstance);
    return ItemInfo ? ItemInfo->IsAttributeBoostType() : false;
}

int32 ASpecUpItem::GetAbilityCount() const
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        return 0;
    }

    const FSpecUpItemInformation* ItemInfo = const_cast<ASpecUpItem*>(this)->GetItemInformation(GameInstance);
    return ItemInfo ? ItemInfo->Abilities.Num() : 0;
}

APlayerStateBase* ASpecUpItem::GetPlayerState(UGameInstance* GameInstance) const
{
    if (!GameInstance)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: GameInstance가 유효하지 않음", ItemTag.ToString());
        return nullptr;
    }

    UWorld* World = GameInstance->GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: World 가져오기 실패", ItemTag.ToString());
        return nullptr;
    }

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: PlayerController 가져오기 실패", ItemTag.ToString());
        return nullptr;
    }

    APlayerStateBase* PS = PC->GetPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogSpecUpItem, Error, "{0}: PlayerState 가져오기 실패", ItemTag.ToString());
        return nullptr;
    }

    return PS;
}
#pragma endregion