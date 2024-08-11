#include "InventoryUI.h"
#include "Logging/StructuredLog.h"

#include "04_Component/InventoryComponent.h"
#include "04_Component/BaseInputComponent.h"

#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemDescription.h"

#include "Components/ScaleBox.h"
#include "Components/ScaleBoxSlot.h"

DEFINE_LOG_CATEGORY(LogInventoryUI)

#pragma region Default Setting
UInventoryUI::UInventoryUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UInventoryUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@외부 바인딩
    ExternalBindingToInventoryComp();
    ExternalBindingToInputComp();
}

void UInventoryUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UInventoryUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void UInventoryUI::ExternalBindingToInventoryComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawns
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Inven Comp
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    
    //@External Binding
    InvenComp->NewItemAssignedToInventory.AddUFunction(this, "OnItemAssignedToInventory");
    InvenComp->ItemRemovedFromInventory.AddUFunction(this, "OnItemRemovedFromInventory");
    InvenComp->InventoryItemUpdated.AddUFunction(this, "OnInventoryItemUpdated");
}

void UInventoryUI::ExternalBindingToInputComp()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawn
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Input Comp
    UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(PC->InputComponent);
    if (!BaseInputComp)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Input Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@TODO: Binding
    BaseInputComp->UIInputTagTriggered.AddUFunction(this, "OnUIInputTagTriggered");
    BaseInputComp->UIInputTagReleased.AddUFunction(this, "OnUIInputTagReleased");
}

void UInventoryUI::InternalBindingToItemSlots(UItemSlots* ItemSlots)
{
    //@Item Slots
    if (!ItemSlots)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemSlots UI가 유효하지 않습니다.");
        return;
    }

    ItemSlots->ItemSlotsInitFinished.BindUFunction(this, "OnItemSlotsInitFinished");
}

void UInventoryUI::InternalBindingToItemDescription(UItemDescription* ItemDescription)
{
    //@Item Description
    if (!ItemDescription)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemDescription UI가 유효하지 않습니다.");
        return;
    }

    ItemDescription->ItemDescriptionInitFinished.BindUFunction(this, "OnItemDescriptionInitFinished");
}

void UInventoryUI::InitializeInventoryUI()
{
    //@Item Slots
    CreateItemSlots();
    //@Item Descrption
    CreateItemDescription();

    //@초기화 완료 이벤트 호출
    InventoryUIInitFinished.ExecuteIfBound();
}

void UInventoryUI::CheckInventoryUIInitialization()
{
    if (bItemSlotsInitialized && bItemDescriptionInitialized)
    {
        UE_LOGFMT(LogInventoryUI, Log, "InventoryUI의 모든 서브위젯 초기화가 완료되었습니다.");

        //@Delegate
        InventoryUIInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

#pragma region SubWidgets
void UInventoryUI::CreateItemSlots()
{
    if (!ItemSlotsBox)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemSlotsBox가 유효하지 않습니다.");
        return;
    }

    if (!ensureMsgf(ItemSlotsClass, TEXT("ItemSlotsClass가 설정되지 않았습니다.")))
    {
        return;
    }

    UItemSlots* ItemSlots = CreateWidget<UItemSlots>(this, ItemSlotsClass);
    if (ItemSlots)
    {
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(ItemSlotsBox->AddChild(ItemSlots));
        if (ScaleBoxSlot)
        {
            ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }

        RequestStartInitByInventoryUI.AddUFunction(ItemSlots, "InitializeItemSlots");
        InternalBindingToItemSlots(ItemSlots);

        UE_LOGFMT(LogInventoryUI, Log, "ItemSlots 위젯이 성공적으로 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemSlots 위젯 생성에 실패했습니다.");
    }
}

void UInventoryUI::CreateItemDescription()
{
    if (!ItemDescriptionBox)
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemDescriptionBox가 유효하지 않습니다.");
        return;
    }

    if (!ensureMsgf(ItemDescriptionClass, TEXT("ItemDescriptionClass가 설정되지 않았습니다.")))
    {
        return;
    }

    UItemDescription* ItemDescription = CreateWidget<UItemDescription>(this, ItemDescriptionClass);
    if (ItemDescription)
    {
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(ItemDescriptionBox->AddChild(ItemDescription));
        if (ScaleBoxSlot)
        {
            ScaleBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            ScaleBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }

        RequestStartInitByInventoryUI.AddUFunction(ItemDescription, "InitializeItemDescription");
        InternalBindingToItemDescription(ItemDescription);

        UE_LOGFMT(LogInventoryUI, Log, "ItemDescription 위젯이 성공적으로 추가되었습니다.");
    }
    else
    {
        UE_LOGFMT(LogInventoryUI, Error, "ItemDescription 위젯 생성에 실패했습니다.");
    }
}
#pragma endregion

#pragma region Callbacks
void UInventoryUI::OnItemSlotsInitFinished()
{
    UE_LOGFMT(LogInventoryUI, Log, "ItemSlots 초기화가 완료되었습니다.");

    //@Init Finished
    bItemSlotsInitialized = true;
    //@Inventory UI의 초기화 완료 여부 체크
    CheckInventoryUIInitialization();
}

void UInventoryUI::OnItemDescriptionInitFinished()
{
    UE_LOGFMT(LogInventoryUI, Log, "ItemDescription 초기화가 완료되었습니다.");

    //@Init Finished
    bItemDescriptionInitialized = true;
    //@Inventory UI의 초기화 완료 여부 체크
    CheckInventoryUIInitialization();
}

void UInventoryUI::OnUIInputTagTriggered(const FGameplayTag& InputTag)
{
    //UE_LOGFMT(LogInventoryUI, Log, "UI 입력 태그 트리거됨: {0}", InputTag.ToString());
    //@TODO: 입력에 대한 처리
}

void UInventoryUI::OnUIInputTagReleased(const FGameplayTag& InputTag)
{
    //UE_LOGFMT(LogInventoryUI, Log, "UI 입력 태그 해제됨: {0}", InputTag.ToString());
    //@TODO: 입력에 대한 처리
}

void UInventoryUI::OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag)
{
    UE_LOGFMT(LogInventoryUI, Log, "새 아이템이 인벤토리에 할당됨: ID {0}, 유형 {1}, 태그 {2}",
        UniqueItemID.ToString(), UEnum::GetValueAsString(ItemType), ItemTag.ToString());
}

void UInventoryUI::OnItemRemovedFromInventory(const FGuid& UniqueItemID)
{
    UE_LOGFMT(LogInventoryUI, Log, "아이템이 인벤토리에서 제거됨: ID {0}", UniqueItemID.ToString());
}

void UInventoryUI::OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 UpdatedItemCount)
{
    UE_LOGFMT(LogInventoryUI, Log, "인벤토리 아이템 업데이트됨: ID {0}, 유형 {1}, 태그 {2}, 새 수량 {3}",
        UniqueItemID.ToString(), UEnum::GetValueAsString(ItemType), ItemTag.ToString(), UpdatedItemCount);
}
#pragma endregion