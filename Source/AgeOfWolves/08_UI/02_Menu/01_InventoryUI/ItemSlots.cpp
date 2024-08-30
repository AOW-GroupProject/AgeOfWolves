#include "ItemSlots.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseInputComponent.h"
#include "04_Component/InventoryComponent.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/InteractableItemSlot.h"
#include "08_UI/CustomButton.h"

#include "14_Subsystem/ItemManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogItemSlots)

#pragma region Default Setting
UItemSlots::UItemSlots(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UItemSlots::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@External Binding
    ExternalBindToInputComp();
    ExternalBindToInventoryComp();

    //@UUserWidget 제공 가시성 변화 이벤트 바인딩
    OnVisibilityChanged.AddDynamic(this, &UItemSlots::OnUIVisibilityChanged);
}

void UItemSlots::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UItemSlots::NativeConstruct()
{
	Super::NativeConstruct();

}

void UItemSlots::NativeDestruct()
{
	Super::NativeDestruct();

}

void UItemSlots::ExternalBindToInputComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Input Comp
    UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(PC->InputComponent);
    if (!BaseInputComp)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: Input Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@TODO: Binding
    BaseInputComp->UIInputTagTriggered.AddUFunction(this, "OnUIInputTagTriggered");
    BaseInputComp->UIInputTagReleased.AddUFunction(this, "OnUIInputTagReleased");

}

void UItemSlots::ExternalBindToInventoryComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawns
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Inven Comp
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@External Binding
    InvenComp->ItemAssignedToInventory.AddUFunction(this, "OnItemAssignedToInventory");
    InvenComp->ItemRemovedFromInventory.AddUFunction(this, "OnItemRemovedFromInventory");
    InvenComp->InventoryItemUpdated.AddUFunction(this, "OnInventoryItemUpdated");
}

void UItemSlots::InternalBindingToItemSlot(UInteractableItemSlot* ItemSlot, bool bLastItemSlot)
{
    //@Item Slot
    if (!ItemSlot)
    {
        UE_LOGFMT(LogItemSlots, Error, "ItemSlot UI가 유효하지 않습니다.");
        return;
    }

    //@내부 바인딩
    ItemSlot->ItemSlotButtonClicked.AddUFunction(this, "OnItemSlotButtonClicked");
    //@마지막 Item Slot에만 초기화 완료 이벤트 구독
    if (bLastItemSlot)
        ItemSlot->ItemSlotInitFinished.BindUFunction(this, "OnItemSlotInitFinished");


}

void UItemSlots::InitializeItemSlots()
{
	//@Item Slots
	CreateItemSlots();

	//@초기화 요청 이벤트
    RequestStartInitByItemSlots.Broadcast();
}

void UItemSlots::CheckItemSlotInitFinished()
{
    if (bItemSlotReady)
    {
        bItemSlotReady = false;

        //@Item Slots 초기화 완료 이벤트
        ItemSlotsInitFinished.ExecuteIfBound();

        UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 초기화가 완료되었고, CustomButton의 취소 이벤트 바인딩이 수행되었습니다.");
    }
}
#pragma endregion

#pragma region SubWidgets
void UItemSlots::ResetItemSlots()
{
    //@선택 취소 이벤트 호출
    if (CurrentSelectedItemSlot.IsValid())
    {
        CancelItemSlotButton.Broadcast(CurrentSelectedItemSlot->GetUniqueItemID());
        CurrentSelectedItemSlot = nullptr;
    }

    //@First Item Slot
    UInteractableItemSlot* FirstItemSlot = FindFirstItemSlot();
    if (!FirstItemSlot)
    {
        UE_LOGFMT(LogItemSlots, Warning, "UI가 표시되었지만 첫 번째 아이템 슬롯을 찾을 수 없습니다.");
        return;
    }

    //@TODO: First Item Slot을 Hover 상태로 변경해줍니다.
    //FirstItemSlot->ForceButtonState(EItemSlotButtonState::Hovered);

    UE_LOGFMT(LogItemSlots, Log, "{0}의 Item Slot 목록이 초기 상태로 리셋되었습니다.",
        *UEnum::GetValueAsString(ItemType));

}

void UItemSlots::CreateItemSlots()
{
    //@Interactable Item Slot 블루프린트 클래스, Item Slot Box
    if (!ensureMsgf(InteractableItemSlotClass && ItemSlotBox, TEXT("InteractableItemSlotClass 또는 ItemSlots가 유효하지 않습니다.")))
    {
        return;
    }
    //@Clear Children
    ItemSlotBox->ClearChildren();

    int32 TotalSlots = DefaultRows * MaxItemSlotsPerRow;
    int32 CurrentSlot = 0;

    while (CurrentSlot < TotalSlots)
    {
        UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>(this);
        UVerticalBoxSlot* VerticalBoxSlot = ItemSlotBox->AddChildToVerticalBox(HorizontalBox);
        //@Vertical Box Slot
        if (VerticalBoxSlot)
        {
            VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            VerticalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            VerticalBoxSlot->SetVerticalAlignment(VAlign_Fill);
            VerticalBoxSlot->SetPadding(PaddingBetweenRows); // 행 간 간격 추가
        }

        for (int8 SlotIndex = 0; SlotIndex < MaxItemSlotsPerRow && CurrentSlot < TotalSlots; ++SlotIndex, ++CurrentSlot)
        {
            //@Item Slot
            UInteractableItemSlot* ItemSlot = CreateWidget<UInteractableItemSlot>(this, InteractableItemSlotClass);
            if (ItemSlot)
            {
                //@Item Slot의 외부 바인딩
                RequestStartInitByItemSlots.AddUFunction(ItemSlot, "InitializeItemSlot");
                CancelItemSlotButton.AddUFunction(ItemSlot, "OnItemSlotButtonCanceled");

                //@내부 바인딩
                if (CurrentSlot == TotalSlots - 1)
                {
                    InternalBindingToItemSlot(ItemSlot, true);
                }
                else
                {
                    InternalBindingToItemSlot(ItemSlot);
                }

                //@Horizontal Box Slot, Alignment
                UHorizontalBoxSlot* BoxSlot = HorizontalBox->AddChildToHorizontalBox(ItemSlot);
                if (BoxSlot)
                {
                    BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                    BoxSlot->SetHorizontalAlignment(HAlign_Fill);
                    BoxSlot->SetVerticalAlignment(VAlign_Fill);
                    BoxSlot->SetPadding(PaddingBetweenItemSlots);
                }
            }
            else
            {
                UE_LOGFMT(LogItemSlots, Error, "InteractableItemSlot 생성 실패");
            }
        }
    }

    UE_LOGFMT(LogItemSlots, Log, "ItemSlots가 성공적으로 생성되었습니다. 총 슬롯 수: {0}, 행 개수: {1}, 열 개수: {2}",
        TotalSlots, DefaultRows, MaxItemSlotsPerRow);
}
#pragma endregion

#pragma region Callback
void UItemSlots::OnUIVisibilityChanged(ESlateVisibility VisibilityType)
{
    //@SelfHitTestInvisible
    if (VisibilityType == ESlateVisibility::SelfHitTestInvisible)
    {
        //@Reset Item Slots
        ResetItemSlots();
    }
}

void UItemSlots::OnItemSlotInitFinished()
{
    bItemSlotReady = true;
    //@초기화 완료 이벤트 호출
    CheckItemSlotInitFinished();
}

void UItemSlots::OnItemSlotButtonClicked(const FGuid& UniqueItemID)
{
    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 버튼 클릭됨: ID {0}", UniqueItemID.ToString());

    //@CurrentSelectedItemSlot
    if (CurrentSelectedItemSlot.IsValid() && CurrentSelectedItemSlot->GetUniqueItemID() == UniqueItemID)
    {
        UE_LOGFMT(LogItemSlots, Log, "이미 선택된 아이템 슬롯입니다. 처리를 무시합니다: ID {0}", UniqueItemID.ToString());
        return;
    }

    //@선택 취소 이벤트 호출
    if (CurrentSelectedItemSlot.IsValid())
    {
        FGuid PreviousItemID = CurrentSelectedItemSlot->GetUniqueItemID();
        CancelItemSlotButton.Broadcast(PreviousItemID);

        UE_LOGFMT(LogItemSlots, Log, "이전에 선택된 아이템 슬롯 취소: ID {0}", PreviousItemID.ToString());
    }

    //@Find Slot By FGuid
    auto SelectedItemSlot = FindSlotByItemID(UniqueItemID);
    if (!SelectedItemSlot)
    {
        UE_LOGFMT(LogItemSlots, Error, "클릭된 아이템 슬롯을 찾을 수 없습니다: ID {0}", UniqueItemID.ToString());
        return;
    }

    //@Current Selected Item Slot 업데이트
    CurrentSelectedItemSlot = SelectedItemSlot;
    UE_LOGFMT(LogItemSlots, Log, "새로운 아이템 슬롯이 선택됨: ID {0}", UniqueItemID.ToString());

    // TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 선택된 아이템 슬롯의 시각적 상태 변경, 아이템 정보 표시 등
}

void UItemSlots::OnUIInputTagTriggered(const FGameplayTag& InputTag)
{
    UE_LOGFMT(LogItemSlots, Log, "UI 입력 태그 트리거됨: {0}", InputTag.ToString());
    //@TODO: InputTag 가 Input.UI.MenuUI.InventoryUI.~ 일 경우에 대한 처리 아래에서 진행...
}

void UItemSlots::OnUIInputTagReleased(const FGameplayTag& InputTag)
{
    UE_LOGFMT(LogItemSlots, Log, "UI 입력 태그 해제됨: {0}", InputTag.ToString());
    //@TODO: InputTag 가 Input.UI.MenuUI.InventoryUI.~ 일 경우에 대한 처리 아래에서 진행...

}

