#include "ItemSlots.h"
#include "Logging/StructuredLog.h"

#include "04_Component/InventoryComponent.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"

#include "08_UI/InteractableItemSlot.h"
#include "08_UI/CustomButton.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlot_DropDownMenu.h"
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

    CurrentHoveredItemSlot = nullptr;
    CurrentSelectedItemSlot = nullptr;
    CurrentSelectedDropDownMenuOptionName = FName();
}

void UItemSlots::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@External Binding
    ExternalBindToInventoryComp();

    //@UUserWidget 제공 가시성 변화 이벤트 바인딩
    OnVisibilityChanged.AddDynamic(this, &UItemSlots::OnUIVisibilityChanged);
}

void UItemSlots::NativePreConstruct()
{
	Super::NativePreConstruct();

    SetIsFocusable(true);

}

void UItemSlots::NativeConstruct()
{
	Super::NativeConstruct();

}

void UItemSlots::NativeDestruct()
{
	Super::NativeDestruct();

}

FNavigationReply UItemSlots::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{

    return FNavigationReply::Explicit(nullptr);
}

FReply UItemSlots::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{

    //@Set Directly(SetFocus())를 통한 포커스 시도 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return FReply::Handled().ClearUserFocus();
    }

    //@Current Selected Item Slot이 있을 경우
    if (CurrentSelectedItemSlot.IsValid())
    {
        //@Selected -> Normal
        CancelItemSlotButton.Broadcast(CurrentSelectedItemSlot->GetUniqueItemID());

        //@Current Selected Item Slot Button 가져오기
        UCustomButton* ItemSlotButton = CurrentSelectedItemSlot->GetItemSlotButton();
        if (!ItemSlotButton)
        {
            UE_LOGFMT(LogItemSlots, Error, "선택된 아이템 슬롯의 버튼을 찾을 수 없습니다.");
            return FReply::Handled().ClearUserFocus();
        }

        //@Normal -> Selected
        if (!ItemSlotButton->SetButtonHoveredByKeyboard())
        {
            UE_LOGFMT(LogItemSlots, Error, "선택된 아이템 슬롯의 버튼을 Hovered 상태로 전환하는데 실패했습니다.");
            return FReply::Handled().ClearUserFocus();
        }
    }

    UE_LOGFMT(LogItemSlots, Log, "포커스 : 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

    return FReply::Handled();
}

void UItemSlots::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{

    //@SetDirectly(SetFocus())를 통한 포커스 소실 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        SetFocus();

        return;
    }

    Super::NativeOnFocusLost(InFocusEvent);

    UE_LOGFMT(LogItemSlots, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));
}

