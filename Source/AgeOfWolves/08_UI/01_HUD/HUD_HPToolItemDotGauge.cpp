#include "HUD_HPToolItemDotGauge.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "03_Player/BasePlayerController.h"

#include "04_Component/InventoryComponent.h"

#include "Components/EditableTextBox.h"

#include "08_UI/DotGaugeUnit.h"

DEFINE_LOG_CATEGORY(LogHPToolItemDotGauge)

//@Defualt Setting
#pragma region Default Setting
UHUD_HPToolItemDotGauge::UHUD_HPToolItemDotGauge(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    HPToolItemID = FGuid();
}

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

    //@외부 바인딩
    InvenComp->QuickSlotItemsLoaded.AddUFunction(this, "OnQuickSlotItemsLoaded");
    InvenComp->QuickSlotItemUpdated.AddUFunction(this, "OnQuickSlotItemUpdated");
}

void UHUD_HPToolItemDotGauge::InitializeHorizontalDotGauge()
{
    Super::InitializeHorizontalDotGauge();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UHUD_HPToolItemDotGauge::UpdateHPToolItemCount(int32 NewCount)
{
    if (!HPToolItemCountTextBox)
    {
        UE_LOGFMT(LogHPToolItemDotGauge, Error, "HPToolItemCountTextBox가 유효하지 않습니다.");
        return;
    }

    //@Text 포맷팅 ('x5' 형식)
    FString CountText = FString::Printf(TEXT("x%d"), NewCount);
    HPToolItemCountTextBox->SetText(FText::FromString(CountText));

    UE_LOGFMT(LogHPToolItemDotGauge, Log, "HP Tool Item 개수 텍스트가 업데이트되었습니다: {0}", CountText);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UHUD_HPToolItemDotGauge::OnQuickSlotItemsLoaded(int32 QuickSlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
    //@아이템 타입 체크
    if (ItemType != EItemType::Tool)
    {
        return;
    }

    //@HPPotion 태그 체크
    if (!ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Item.Tool.Potion.HP"))))
    {
        return;
    }

    //@FGuid
    HPToolItemID = UniqueItemID;

    //@Max Count 설정
    int32 NewMaxCount = FMath::Clamp(ItemCount, 0, GaugeSettings.MaxCount);
    UpdateMaxCount(NewMaxCount);

    //@텍스트 업데이트
    UpdateHPToolItemCount(ItemCount);

    UE_LOGFMT(LogHPToolItemDotGauge, Log, "퀵슬롯 {0}에 HP 포션 {1}개가 로드되었습니다. Max Count가 {2}로 설정되었습니다.",
        QuickSlotNum, ItemCount, ItemCount);
}

void UHUD_HPToolItemDotGauge::OnQuickSlotItemUpdated(int32 QuickSlotNum, const FGuid& UniqueItemID, int32 ItemCount)
{

    //@FGuid 체크
    if (UniqueItemID != HPToolItemID)
    {
        return;
    }

    //@충전 Dot Gauge Unit 업데이트
    UpdateFilledCount(ItemCount);
    //@텍스트 업데이트
    UpdateHPToolItemCount(ItemCount);

    UE_LOGFMT(LogHPToolItemDotGauge, Log, "퀵슬롯 {0}의 HP 포션이 업데이트되었습니다. 현재 아이템 수: {1}",
        QuickSlotNum, ItemCount);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion