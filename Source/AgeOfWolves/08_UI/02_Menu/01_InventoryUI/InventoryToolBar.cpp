#include "InventoryToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogInventoryToolBar)

//@Defualt Setting
#pragma region Default Setting
UInventoryToolBar::UInventoryToolBar(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UInventoryToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UInventoryToolBar::NativePreConstruct()
{
    Super::NativePreConstruct();

    SetIsFocusable(false);
}

void UInventoryToolBar::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryToolBar::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UInventoryToolBar::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);

}

void UInventoryToolBar::InternalBindToButton(UCustomButton* Button, EItemType ItemType)
{
    if (!Button)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Button이 유효하지 않습니다.");
        return;
    }

    Button->ButtonSelected.AddUObject(this, &UInventoryToolBar::OnInventoryToolBarButtonClicked, ItemType);
    Button->ButtonHovered.AddUObject(this, &UInventoryToolBar::OnInventoryToolBarButtonHovered, ItemType);
    Button->ButtonUnhovered.AddUObject(this, &UInventoryToolBar::OnInventoryToolBarButtonUnhovered, ItemType);
}

void UInventoryToolBar::InitializeInventoryToolBar()
{
    //@Create Buttons
    CreateButtons();

    //@초기화 완료 이벤트
    InventoryToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion

//@Property/Info...etc
#pragma region Widgets
void UInventoryToolBar::ResetToolBar()
{
    EItemType PreviousType = CurrentSelectedItemType;
    CurrentSelectedItemType = DefaultItemType;

    if (PreviousType < EItemType::MAX && PreviousType != DefaultItemType)
    {
        CancelInventoryToolBarButtonSelected(PreviousType);
    }

    UCustomButton* DefaultButton = MItemTypeButtons[CurrentSelectedItemType];
    if (!DefaultButton)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Default 아이템 타입 버튼을 찾을 수 없습니다. 초기화에 실패했을 수 있습니다.");
        return;
    }

    DefaultButton->SetButtonState(EButtonState::Selected);

    InventoryToolBarButtonClicked.ExecuteIfBound(CurrentSelectedItemType);

    UE_LOGFMT(LogInventoryToolBar, Log, "Inventory Tool Bar가 초기 상태로 리셋되었습니다.");
}

void UInventoryToolBar::CreateButtons()
{
    if (!ItemTypeButtonBox)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "ItemTypeButtonBox가 유효하지 않습니다.");
        return;
    }

    ItemTypeButtonBox->ClearChildren();
    MItemTypeButtons.Empty();

    //@Item Type별 버튼 생성
    CreateAndAddButton(EItemType::Tool, 0.5f);
    CreateAndAddButton(EItemType::Material, 1.0f);
    CreateAndAddButton(EItemType::Memory, 0.5f);
    CreateAndAddButton(EItemType::Equipment, 0.8f);

    //@Spacer
    USpacer* Spacer = NewObject<USpacer>(this);
    if (!Spacer)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Spacer 생성 실패!");
        return;
    }
    //@HorizontalSlot
    UHorizontalBoxSlot* HorizontalSlot = ItemTypeButtonBox->AddChildToHorizontalBox(Spacer);
    if (!HorizontalSlot)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Spacer를 Horizontal Box에 추가하는데 실패했습니다.");
        return;
    }

    //@FSlateChildSize
    FSlateChildSize SlateChildSize;
    SlateChildSize.SizeRule = ESlateSizeRule::Fill;
    SlateChildSize.Value = 2.f;
    //@Size
    HorizontalSlot->SetSize(SlateChildSize);
    //@Alignment
    HorizontalSlot->SetHorizontalAlignment(HAlign_Fill);
    HorizontalSlot->SetVerticalAlignment(VAlign_Fill);

    //@초기 상태로 설정
    ResetToolBar();

    UE_LOGFMT(LogInventoryToolBar, Log, "모든 버튼과 Spacer가 생성되고 추가되었습니다.");
}