FReply UItemSlots::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogItemSlots, Log, "키 입력 감지됨: {0}", Key.ToString());

    //@방향키 조작
    if (Key == EKeys::Left)
    {
        //@좌로 이동
        HandleDirectionalInput(EUINavigation::Left);
        return FReply::Handled();
    }
    else if (Key == EKeys::Right)
    {
        //@우로 이동
        HandleDirectionalInput(EUINavigation::Right);
        return FReply::Handled();
    }
    else if (Key == EKeys::Up)
    {
        //@현재 호버된 슬롯이 있는지 확인
        if (!CurrentHoveredItemSlot.IsValid())
        {
            UE_LOGFMT(LogItemSlots, Warning, "현재 호버된 아이템 슬롯이 없습니다.");
            return FReply::Unhandled();
        }

        //@현재 호버된 슬롯의 인덱스 찾기
        int32 SlotIndex = ItemSlots.IndexOfByPredicate([this](const UInteractableItemSlot* ItemSlot) {
            return ItemSlot == CurrentHoveredItemSlot.Get();
            });

        if (SlotIndex != INDEX_NONE)
        {
            int32 CurrentRow = 0;
            int32 CurrentColumn = 0;
            GetSlotRowAndColumn(SlotIndex, CurrentRow, CurrentColumn);

            //@첫 번째 Row에 있으면 상위 위젯에서 처리하도록 Unhandled 반환
            if (CurrentRow == 0)
            {
                //@현재 Hovered 상태의 Item Slot의 상태를 Normal로 변경
                OnRequestCancelCurrentHoveredItemSlot(ItemType);

                //@현재 Item Slots의 포커스 소실 요청 이벤트
                RequestCancelItemSlotsFocus.ExecuteIfBound();

                return FReply::Handled();
            }

            //@첫 번째 Row가 아니면 위로 이동
            HandleDirectionalInput(EUINavigation::Up);

            return FReply::Handled();
        }
    }
    else if (Key == EKeys::Down)
    {
        //@아래로 이동
        HandleDirectionalInput(EUINavigation::Down);
        return FReply::Handled();
    }
    else if (Key == EKeys::Enter)
    {
        //@Current Hovered Item Slot을 Select 해줍니다.
        if (!CurrentHoveredItemSlot.IsValid())
        {
            UE_LOGFMT(LogItemSlots, Warning, "현재 호버된 아이템 슬롯이 없습니다.");
            return FReply::Handled();
        }

        //@아이템 슬롯 버튼 가져오기
        UCustomButton* HoveredButton = CurrentHoveredItemSlot->GetItemSlotButton();
        if (!HoveredButton)
        {
            UE_LOGFMT(LogItemSlots, Error, "현재 호버된 아이템 슬롯의 버튼을 찾을 수 없습니다.");
            return FReply::Handled();
        }

        //@버튼 선택 상태로 설정
        bool bSuccess = HoveredButton->SetButtonSelectedByKeyboard();
        if (!bSuccess)
        {
            return FReply::Handled();
        }

        //@성공 로그 출력
        UE_LOGFMT(LogItemSlots, Log, "Enter 키로 아이템 슬롯 선택됨: ID {0}",
            CurrentHoveredItemSlot->GetUniqueItemID().ToString());

        return FReply::Handled();
    }

    UE_LOGFMT(LogItemSlots, Log, "Item Slots UI에서 처리하지 않는 키 입력: {0}", *Key.ToString());

    //@이외 키 입력은 처리되지 않았고, 다른 UI에서 키 입력 처리를 허용합니다.
    return FReply::Unhandled();
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
    ItemSlot->ItemSlotButtonHovered.AddUFunction(this, "OnItemSlotButtonHovered");
    ItemSlot->ItemSlotButtonUnhovered.AddUFunction(this, "OnItemSlotButtonUnhovered");
    //@마지막 Item Slot에만 초기화 완료 이벤트 구독
    if (bLastItemSlot)
        ItemSlot->ItemSlotInitFinished.BindUFunction(this, "OnItemSlotInitFinished");
}

void UItemSlots::InternalBindToItemSlotDropDownMenu(UItemSlot_DropDownMenu* DropDownMenu)
{
    if (!DropDownMenu)
    {
        return;
    }

    //@내부 바인딩
    DropDownMenu->DropDownMenuInitFinished.BindUFunction(this, "OnItemSlotDropDownMenuInitFinished");
    DropDownMenu->DropDownMenuOptionButtonClicked.AddUFunction(this, "OnItemSlotDropDownMenuOptionSelected");

    DropDownMenu->OnVisibilityChanged.AddDynamic(this, &UItemSlots::OnItemSlotDropDownMenuClosed);
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

    UE_LOGFMT(LogDropDownMenu, Log, "Confirmation Menu에 대한 내부 바인딩 완료");
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

        //@초기 상태로 설정
        ResetItemSlots();

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

    //@선택 취소 이벤트 호출
    if (CurrentHoveredItemSlot.IsValid())
    {
        CancelItemSlotButton.Broadcast(CurrentHoveredItemSlot->GetUniqueItemID());
        CurrentHoveredItemSlot = nullptr;
    }

    //@Close Drop Down Menu
    if (ItemSlotDropDownMenu && ItemSlotDropDownMenu->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
    {
        ItemSlotDropDownMenu->CloseDropDownMenu();
    }

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
            UE_LOGFMT(LogItemSlots, Log, "추가 행 {0}에 Spacer 추가 완료", Row + 1);
        }
    }

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
    ItemSlotDropDownMenu = CreateWidget<UItemSlot_DropDownMenu>(this, ItemSlotDropDownMenuClass);
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

    //@아이템 버리기 이벤트
    ItemDiscarded.Broadcast(ItemID, ItemCount);

    return true;
}

void UItemSlots::OpenDropDownMenu(const FVector2D& Position, EInteractionMethod InteractionMethod)
{
    if (!ItemSlotDropDownMenu)
    {
        UE_LOGFMT(LogItemSlots, Error, "ItemSlotDropDownMenu가 유효하지 않습니다.");
        return;
    }

    //@Set Position In Viewport
    ItemSlotDropDownMenu->SetPositionInViewport(Position);

    //@Drop Down Menu 열기
    ItemSlotDropDownMenu->OpenDropDownMenu();

    UE_LOGFMT(LogItemSlots, Log, "DropDownMenu가 열렸습니다. 위치: ({0}, {1}), 상호작용 방식: {2}",
        Position.X, Position.Y, UEnum::GetValueAsString(InteractionMethod));
}

