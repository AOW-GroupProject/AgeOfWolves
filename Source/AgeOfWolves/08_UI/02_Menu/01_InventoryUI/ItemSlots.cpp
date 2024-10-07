#include "ItemSlots.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseInputComponent.h"
#include "04_Component/InventoryComponent.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"

#include "08_UI/InteractableItemSlot.h"
#include "08_UI/CustomButton.h"
#include "08_UI/DropDownMenu.h"
#include "08_UI/ConfirmationMenu.h"

#include "14_Subsystem/ItemManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogItemSlots)

//@Default Settings
#pragma region Default Setting
UItemSlots::UItemSlots(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
    ItemSlotDropDownMenu = nullptr;
    ItemSlotDropDownMenuClass = nullptr;
    ConfirmationMenu = nullptr;
    ConfirmationMenuClass = nullptr;

    CurrentSelectedItemSlot = nullptr;
    CurrentSelectedDropDownMenuOptionName = FName();
}

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

void UItemSlots::InternalBindToItemSlotDropDownMenu(UDropDownMenu* DropDownMenu)
{
    if (!DropDownMenu)
    {
        return;
    }

    DropDownMenu->DropDownMenuInitFinished.BindUFunction(this, "OnItemSlotDropDownMenuInitFinished");
    DropDownMenu->DropDownMenuOptionButtonClicked.AddUFunction(this, "OnItemSlotDropDownMenuOptionSelected");
}

void UItemSlots::InternalBindToConfirmationMenu(UConfirmationMenu* Menu)
{
    //@Confirmation Menu
    if (!Menu)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "Confirmation Menu가 null입니다. 바인딩을 수행할 수 없습니다.");
        return;
    }

    //@Confirmation Menu 버튼 클릭 이벤트 바인딩
    Menu->ConfirmationMenuButtonClicked.BindUObject(this, &UItemSlots::OnConfirmationMenuOptionSelected);
    Menu->ConfirmationMenuInitFinished.BindUFunction(this, "OnConfirmationMenuInitFinished");

    UE_LOGFMT(LogDropDownMenu, Verbose, "Confirmation Menu에 대한 내부 바인딩 완료");
}

void UItemSlots::InitializeItemSlots()
{
	//@Item Slots
	CreateItemSlots();
    //@Drop Down Menu
    CreateItemSlotDropDownMenu();
	//@Confirmation Menu
    CreateConfirmationMenu();
    //@초기화 요청 이벤트
    RequestStartInitByItemSlots.Broadcast();
}