void UInventoryToolBar::CreateAndAddButton(EItemType ButtonType, float Scale)
{
    TSubclassOf<UCustomButton> ItemSlotButtonClass;
    switch (ButtonType)
    {
    case EItemType::Tool:
        ItemSlotButtonClass = ToolTypeButtonClass;
        break;
    case EItemType::Material:
        ItemSlotButtonClass = MaterialTypeButtonClass;
        break;
    case EItemType::Memory:
        ItemSlotButtonClass = MemoryTypeButtonClass;
        break;
    case EItemType::Equipment:
        ItemSlotButtonClass = EquipmentTypeButtonClass;
        break;
    default:
        UE_LOGFMT(LogInventoryToolBar, Error, "알 수 없는 버튼 타입: {0}", UEnum::GetValueAsString(ButtonType));
        return;
    }

    if (!ItemSlotButtonClass)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "{0} 타입의 버튼 클래스가 설정되지 않았습니다.", UEnum::GetValueAsString(ButtonType));
        return;
    }

    //@CustomButton
    UCustomButton* NewButton = CreateWidget<UCustomButton>(this, ItemSlotButtonClass);
    if (!NewButton)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "CustomButton 생성 실패: {0}", UEnum::GetValueAsString(ButtonType));
        return;
    }

    //@내부 바인딩
    InternalBindToButton(NewButton, ButtonType);

    //@HorizontalSlot
    UHorizontalBoxSlot* HorizontalSlot = ItemTypeButtonBox->AddChildToHorizontalBox(NewButton);
    if (!HorizontalSlot)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "{0} 버튼을 Horizontal Box에 추가하는데 실패했습니다.", UEnum::GetValueAsString(ButtonType));
        return;
    }

    //@FSlateChildSize
    FSlateChildSize SlateChildSize;
    SlateChildSize.SizeRule = ESlateSizeRule::Fill;
    SlateChildSize.Value = Scale;

    //@Size
    HorizontalSlot->SetSize(SlateChildSize);

    //@Alignment
    HorizontalSlot->SetHorizontalAlignment(HAlign_Fill);
    HorizontalSlot->SetVerticalAlignment(VAlign_Fill);
    //@TMap
    MItemTypeButtons.Add(ButtonType, NewButton);

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 생성되고 추가되었습니다. Scale: {1}", UEnum::GetValueAsString(ButtonType), Scale);
}

void UInventoryToolBar::MoveLeft()
{
    MoveSelection(-1);
}

void UInventoryToolBar::MoveRight()
{
    MoveSelection(1);
}

void UInventoryToolBar::MoveSelection(int32 Direction)
{
    //@Item Types
    TArray<EItemType> ItemTypes;
    MItemTypeButtons.GetKeys(ItemTypes);

    //@Current Item Type Button의 인덱스
    int32 CurrentIndex = GetCurrentButtonIndex();
    int32 NewIndex;

    //@오른쪽 방향키 눌림
    if (Direction > 0)
    {
        NewIndex = (CurrentIndex + 1) % ItemTypes.Num();
    }
    //@왼쪽 방향키 눌림
    else
    {
        NewIndex = (CurrentIndex - 1 + ItemTypes.Num()) % ItemTypes.Num();
    }
     
    if (NewIndex != CurrentIndex)
    {
        //@Previous Item Type Button
        if (UCustomButton* PreviousButton = MItemTypeButtons[CurrentSelectedItemType])
        {
            PreviousButton->SetButtonState(EButtonState::Normal);
        }

        //@Current Item Type Button                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
        EItemType NewType = ItemTypes[NewIndex];
        CurrentSelectedItemType = NewType;

        //@Button의 상태를 Selected로 변경
        if (UCustomButton* NewButton = MItemTypeButtons[NewType])
        {
            NewButton->SetButtonState(EButtonState::Selected);
        }

        //@Item Type 버튼 클릭 이벤트
        InventoryToolBarButtonClicked.ExecuteIfBound(NewType);

        UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 선택되었습니다.", *UEnum::GetValueAsString(NewType));
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInventoryToolBar::OnInventoryToolBarButtonClicked_Implementation(EItemType ItemType)
{
    if (CurrentSelectedItemType == ItemType) return;

    //@선택된 버튼 취소
    CancelInventoryToolBarButtonSelected(CurrentSelectedItemType);
    //@Current Item Type
    CurrentSelectedItemType = ItemType;
    //@Type 버튼 클릭 이벤트
    InventoryToolBarButtonClicked.ExecuteIfBound(ItemType);

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(ItemType));
    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UInventoryToolBar::OnInventoryToolBarButtonHovered_Implementation(EItemType ItemType)
{
    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(ItemType));

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UInventoryToolBar::OnInventoryToolBarButtonUnhovered_Implementation(EItemType ItemType)
{
    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼에서 마우스가 벗어났습니다.", *UEnum::GetValueAsString(ItemType));

    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}

void UInventoryToolBar::CancelInventoryToolBarButtonSelected_Implementation(EItemType PreviousItemType)
{
    auto PreviousButton = MItemTypeButtons.FindRef(PreviousItemType);
    if (!PreviousButton)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Item Type Button이 유효하지 않습니다!");
        return;
    }

    PreviousButton->CancelSelectedButton();

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 취소되었습니다.", *UEnum::GetValueAsString(PreviousItemType));
    //@TODO: Animation 관련 작업 시 해당 함수 오버라이딩...
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
int32 UInventoryToolBar::GetCurrentButtonIndex() const
{
    TArray<EItemType> ItemTypes;
    MItemTypeButtons.GetKeys(ItemTypes);
    return ItemTypes.IndexOfByKey(CurrentSelectedItemType);
}
#pragma endregion