void UItemSlots::HandleDirectionalInput_Implementation(EUINavigation NavigationInput)
{
    if (!CurrentHoveredItemSlot.IsValid())
    {
        UE_LOGFMT(LogItemSlots, Warning, "{0} 타입의 ItemSlots에서 현재 호버된 슬롯이 없습니다. 방향키 입력을 무시합니다.",
            *UEnum::GetValueAsString(ItemType));
        return;
    }

    UE_LOGFMT(LogItemSlots, Log, "{0} 타입의 ItemSlots에서 방향키 입력 감지: {1}",
        *UEnum::GetValueAsString(ItemType),
        *UEnum::GetValueAsString(NavigationInput));

    UE_LOGFMT(LogItemSlots, Log, "현재 호버된 슬롯 ID: {0}",
        CurrentHoveredItemSlot->GetUniqueItemID().ToString());

    int32 CurrentIndex = ItemSlots.IndexOfByPredicate([this](const UInteractableItemSlot* ItemSlot) {
        return ItemSlot == CurrentHoveredItemSlot.Get();
        });

    if (CurrentIndex == INDEX_NONE)
    {
        UE_LOGFMT(LogItemSlots, Error, "현재 호버된 슬롯의 인덱스를 찾을 수 없습니다.");
        return;
    }

    UE_LOGFMT(LogItemSlots, Log, "현재 호버된 슬롯의 인덱스: {0}, ItemSlots 전체 크기: {1}",
        CurrentIndex, ItemSlots.Num());

    int32 NextIndex = CurrentIndex;
    switch (NavigationInput)
    {
    case EUINavigation::Left:
        NextIndex = (CurrentIndex > 0) ? CurrentIndex - 1 : ItemSlots.Num() - 1;
        break;
    case EUINavigation::Right:
        NextIndex = (CurrentIndex < ItemSlots.Num() - 1) ? CurrentIndex + 1 : 0;
        break;
    case EUINavigation::Up:
        NextIndex = (CurrentIndex >= MaxItemSlotsPerRow) ? CurrentIndex - MaxItemSlotsPerRow : CurrentIndex;
        break;
    case EUINavigation::Down:
        NextIndex = (CurrentIndex < ItemSlots.Num() - MaxItemSlotsPerRow) ? CurrentIndex + MaxItemSlotsPerRow : CurrentIndex;
        break;
    }

    UE_LOGFMT(LogItemSlots, Log, "계산된 다음 인덱스: {0}", NextIndex);

    if (ItemSlots.IsValidIndex(NextIndex))
    {
        UE_LOGFMT(LogItemSlots, Log, "다음 인덱스가 유효합니다.");

        UInteractableItemSlot* NextSlot = ItemSlots[NextIndex];
        if (!NextSlot)
        {
            UE_LOGFMT(LogItemSlots, Error, "다음 슬롯이 nullptr입니다. Index: {0}", NextIndex);
            return;
        }

        UE_LOGFMT(LogItemSlots, Log, "다음 슬롯의 ItemID: {0}",
            NextSlot->GetUniqueItemID().IsValid() ? *NextSlot->GetUniqueItemID().ToString() : TEXT("Invalid"));

        UCustomButton* Button = NextSlot->GetItemSlotButton();
        if (!Button)
        {
            UE_LOGFMT(LogItemSlots, Error, "다음 슬롯의 Button이 nullptr입니다. Index: {0}", NextIndex);
            return;
        }

        UE_LOGFMT(LogItemSlots, Log, "다음 슬롯의 Button을 찾았습니다. SetButtonHoveredByKeyboard 시도");

        if (Button->SetButtonHoveredByKeyboard())
        {
            UE_LOGFMT(LogItemSlots, Log, "{0} 타입의 ItemSlots에서 새로운 슬롯이 호버 상태로 설정되었습니다. Index: {1}",
                *UEnum::GetValueAsString(ItemType), NextIndex);
        }
        else
        {
            UE_LOGFMT(LogItemSlots, Error, "SetButtonHoveredByKeyboard 호출 실패. Index: {0}", NextIndex);
        }
    }
    else
    {
        UE_LOGFMT(LogItemSlots, Error, "계산된 다음 인덱스가 유효하지 않습니다. NextIndex: {0}, ItemSlots 크기: {1}",
            NextIndex, ItemSlots.Num());
    }

    UE_LOGFMT(LogItemSlots, Log, "{0} 타입의 ItemSlots에서 방향키 입력 처리 완료",
        *UEnum::GetValueAsString(ItemType));
}
#pragma endregion