void UItemSlots::CheckItemSlotInitFinished()
{
    if (bItemSlotReady && bItemSlotDropDownMenuInitFinished && bConfirmationMenuInitFinished)
    {
        bItemSlotReady = false;
        bItemSlotDropDownMenuInitFinished = false;
        bConfirmationMenuInitFinished = false;

        //@Item Slots 초기화 완료 이벤트
        ItemSlotsInitFinished.ExecuteIfBound();

        UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 초기화가 완료되었고, CustomButton의 취소 이벤트 바인딩이 수행되었습니다.");
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
void UItemSlots::ResetItemSlots()
{
    //@선택 취소 이벤트 호출
    if (CurrentSelectedItemSlot.IsValid())
    {
        CancelItemSlotButton.Broadcast(CurrentSelectedItemSlot->GetUniqueItemID());
        CurrentSelectedItemSlot = nullptr;
    }

    //@Close Drop Down Menu
    if (ItemSlotDropDownMenu && ItemSlotDropDownMenu->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
    {
        ItemSlotDropDownMenu->CloseDropDownMenu();
    }

    //@First Item Slot
    UInteractableItemSlot* FirstItemSlot = FindFirstItemSlot();
    if (!FirstItemSlot)
    {
        UE_LOGFMT(LogItemSlots, Warning, "UI가 표시되었지만 첫 번째 아이템 슬롯을 찾을 수 없습니다.");
        return;
    }

    //@TODO: First Item Slot을 Hover 상태로 강제 상태 변화...

    UE_LOGFMT(LogItemSlots, Log, "{0}의 Item Slot 목록이 초기 상태로 리셋되었습니다.",
        *UEnum::GetValueAsString(ItemType));

}

void UItemSlots::CreateItemSlots()
{
    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 생성 시작");

    //@Interactable Item Slot 블루프린트 클래스, Item Slot Box
    if (!ensureMsgf(InteractableItemSlotClass && ItemSlotBox, TEXT("InteractableItemSlotClass 또는 ItemSlots가 유효하지 않습니다.")))
    {
        UE_LOGFMT(LogItemSlots, Error, "아이템 슬롯 생성 실패: InteractableItemSlotClass 또는 ItemSlotBox가 유효하지 않음");
        return;
    }

    //@Clear Children
    ItemSlotBox->ClearChildren();

    //@Clear ItemSlots array
    ItemSlots.Empty();

    int32 TotalSlots = DefaultRows * MaxItemSlotsPerRow;
    int32 CurrentSlot = 0;

    for (int32 Row = 0; Row < MaxRows; ++Row)
    {
        UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>(this);
        UVerticalBoxSlot* VerticalBoxSlot = ItemSlotBox->AddChildToVerticalBox(HorizontalBox);

        if (VerticalBoxSlot)
        {
            VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
            VerticalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            VerticalBoxSlot->SetVerticalAlignment(VAlign_Fill);
            VerticalBoxSlot->SetPadding(PaddingBetweenRows);
        }

        if (Row < DefaultRows)
        {
            for (int8 SlotIndex = 0; SlotIndex < MaxItemSlotsPerRow && CurrentSlot < TotalSlots; ++SlotIndex, ++CurrentSlot)
            {
                UInteractableItemSlot* ItemSlot = CreateWidget<UInteractableItemSlot>(this, InteractableItemSlotClass);
                if (ItemSlot)
                {
                    RequestStartInitByItemSlots.AddUFunction(ItemSlot, "InitializeItemSlot");
                    CancelItemSlotButton.AddUFunction(ItemSlot, "ItemSlotButtonCanceledNotified");

                    if (CurrentSlot == TotalSlots - 1)
                    {
                        InternalBindingToItemSlot(ItemSlot, true);
                    }
                    else
                    {
                        InternalBindingToItemSlot(ItemSlot);
                    }

                    UHorizontalBoxSlot* BoxSlot = HorizontalBox->AddChildToHorizontalBox(ItemSlot);
                    if (BoxSlot)
                    {
                        FSlateChildSize SlateChildSize;
                        SlateChildSize.SizeRule = ESlateSizeRule::Fill;
                        SlateChildSize.Value = 1.f;
                        //@Size
                        BoxSlot->SetSize(SlateChildSize);
                        //@Alignment
                        BoxSlot->SetHorizontalAlignment(HAlign_Fill);
                        BoxSlot->SetVerticalAlignment(VAlign_Fill);
                        //@Padding
                        BoxSlot->SetPadding(PaddingBetweenItemSlots);
                    }

                    //@Add to ItemSlots array for direct management
                    ItemSlots.Add(ItemSlot);
                }
                else
                {
                    UE_LOGFMT(LogItemSlots, Error, "InteractableItemSlot 생성 실패: 슬롯 {0}", CurrentSlot + 1);
                }
            }
        }
        else
        {
            // DefaultRows 이후의 행에 대해 Spacer 추가
            USpacer* Spacer = NewObject<USpacer>(this);
            if (!Spacer)
            {
                UE_LOGFMT(LogItemSlots, Error, "Spacer 생성 실패!");
                return;
            }
            UVerticalBoxSlot* SpacerSlot = ItemSlotBox->AddChildToVerticalBox(Spacer);
            if (SpacerSlot)
            {
                //@FSlateChildSize
                FSlateChildSize SlateChildSize;
                SlateChildSize.SizeRule = ESlateSizeRule::Fill;
                SlateChildSize.Value = 0.5f;
                //@Size
                SpacerSlot->SetSize(SlateChildSize);
                //@Alignment
                SpacerSlot->SetHorizontalAlignment(HAlign_Fill);
                SpacerSlot->SetVerticalAlignment(VAlign_Fill);
            }
            UE_LOGFMT(LogItemSlots, Verbose, "추가 행 {0}에 Spacer 추가 완료", Row + 1);
        }
    }

    //@초기 상태로 설정
    ResetItemSlots();

    UE_LOGFMT(LogItemSlots, Log, "ItemSlots가 성공적으로 생성되었습니다. 총 슬롯 수: {0}, 기본 행 개수: {1}, 최대 행 개수: {2}, 열 개수: {3}",
        TotalSlots, DefaultRows, MaxRows, MaxItemSlotsPerRow);
}

void UItemSlots::CreateItemSlotDropDownMenu()
{
    //@Drop Down Menu 블루프린트 클래스
    if (!ItemSlotDropDownMenuClass)
    {
        UE_LOGFMT(LogItemSlots, Error, "ItemSlotDropDownMenuClass가 설정되지 않았습니다. 에디터에서 ItemSlotDropDownMenuClass를 설정해주세요.");
        return;
    }
    //@Drop Down Menu
    ItemSlotDropDownMenu = CreateWidget<UDropDownMenu>(this, ItemSlotDropDownMenuClass);
    if (!ItemSlotDropDownMenu)
    {
        UE_LOGFMT(LogItemSlots, Error, "UItemSlotDropDownMenu 위젯을 생성하지 못했습니다. ItemSlotDropDownMenuClass: {0}", *ItemSlotDropDownMenuClass->GetName());
        return;
    }

    //@비동기 초기화
    RequestStartInitByItemSlots.AddUFunction(ItemSlotDropDownMenu, "InitializeDropDownMenu");

    //@내부 바인딩
    InternalBindToItemSlotDropDownMenu(ItemSlotDropDownMenu);

    //@Close Drop Down Menu
    ItemSlotDropDownMenu->AddToViewport(100);

    UE_LOGFMT(LogItemSlots, Log, "ItemSlotDropDownMenu가 생성되었습니다.");
}

void UItemSlots::CreateConfirmationMenu()
{
    //@Confirmation Menu 블루프린트 클래스
    if (!ConfirmationMenuClass)
    {
        UE_LOGFMT(LogItemSlots, Error, "ConfirmationMenuClass가 설정되지 않았습니다. 에디터에서 ConfirmationMenuClass를 설정해주세요.");
        return;
    }

    //@Confirmation Menu
    ConfirmationMenu = CreateWidget<UConfirmationMenu>(this, ConfirmationMenuClass);
    if (!ConfirmationMenu)
    {
        UE_LOGFMT(LogItemSlots, Error, "UConfirmationMenu 위젯을 생성하지 못했습니다. ConfirmationMenuClass: {0}", *ConfirmationMenuClass->GetName());
        return;
    }

    //@비동기 초기화
    RequestStartInitByItemSlots.AddUFunction(ConfirmationMenu, "InitializeConfirmationMenu");

    //@내부 바인딩
    InternalBindToConfirmationMenu(ConfirmationMenu);

    //@Add To Viewport
    ConfirmationMenu->AddToViewport(100);

    UE_LOGFMT(LogItemSlots, Log, "ConfirmationMenu가 생성되었습니다.");
}

bool UItemSlots::UseItem_Implementation(const int32 ItemCount)
{
    //@현재 선택된 아이템 슬롯
    if (!CurrentSelectedItemSlot.IsValid())
    {
        UE_LOGFMT(LogItemSlots, Error, "UseItem: 현재 선택된 아이템 슬롯이 유효하지 않습니다.");
        return false;
    }

    FGuid ItemID = CurrentSelectedItemSlot->GetUniqueItemID();
    int32 CurrentCount = CurrentSelectedItemSlot->GetSlotItemNum();

    //@아이템 갯수 유효성 검사
    if (ItemCount <= 0 || ItemCount > CurrentCount)
    {
        UE_LOGFMT(LogItemSlots, Warning, "UseItem: 유효하지 않은 아이템 개수입니다. 요청된 개수: {0}, 현재 개수: {1}", ItemCount, CurrentCount);
        return false;
    }

    //@TODO: 아이템 사용 로직 구현

    //@아이템 갯수 업데이트
    int32 RemainingCount = CurrentCount - ItemCount;
    CurrentSelectedItemSlot->UpdateItemCount(RemainingCount);

    //@현재 선택된 아이템 슬롯 비울지 여부
    if (RemainingCount <= 0)
    {
        CurrentSelectedItemSlot->ClearAssignedItem();
    }

    UE_LOGFMT(LogItemSlots, Log, "아이템 사용됨: ID {0}, 사용 개수: {1}, 남은 개수: {2}", ItemID.ToString(), ItemCount, RemainingCount);

    //@아이템 사용 이벤트
    ItemUsed.Broadcast(ItemID, ItemCount);

    return true;
}

bool UItemSlots::LeaveItem_Implementation(const int32 ItemCount)
{
    //@현재 선택된 아이템 슬롯 체크
    if (!CurrentSelectedItemSlot.IsValid())
    {
        UE_LOGFMT(LogItemSlots, Error, "LeaveItem: 현재 선택된 아이템 슬롯이 유효하지 않습니다.");
        return false;
    }

    FGuid ItemID = CurrentSelectedItemSlot->GetUniqueItemID();
    int32 CurrentCount = CurrentSelectedItemSlot->GetSlotItemNum();

    //@아이템 갯수 유효성 체크
    if (ItemCount <= 0 || ItemCount > CurrentCount)
    {
        UE_LOGFMT(LogItemSlots, Warning, "LeaveItem: 유효하지 않은 아이템 개수입니다. 요청된 개수: {0}, 현재 개수: {1}", ItemCount, CurrentCount);
        return false;
    }

    //@아이템 갯수 업데이트
    int32 RemainingCount = CurrentCount - ItemCount;
    CurrentSelectedItemSlot->UpdateItemCount(RemainingCount);

    //@Item Slot Clear 여부
    if (RemainingCount <= 0)
    {
        CurrentSelectedItemSlot->ClearAssignedItem();
    }

    UE_LOGFMT(LogItemSlots, Log, "아이템 드롭됨: ID {0}, 드롭 개수: {1}, 남은 개수: {2}", ItemID.ToString(), ItemCount, RemainingCount);

    //@아이템 드롭 이벤트
    ItemLeft.Broadcast(ItemID, ItemCount);

    return true;
}

bool UItemSlots::DiscardItem_Implementation(const int32 ItemCount)
{
    //@현재 선택된 아이템 슬롯
    if (!CurrentSelectedItemSlot.IsValid())
    {
        UE_LOGFMT(LogItemSlots, Error, "DiscardItem: 현재 선택된 아이템 슬롯이 유효하지 않습니다.");
        return false;
    }

    FGuid ItemID = CurrentSelectedItemSlot->GetUniqueItemID();
    int32 CurrentCount = CurrentSelectedItemSlot->GetSlotItemNum();

    //@아이템 갯수 체크
    if (ItemCount <= 0 || ItemCount > CurrentCount)
    {
        UE_LOGFMT(LogItemSlots, Warning, "DiscardItem: 유효하지 않은 아이템 개수입니다. 요청된 개수: {0}, 현재 개수: {1}", ItemCount, CurrentCount);
        return false;
    }

    //@제거 가능 여부
    if (!CurrentSelectedItemSlot->IsRemovable())
    {
        UE_LOGFMT(LogItemSlots, Warning, "DiscardItem: 이 아이템은 버릴 수 없습니다. ID: {0}", ItemID.ToString());
        return false;
    }

    //@TODO: 아이템 버리기 로직 구현

    //@Item Count 업데이트
    int32 RemainingCount = CurrentCount - ItemCount;
    CurrentSelectedItemSlot->UpdateItemCount(RemainingCount);

    if (RemainingCount <= 0)
    {
        CurrentSelectedItemSlot->ClearAssignedItem();
    }

    UE_LOGFMT(LogItemSlots, Log, "아이템 버려짐: ID {0}, 버린 개수: {1}, 남은 개수: {2}", ItemID.ToString(), ItemCount, RemainingCount);

    //@아이템 버리기 이벤트
    ItemDiscarded.Broadcast(ItemID, ItemCount);

    return true;
}
#pragma endregion

//@Callbacks
#pragma region Callback
void UItemSlots::OnUIVisibilityChanged(ESlateVisibility VisibilityType)
{
    //@Reset Item Slots
    ResetItemSlots();
}

void UItemSlots::OnItemSlotInitFinished()
{
    bItemSlotReady = true;
    //@초기화 완료 이벤트 호출
    CheckItemSlotInitFinished();
}

void UItemSlots::OnItemSlotDropDownMenuInitFinished()
{
    bItemSlotDropDownMenuInitFinished = true;

    //@초기화 완료 체크
    CheckItemSlotInitFinished();
}

void UItemSlots::OnConfirmationMenuInitFinished()
{
    bConfirmationMenuInitFinished = true;
    //@초기화 완료 체크
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

    //@Player Controller
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        FVector2D MousePosition;
        //@Mouse Position
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            //@Set Position In Viewport
            ItemSlotDropDownMenu->SetPositionInViewport(MousePosition);
            //@Drop Down Menu 열기
            ItemSlotDropDownMenu->OpenDropDownMenu();
        }
    }

    // TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 선택된 아이템 슬롯의 시각적 상태 변경, 아이템 정보 표시 등
}

void UItemSlots::OnItemSlotDropDownMenuOptionSelected(const FName& ItemSlotDropDownMenuOptionName)
{

    //@CurrentSelectedDropDownMenuOptionName
    CurrentSelectedDropDownMenuOptionName = ItemSlotDropDownMenuOptionName;

    // ItemSlotDropDownMenu와 ConfirmationMenu가 유효한지 확인
    if (!ItemSlotDropDownMenu || !ConfirmationMenu)
    {
        UE_LOG(LogItemSlots, Error, TEXT("ItemSlotDropDownMenu 또는 ConfirmationMenu가 유효하지 않습니다."));
        return;
    }

    //@BACK일 경우, 메뉴 닫기
    if (ItemSlotDropDownMenuOptionName == "BACK")
    {
        //@드롭 다운 메뉴 닫기
        ItemSlotDropDownMenu->CloseDropDownMenu();

        //@아이템 슬롯 전체 리셋
        ResetItemSlots();

        return;
    }

    // DropDownMenu로부터 ConfirmationMenuDialogueText 가져오기
    FText DialogueText = ItemSlotDropDownMenu->GetConfirmationMenuDialogueText(CurrentSelectedDropDownMenuOptionName);

    // ConfirmationMenu의 DialogueText 설정
    ConfirmationMenu->SetConfirmationMenuDialogueText(DialogueText);

    // ConfirmationMenu 열기
    ConfirmationMenu->OpenConfirmationMenu();

    UE_LOG(LogItemSlots, Log, TEXT("DropDownMenu 옵션 '%s'가 선택되었고, ConfirmationMenu가 열렸습니다."), *CurrentSelectedDropDownMenuOptionName.ToString());

    // TODO: Current Selected Item Slot에 대하여 관련 작업 처리
    // 예: 선택된 아이템 슬롯의 상태 업데이트, 추가 로직 실행 등
}

void UItemSlots::OnConfirmationMenuOptionSelected(FName OkOrCancel)
{
    //@Confirmation Menu
    if (!ConfirmationMenu)
    {
        UE_LOGFMT(LogItemSlots, Error, "Confirmation Menu를 찾을 수 없습니다.");
        return;
    }

    if (!ItemSlotDropDownMenu)
    {
        UE_LOGFMT(LogItemSlots, Error, "Drop Down Menu를 찾을 수 없습니다.");
        return;
    }

    //@Ok?
    if (OkOrCancel == "OK")
    {
        UE_LOGFMT(LogItemSlots, Log, "'{0}' 옵션에 대해 'OK'가 선택되었습니다.", *CurrentSelectedDropDownMenuOptionName.ToString());

        //@CurrentSelectedItemSlot 유효성 검사
        if (!CurrentSelectedItemSlot.IsValid())
        {
            UE_LOGFMT(LogItemSlots, Error, "현재 선택된 아이템 슬롯이 유효하지 않습니다.");
            return;
        }

        //@TODO: 확정 메뉴로부터 사용할 갯수도 가져오도록 수정, 우선 1로 고정
        //@아이템 개수 가져오기
        int32 ItemCount = 1;

        //@선택된 옵션에 따라 해당 함수 호출
        if (CurrentSelectedDropDownMenuOptionName == "USE")
        {
            UseItem(ItemCount);
        }
        else if (CurrentSelectedDropDownMenuOptionName == "LEAVE")
        {
            LeaveItem(ItemCount);
        }
        else if (CurrentSelectedDropDownMenuOptionName == "DISCARD")
        {
            DiscardItem(ItemCount);
        }
        else
        {
            UE_LOGFMT(LogItemSlots, Warning, "알 수 없는 옵션이 선택되었습니다: {0}", *CurrentSelectedDropDownMenuOptionName.ToString());
        }

        //@확정 메뉴 닫기
        ConfirmationMenu->CloseConfirmationMenu();

        //@드롭 다운 메뉴 닫기
        ItemSlotDropDownMenu->CloseDropDownMenu();

        return;
    }

    //@Cancel?
    if (OkOrCancel == "CANCEL")
    {
        UE_LOGFMT(LogItemSlots, Log, "'{0}' 옵션에 대해 'CANCEL'이 선택되었습니다.", *CurrentSelectedDropDownMenuOptionName.ToString());

        //@확정 메뉴 닫기
        ConfirmationMenu->CloseConfirmationMenu();

        //@드롭 다운 메뉴 리셋
        ItemSlotDropDownMenu->ResetDropDownMenu();

        UE_LOGFMT(LogItemSlots, Verbose, "'{0}' 옵션의 Confirmation Menu가 닫혔습니다.", *CurrentSelectedDropDownMenuOptionName.ToString());
        return;
    }
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
    EmptySlot->AssignNewItem(UniqueItemID, ItemInfo, 1);
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

//@Utility(Setter, Getter,...etc)
#pragma region Utilities
TArray<UInteractableItemSlot*> UItemSlots::GetAllItemSlots() const
{
    return ItemSlots;
}

int32 UItemSlots::GetSlotIndex(int32 Row, int32 Column) const
{
    return Row * MaxItemSlotsPerRow + Column;
}

void UItemSlots::GetSlotRowAndColumn(int32 Index, int32& OutRow, int32& OutColumn) const
{
    OutRow = Index / MaxItemSlotsPerRow;
    OutColumn = Index % MaxItemSlotsPerRow;
}

UInteractableItemSlot* UItemSlots::GetSlotAtPosition(int32 Row, int32 Column) const
{
    int32 Index = GetSlotIndex(Row, Column);
    if (ItemSlots.IsValidIndex(Index))
    {
        return ItemSlots[Index];
    }
    return nullptr;
}

UInteractableItemSlot* UItemSlots::FindFirstItemSlot()
{
    return ItemSlots.IsValidIndex(0) ? ItemSlots[0] : nullptr;
}

UInteractableItemSlot* UItemSlots::FindEmptySlot()
{
    auto* FoundSlot = ItemSlots.FindByPredicate([](const UInteractableItemSlot* ItemSlot) {
        return !ItemSlot->GetUniqueItemID().IsValid();
        });
    return FoundSlot ? *FoundSlot : nullptr;
}

UInteractableItemSlot* UItemSlots::FindSlotByItemID(const FGuid& ItemID)
{
    auto* FoundSlot = ItemSlots.FindByPredicate([ItemID](const UInteractableItemSlot* ItemSlot) {
        return ItemSlot->GetUniqueItemID() == ItemID;
        });
    return FoundSlot ? *FoundSlot : nullptr;
}
#pragma endregion