void UItemSlots::OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag)
{
    UE_LOGFMT(LogItemSlots, Log, "새 아이템이 인벤토리에 할당됨: ID {0}, 유형 {1}, 태그 {2}",
        UniqueItemID.ToString(), UEnum::GetValueAsString(Type), ItemTag.ToString());

    //@Item Type
    if (Type != ItemType)
    {
        return; // 다른 유형의 아이템은 무시
    }
    //@Empty Item Slot
    UInteractableItemSlot* EmptySlot = FindEmptySlot();
    if (!EmptySlot)
    {
        UE_LOGFMT(LogItemSlots, Warning, "빈 슬롯을 찾을 수 없습니다. 아이템을 추가할 수 없습니다.");
        return;
    }
    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogItemSlots, Error, "ItemManagerSubsystem을 찾을 수 없습니다.");
        return;
    }
    //@FItemInformation
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(Type, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogItemSlots, Error, "아이템 정보를 찾을 수 없습니다: {0}", ItemTag.ToString());
        return;
    }
    //@Item Images
    UTexture2D* ItemTexture = ItemInfo->ItemSlotImage.LoadSynchronous();
    //@Assign New Item
    EmptySlot->AssignNewItem(UniqueItemID, ItemTexture, ItemInfo->bStackable, 1, ItemInfo->bRemovable);
    //@Enabled
    EmptySlot->ActivateItemSlotInteraction();
}

void UItemSlots::OnItemRemovedFromInventory(const FGuid& UniqueItemID)
{
    UE_LOGFMT(LogItemSlots, Log, "아이템이 인벤토리에서 제거됨: ID {0}", UniqueItemID.ToString());

    //@Find Slot By Item ID
    UInteractableItemSlot* ItemSlot = FindSlotByItemID(UniqueItemID);
    if (ItemSlot)
    {
        //@Clear Assigned Item
        ItemSlot->ClearAssignedItem();
        //@Deactive Item Slot
        ItemSlot->DeactivateItemSlotInteraction();
    }
    else
    {
        UE_LOGFMT(LogItemSlots, Warning, "제거할 아이템을 찾을 수 없습니다: ID {0}", UniqueItemID.ToString());
    }
}

void UItemSlots::OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag, int32 UpdatedItemCount)
{
    UE_LOGFMT(LogItemSlots, Log, "인벤토리 아이템 업데이트됨: ID {0}, 유형 {1}, 태그 {2}, 새 수량 {3}",
        UniqueItemID.ToString(), UEnum::GetValueAsString(Type), ItemTag.ToString(), UpdatedItemCount);

    //@Item Type
    if (Type != ItemType)
    {
        return; // 다른 유형의 아이템은 무시
    }
    //@Find Slot By Item ID
    UInteractableItemSlot* ItemSlot = FindSlotByItemID(UniqueItemID);
    if (ItemSlot)
    {
        ItemSlot->UpdateItemCount(UpdatedItemCount);
    }
    else
    {
        UE_LOGFMT(LogItemSlots, Warning, "업데이트할 아이템을 찾을 수 없습니다: ID {0}", UniqueItemID.ToString());
    }
}
#pragma endregion

#pragma region Utility Functions
UInteractableItemSlot* UItemSlots::FindFirstItemSlot()
{
    for (auto* Child : ItemSlotBox->GetAllChildren())
    {
        if (UHorizontalBox* HBox = Cast<UHorizontalBox>(Child))
        {
            for (auto* SlotChild : HBox->GetAllChildren())
            {
                if (UInteractableItemSlot* ItemSlot = Cast<UInteractableItemSlot>(SlotChild))
                {
                    return ItemSlot;
                }
            }
        }
    }
    return nullptr;
}

UInteractableItemSlot* UItemSlots::FindEmptySlot()
{
    for (auto* Child : ItemSlotBox->GetAllChildren())
    {
        if (UHorizontalBox* HBox = Cast<UHorizontalBox>(Child))
        {
            for (auto* SlotChild : HBox->GetAllChildren())
            {
                if (UInteractableItemSlot* ItemSlot = Cast<UInteractableItemSlot>(SlotChild))
                {
                    if (!ItemSlot->GetUniqueItemID().IsValid())
                    {
                        return ItemSlot;
                    }
                }
            }
        }
    }
    return nullptr;
}

UInteractableItemSlot* UItemSlots::FindSlotByItemID(const FGuid& ItemID)
{
    for (auto* Child : ItemSlotBox->GetAllChildren())
    {
        if (UHorizontalBox* HBox = Cast<UHorizontalBox>(Child))
        {
            for (auto* SlotChild : HBox->GetAllChildren())
            {
                if (UInteractableItemSlot* ItemSlot = Cast<UInteractableItemSlot>(SlotChild))
                {
                    if (ItemSlot->GetUniqueItemID() == ItemID)
                    {
                        return ItemSlot;
                    }
                }
            }
        }
    }
    return nullptr;
}
#pragma endregion