//@Callbacks
#pragma region Callback
void UItemSlots::OnUIVisibilityChanged(ESlateVisibility VisibilityType)
{
    if (VisibilityType == ESlateVisibility::Collapsed)
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

void UItemSlots::OnItemSlotButtonHovered(const FGuid& UniqueItemID, EInteractionMethod InteractionMethodType)
{
    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 버튼 호버됨: ID {0}", UniqueItemID.ToString());

    //@Find Slot By FGuid
    UInteractableItemSlot* HoveredItemSlot = FindSlotByItemID(UniqueItemID);
    if (!HoveredItemSlot)
    {
        UE_LOGFMT(LogItemSlots, Error, "호버된 아이템 슬롯을 찾을 수 없습니다: ID {0}", UniqueItemID.ToString());
        return;
    }

    //@Current Hovered Item Slot
    if (CurrentHoveredItemSlot.IsValid() && CurrentHoveredItemSlot->GetUniqueItemID() == UniqueItemID)
    {
        UE_LOGFMT(LogItemSlots, Log, "이미 호버된 아이템 슬롯입니다. 처리를 무시합니다: ID {0}", UniqueItemID.ToString());
        return;
    }

    //@이전 호버 상태 취소
    if (CurrentHoveredItemSlot.IsValid())
    {
        FGuid PreviousItemID = CurrentHoveredItemSlot->GetUniqueItemID();
        CancelItemSlotButton.Broadcast(PreviousItemID);

        UE_LOGFMT(LogItemSlots, Log, "이전에 호버된 아이템 슬롯 호버 취소: ID {0}", PreviousItemID.ToString());
    }

    //@Current Hovered Item Slot 업데이트
    CurrentHoveredItemSlot = HoveredItemSlot;
    UE_LOGFMT(LogItemSlots, Log, "새로운 아이템 슬롯이 호버됨: ID {0}", UniqueItemID.ToString());

    //@SetFocus
    SetFocus();

    // TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 호버된 아이템 슬롯의 시각적 상태 변경, 아이템 정보 표시 등
}

void UItemSlots::OnItemSlotButtonUnhovered(const FGuid& UniqueItemID)
{
    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 버튼 언호버됨: ID {0}", UniqueItemID.ToString());

    //@Current Hovered Item Slot
    if (CurrentHoveredItemSlot.IsValid() && CurrentHoveredItemSlot->GetUniqueItemID() != UniqueItemID)
    {
        UE_LOGFMT(LogItemSlots, Log, "언호버된 아이템 슬롯이 현재 호버된 슬롯과 일치하지 않음: ID {0}", UniqueItemID.ToString());

        return;
    }

    if (CurrentHoveredItemSlot.IsValid() && CurrentHoveredItemSlot->GetUniqueItemID() == UniqueItemID)
    {
        CurrentHoveredItemSlot.Reset();

        UE_LOGFMT(LogItemSlots, Log, "현재 호버된 아이템 슬롯 리셋됨: ID {0}", UniqueItemID.ToString());
    }

}

void UItemSlots::OnItemSlotButtonClicked(const FGuid& UniqueItemID, EInteractionMethod InteractionMethodType)
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

    //@Drop Down Menu의 위치
    FVector2D MenuPosition;

    //@Interaction Method Type
    switch (InteractionMethodType)
    {
        //@Mouse : 마우스 커서 위치에 Drop Down Menu를 위치시킵니다.
        case EInteractionMethod::Mouse:
        {
            //@Player Controller
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                //@Mouse Position
                PlayerController->GetMousePosition(MenuPosition.X, MenuPosition.Y);
            }
            break;
        }
        //@Keyboard : 아이템 슬롯 위치에 Drop Down Menu를 위치시킵니다.
        case EInteractionMethod::Keyboard:
        {
            //@Paint Geometry
            FGeometry Geometry = CurrentSelectedItemSlot->GetPaintSpaceGeometry();
            //@Absolute Position
            FVector2D SlotScreenPosition = Geometry.GetAbsolutePosition();
            //@Local Size
            FVector2D SlotSize = Geometry.GetLocalSize();

            //@아이템 슬롯의 중심 위치 계산
            FVector2D SlotCenter;
            SlotCenter.X = SlotScreenPosition.X + (SlotSize.X / 2.0f);
            SlotCenter.Y = SlotScreenPosition.Y + (SlotSize.Y / 2.0f);

            //@Drop Down Menu 위치 업데이트
            MenuPosition = SlotCenter;

            break;
        }
        default:
        {
            UE_LOGFMT(LogItemSlots, Warning, "알 수 없는 상호작용 방식입니다: {0}", UEnum::GetValueAsString(InteractionMethodType));
            return;
        }
    }

    //@DropDownMenu 열기
    OpenDropDownMenu(MenuPosition, InteractionMethodType);

    // TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 선택된 아이템 슬롯의 시각적 상태 변경, 아이템 정보 표시 등
}

void UItemSlots::OnItemSlotDropDownMenuClosed(ESlateVisibility VisibilityType)
{
    if (VisibilityType != ESlateVisibility::Collapsed)
    {
        return;
    }
    
    UE_LOGFMT(LogItemSlots, Warning, "Drop Down Menu가 닫혔습니다.");

    //@Set Focus
    SetFocus();
}

void UItemSlots::OnItemSlotDropDownMenuOptionSelected(FName ItemSlotDropDownMenuOptionName)
{

    //@CurrentSelectedDropDownMenuOptionName
    CurrentSelectedDropDownMenuOptionName = ItemSlotDropDownMenuOptionName;

    // ItemSlotDropDownMenu와 ConfirmationMenu가 유효한지 확인
    if (!ItemSlotDropDownMenu || !ConfirmationMenu)
    {
        UE_LOGFMT(LogItemSlots, Error, "ItemSlotDropDownMenu 또는 ConfirmationMenu가 유효하지 않습니다.");
        return;
    }

    //@BACK일 경우, 메뉴 닫기
    if (ItemSlotDropDownMenuOptionName == "BACK")
    {
        //@Current Selected Item Slot 유효성 검사
        if (!CurrentSelectedItemSlot.IsValid())
        {
            UE_LOGFMT(LogItemSlots, Error, "선택된 아이템 슬롯이 유효하지 않습니다.");
            return;
        }

        //@Cancel 이벤트 호출
        CancelItemSlotButton.Broadcast(CurrentSelectedItemSlot->GetUniqueItemID());

        //@Current Selected Item Slot Button 가져오기
        UCustomButton* ItemSlotButton = CurrentSelectedItemSlot->GetItemSlotButton();
        if (!ItemSlotButton)
        {
            UE_LOGFMT(LogItemSlots, Error, "선택된 아이템 슬롯의 버튼을 찾을 수 없습니다.");
            return;
        }

        //@아이템 슬롯 버튼 Hover 상태로 전환
        if (ItemSlotButton->SetButtonHoveredByKeyboard())
        {
            UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 버튼이 Hover 상태로 전환되었습니다. ID: {0}",
                CurrentSelectedItemSlot->GetUniqueItemID().ToString());
        }

        //@드롭 다운 메뉴 닫기
        ItemSlotDropDownMenu->CloseDropDownMenu();

        return;
    }

    //@HELP일 경우, 도움 창 열기
    if (ItemSlotDropDownMenuOptionName == "HELP")
    {

        //@TODO: 도움창 제작 후 여기서 열기
        ItemSlotDropDownMenu->CloseDropDownMenu();

        return;
    }

    //@DropDownMenu로부터 ConfirmationMenuDialogueText 가져오기
    FText DialogueText = ItemSlotDropDownMenu->GetConfirmationMenuDialogueText(CurrentSelectedDropDownMenuOptionName);

    //@ConfirmationMenu의 DialogueText 설정
    ConfirmationMenu->SetConfirmationMenuDialogueText(DialogueText);

    //@ConfirmationMenu 열기
    ConfirmationMenu->OpenConfirmationMenu();

    UE_LOGFMT(LogItemSlots, Log, "DropDownMenu 옵션 '{0}'가 선택되었고, ConfirmationMenu가 열렸습니다.", CurrentSelectedDropDownMenuOptionName.ToString());

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
            //@TODO: Item 사용 작업 처리 예정
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

        //@드롭다운 메뉴 현재 선택된 Option의 상태를 Selected -> Hovered로 전환
        ItemSlotDropDownMenu->ResetSelectedOptionToHovered();

        //@드롭다운 메뉴 Set Focus
        ItemSlotDropDownMenu->SetFocus();

        UE_LOGFMT(LogItemSlots, Log, "'{0}' 옵션의 Confirmation Menu가 닫혔습니다.", *CurrentSelectedDropDownMenuOptionName.ToString());
        return;
    }
}

void UItemSlots::OnRequestFirstItemSlotHover(EItemType RequestedItemType)
{
    //@Item Type
    if (ItemType != RequestedItemType)
    {
        return;  
    }

    //@First Item Slot
    UInteractableItemSlot* FirstItemSlot = FindFirstItemSlot();
    if (!FirstItemSlot)
    {
        return;
    }

    //@Button
    UCustomButton* Button = FirstItemSlot->GetItemSlotButton();
    if (!Button)
    {
        return;
    }

    //@Keyboard 조작에 의한 Hover 상태로 전환
    if (Button->SetButtonHoveredByKeyboard())
    {
        UE_LOGFMT(LogItemSlots, Log, "{0} 타입의 첫 번째 Item Slot이 Hover 상태로 설정되었습니다.", *UEnum::GetValueAsString(ItemType));
        return;
    }

    UE_LOGFMT(LogItemSlots, Log, "{0} 타입의 첫 번째 Item Slot이 Hover 상태 설정에 실패했습니다.", *UEnum::GetValueAsString(ItemType));

}

void UItemSlots::OnRequestCancelCurrentHoveredItemSlot(EItemType RequestedItemType)
{
    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 호버 취소 요청: 타입 {0}", *UEnum::GetValueAsString(RequestedItemType));

    //@요청된 아이템 타입이 현재 ItemSlots의 타입과 일치하지 않으면 early return
    if (RequestedItemType != ItemType)
    {
        return;
    }

    //@Current Hovered Item Slot
    if (!CurrentHoveredItemSlot.IsValid())
    {
        UE_LOGFMT(LogItemSlots, Warning, "{0} 타입의 ItemSlots에 현재 Hover된 아이템 슬롯이 없습니다.",
            *UEnum::GetValueAsString(ItemType));
        return;
    }

    //@FGuid
    FGuid CanceledItemID = CurrentHoveredItemSlot->GetUniqueItemID();

    //@취소 이벤트 호출
    CancelItemSlotButton.Broadcast(CanceledItemID);

    UE_LOGFMT(LogItemSlots, Log, "{0} 타입의 ItemSlots에서 현재 Hover된 아이템 슬롯의 선택을 취소했습니다. ID: {1}",
        *UEnum::GetValueAsString(ItemType), CanceledItemID.ToString());

    //@Reset
    CurrentHoveredItemSlot.Reset();

    // TODO: 필요한 경우 여기에 추가 동작 구현
    // 예: 호버 취소된 아이템 슬롯의 시각적 상태 변경, UI 업데이트 등

}

void UItemSlots::OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag, int32 ItemNum)
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
    const FItemInformation* ItemInfoPtr = ItemManager->GetItemInformation<FItemInformation>(Type, ItemTag);
    if (!ItemInfoPtr)
    {
        UE_LOGFMT(LogItemSlots, Error, "아이템 정보를 찾을 수 없습니다: {0}", ItemTag.ToString());
        return;
    }

    // FItemInformation의 복사본 생성
    FItemInformation ItemInfo = *ItemInfoPtr;

    //@Item Images
    UTexture2D* ItemTexture = ItemInfo.ItemSlotImage.LoadSynchronous();
    //@Assign New Item
    EmptySlot->AssignNewItem(UniqueItemID, ItemInfo, ItemNum);
}

void UItemSlots::OnItemRemovedFromInventory(const FGuid& UniqueItemID, EItemType Type)
{
    if (ItemType != Type) return;

    UE_LOGFMT(LogItemSlots, Log, "아이템이 인벤토리에서 제거됨: ID {0}", UniqueItemID.ToString());

    // Find the slot and its position
    UInteractableItemSlot* RemovedItemSlot = FindSlotByItemID(UniqueItemID);
    if (!RemovedItemSlot)
    {
        UE_LOGFMT(LogItemSlots, Warning, "제거할 아이템을 찾을 수 없습니다: ID {0}", UniqueItemID.ToString());
        return;
    }

    // Get removed slot index
    int32 RemovedIndex = ItemSlots.IndexOfByPredicate([&](const UInteractableItemSlot* ItemSlot) {
        return ItemSlot->GetUniqueItemID() == UniqueItemID;
        });

    // Get row and column
    int32 CurrentRow, CurrentCol;
    GetSlotRowAndColumn(RemovedIndex, CurrentRow, CurrentCol);

    if (ShouldPerformSort(CurrentRow, CurrentCol))
    {
        UE_LOGFMT(LogItemSlots, Log, "정렬 작업 시작 - Row: {0}, Col: {1}", CurrentRow, CurrentCol);
        SortItemSlots(RemovedIndex);

        // 정렬 후에는 제거된 슬롯 위치에 새로운 아이템이 있을 것이므로, 그 위치를 호버 상태로 설정
        if (ItemSlots.IsValidIndex(RemovedIndex))
        {
            UInteractableItemSlot* NewSlot = ItemSlots[RemovedIndex];
            if (NewSlot && NewSlot->GetUniqueItemID().IsValid())
            {
                // 현재 선택된 아이템 슬롯 취소
                if (CurrentSelectedItemSlot.IsValid())
                {
                    if (UCustomButton* SelectedButton = CurrentSelectedItemSlot->GetItemSlotButton())
                    {
                        SelectedButton->CancelSelectedButton();

                        CurrentSelectedItemSlot.Reset();
                    }
                }

                // 새로운 슬롯의 버튼을 호버 상태로 설정
                if (UCustomButton* NewButton = NewSlot->GetItemSlotButton())
                {
                    if (NewButton->SetButtonHoveredByKeyboard())
                    {

                    }
                }
            }
        }
    }
    else
    {
        UE_LOGFMT(LogItemSlots, Log, "정렬 불필요 - Row: {0}, Col: {1}", CurrentRow, CurrentCol);

        // 슬롯 비우기
        RemovedItemSlot->ClearAssignedItem();

        // [0,0] 슬롯인 경우
        if (CurrentRow == 0 && CurrentCol == 0)
        {
            RequestCancelItemSlotsFocus.ExecuteIfBound();
            ResetItemSlots();
            return;
        }

        // 그 외의 경우 이전 슬롯을 호버 상태로 설정
        if (ItemSlots.IsValidIndex(RemovedIndex - 1))
        {
            UInteractableItemSlot* PreviousSlot = ItemSlots[RemovedIndex - 1];
            if (PreviousSlot && PreviousSlot->GetUniqueItemID().IsValid())
            {
                if (UCustomButton* Button = PreviousSlot->GetItemSlotButton())
                {
                    Button->SetButtonHoveredByKeyboard();
                }
            }
        }
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

void UItemSlots::SortItemSlots(int32 RemovedIndex)
{
    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 재정렬 시작. 시작 인덱스: {0}", RemovedIndex);

    //@시작 위치의 Row, Column 계산
    int32 CurrentRow, CurrentCol;
    GetSlotRowAndColumn(RemovedIndex, CurrentRow, CurrentCol);

    UE_LOGFMT(LogItemSlots, Log, "재정렬 시작 위치 - Row: {0}, Col: {1}", CurrentRow, CurrentCol);

    //@다음 유효한 아이템을 찾아 현재 위치로 이동
    bool bFoundNext = false;
    do
    {
        bFoundNext = false;

        //@현재 행의 다음 열부터 검사
        for (int32 col = CurrentCol + 1; col < MaxItemSlotsPerRow; ++col)
        {
            const int32 NextIndex = GetSlotIndex(CurrentRow, col);
            if (ItemSlots.IsValidIndex(NextIndex) && ItemSlots[NextIndex]->GetUniqueItemID().IsValid())
            {
                //@아이템 이동
                const int32 ToIndex = GetSlotIndex(CurrentRow, CurrentCol);
                MoveItemSlot(NextIndex, ToIndex);

                CurrentCol = col;
                bFoundNext = true;

                UE_LOGFMT(LogItemSlots, Log, "같은 행에서 아이템 이동 - From: ({0}, {1}) To: ({2}, {3})",
                    CurrentRow, col, CurrentRow, CurrentCol);
                break;
            }
        }

        //@현재 행에서 찾지 못했다면, 다음 행의 첫 열부터 검사
        if (!bFoundNext && CurrentRow < DefaultRows - 1)
        {
            for (int32 row = CurrentRow + 1; row < DefaultRows; ++row)
            {
                for (int32 col = 0; col < MaxItemSlotsPerRow; ++col)
                {
                    const int32 NextIndex = GetSlotIndex(row, col);
                    if (ItemSlots.IsValidIndex(NextIndex) && ItemSlots[NextIndex]->GetUniqueItemID().IsValid())
                    {
                        //@아이템 이동
                        const int32 ToIndex = GetSlotIndex(CurrentRow, CurrentCol);
                        MoveItemSlot(NextIndex, ToIndex);

                        CurrentRow = row;
                        CurrentCol = col;
                        bFoundNext = true;

                        UE_LOGFMT(LogItemSlots, Log, "다음 행에서 아이템 이동 - From: ({0}, {1}) To: ({2}, {3})",
                            row, NextIndex , CurrentRow, CurrentCol);
                        break;
                    }
                }
                if (bFoundNext) break;
            }
        }
    } while (bFoundNext);

    UE_LOGFMT(LogItemSlots, Log, "아이템 슬롯 재정렬 완료");
}

void UItemSlots::MoveItemSlot(int32 FromIndex, int32 ToIndex)
{
    //@Index 유효성 검사
    if (!ItemSlots.IsValidIndex(FromIndex) || !ItemSlots.IsValidIndex(ToIndex))
    {
        UE_LOGFMT(LogItemSlots, Warning, "유효하지 않은 인덱스 - From: {0}, To: {1}", FromIndex, ToIndex);
        return;
    }

    //@Slot 포인터 가져오기
    UInteractableItemSlot* FromSlot = ItemSlots[FromIndex];
    UInteractableItemSlot* ToSlot = ItemSlots[ToIndex];

    //@Slot 유효성 검사
    if (!FromSlot || !ToSlot)
    {
        UE_LOGFMT(LogItemSlots, Error, "유효하지 않은 슬롯 포인터 - FromSlot: {0}, ToSlot: {1}",
            FromSlot ? TEXT("Valid") : TEXT("Invalid"),
            ToSlot ? TEXT("Valid") : TEXT("Invalid"));
        return;
    }

    //@FromSlot ID 저장 (로깅용)
    const FGuid ItemID = FromSlot->GetUniqueItemID();

    //@아이템 복사 및 이동
    ToSlot->AssignNewItemFromSlot(FromSlot);
    FromSlot->ClearAssignedItem();

    UE_LOGFMT(LogItemSlots, Log, "아이템 이동 완료 - From: {0}, To: {1}, ID: {2}",
        FromIndex, ToIndex, ItemID.ToString());
}

bool UItemSlots::ShouldPerformSort(int32 CurrentRow, int32 CurrentCol) const
{
    // 다음 칸에 아이템이 있는 경우
    if (CheckNeedsSortForNextSlot(CurrentRow, CurrentCol))
    {
        return true;
    }

    // 현재 행의 마지막 칸이고, 다음 행의 첫 칸에 아이템이 있는 경우
    return CheckNeedsSortForNextRow(CurrentRow, CurrentCol);
}

bool UItemSlots::CheckNeedsSortForNextSlot(int32 CurrentRow, int32 CurrentCol) const
{
    // 현재 열이 행의 마지막이면 체크하지 않음
    if (CurrentCol >= MaxItemSlotsPerRow - 1) return false;

    // 다음 슬롯의 인덱스 계산
    int32 NextIndex = GetSlotIndex(CurrentRow, CurrentCol + 1);

    // 다음 슬롯에 아이템이 있는지 체크
    bool bHasNextItem = ItemSlots.IsValidIndex(NextIndex) &&
        ItemSlots[NextIndex] &&
        ItemSlots[NextIndex]->GetUniqueItemID().IsValid();

    if (bHasNextItem)
    {
        UE_LOGFMT(LogItemSlots, Log, "다음 칸에 아이템 존재 - Row: {0}, Col: {1}",
            CurrentRow, CurrentCol + 1);
    }

    return bHasNextItem;
}

bool UItemSlots::CheckNeedsSortForNextRow(int32 CurrentRow, int32 CurrentCol) const
{
    // 현재 위치가 행의 마지막이 아니거나 마지막 행이면 체크하지 않음
    if (CurrentCol != MaxItemSlotsPerRow - 1 || CurrentRow >= DefaultRows - 1) return false;

    // 다음 행 첫 번째 슬롯의 인덱스 계산
    int32 NextRowFirstIndex = GetSlotIndex(CurrentRow + 1, 0);

    // 다음 행 첫 번째 슬롯에 아이템이 있는지 체크
    bool bHasNextRowItem = ItemSlots.IsValidIndex(NextRowFirstIndex) &&
        ItemSlots[NextRowFirstIndex] &&
        ItemSlots[NextRowFirstIndex]->GetUniqueItemID().IsValid();

    if (bHasNextRowItem)
    {
        UE_LOGFMT(LogItemSlots, Log, "다음 행 첫 칸에 아이템 존재 - Row: {0}, Col: 0",
            CurrentRow + 1);
    }

    return bHasNextRowItem;
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