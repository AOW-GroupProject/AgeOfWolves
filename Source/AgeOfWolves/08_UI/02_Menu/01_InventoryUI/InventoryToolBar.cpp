#include "InventoryToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogInventoryToolBar)

//@Default Setting
#pragma region Default Setting
UInventoryToolBar::UInventoryToolBar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DefaultButtonIndex = ItemTypeToIndex(DefaultItemType);
    CurrentSelectedIndex = ItemTypeToIndex(EItemType::MAX);
    ButtonBox = nullptr;
}

void UInventoryToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}


void UInventoryToolBar::InternalBindToButton(UCustomButton* Button, EItemType ItemType)
{
    if (!Button)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Button이 유효하지 않습니다.");
        return;
    }

    uint8 TypeIndex = ItemTypeToIndex(ItemType);
    Button->ButtonSelected.AddUObject(this, &UInventoryToolBar::OnToolBarButtonClicked, TypeIndex);
    Button->ButtonHovered.AddUObject(this, &UInventoryToolBar::OnToolBarButtonHovered, TypeIndex);
    Button->ButtonUnhovered.AddUObject(this, &UInventoryToolBar::OnToolBarButtonUnhovered, TypeIndex);
}

void UInventoryToolBar::InitializeToolBar()
{
    Super::InitializeToolBar();
    //@상위 클래스의 초기화가 완료되면 현재 선택된 인덱스를 ItemType으로 변환
    CurrentSelectedItemType = IndexToItemType(CurrentSelectedIndex);
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
// InventoryToolBar.cpp
void UInventoryToolBar::ResetToolBar()
{
    if (DefaultItemType == EItemType::MAX)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Default Item Type을 설정해주세요!");
        return;
    }

    //@Button
    UCustomButton* DefaultButton = MItemTypeButtons.FindRef(DefaultItemType);
    if (!DefaultButton)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Default 아이템 타입 버튼을 찾을 수 없습니다. 초기화에 실패했을 수 있습니다.");
        return;
    }

    //@Set Button Selected By Keyboard
    if (!DefaultButton->SetButtonSelectedByKeyboard())
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "Default 아이템 타입 버튼을 Selected로 초기화하는데 실패했습니다.");
        return;
    }

    UE_LOGFMT(LogInventoryToolBar, Log, "Inventory Tool Bar가 초기 상태로 리셋되었습니다. 현재 아이템 타입: {0}",
        *UEnum::GetValueAsString(CurrentSelectedItemType));
}

void UInventoryToolBar::CreateButtons()
{
    if (!ButtonBox)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "ButtonBox가 유효하지 않습니다.");
        return;
    }

    ButtonBox->ClearChildren();
    MItemTypeButtons.Empty();
    MButtons.Empty();

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
    UHorizontalBoxSlot* HorizontalSlot = ButtonBox->AddChildToHorizontalBox(Spacer);
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
    UHorizontalBoxSlot* HorizontalSlot = ButtonBox->AddChildToHorizontalBox(NewButton);
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

    //@Maps
    MItemTypeButtons.Add(ButtonType, NewButton);
    MButtons.Add(ItemTypeToIndex(ButtonType), NewButton);

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 생성되고 추가되었습니다. Scale: {1}", UEnum::GetValueAsString(ButtonType), Scale);
}

void UInventoryToolBar::MoveSelection(int32 Direction)
{
    //@Item Types
    TArray<EItemType> ItemTypes;
    MItemTypeButtons.GetKeys(ItemTypes);

    //@Current Item Type Button의 인덱스
    int32 CurrentIndex = ItemTypes.IndexOfByKey(CurrentSelectedItemType);
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
        //@Current Item Type Button
        EItemType NewType = ItemTypes[NewIndex];

        //@Button의 상태를 Selected로 변경
        if (UCustomButton* NewButton = MItemTypeButtons[NewType])
        {
            NewButton->SetButtonSelectedByKeyboard();
        }

        UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 선택되었습니다.", *UEnum::GetValueAsString(NewType));
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInventoryToolBar::OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    //@Super: 이전 Selected Item Type 버튼의 취소 작업
    Super::OnToolBarButtonClicked_Implementation(InteractionMethodType, ButtonIndex);

    EItemType NewItemType = IndexToItemType(ButtonIndex);
    if (CurrentSelectedItemType == NewItemType) return;

    //@Current Selected ItemType 변경
    CurrentSelectedItemType = NewItemType;

    //Inventory Tool Bar 버튼 클릭 이벤트 호출
    InventoryToolBarButtonClicked.ExecuteIfBound(NewItemType);

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(NewItemType));
}

void UInventoryToolBar::OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    Super::OnToolBarButtonHovered_Implementation(InteractionMethodType, ButtonIndex);

    //@Current Hovered  Item Type 변경
    EItemType ItemType = IndexToItemType(ButtonIndex);

    //@Inventory Tool Bar 버튼 호버 이벤트 호출
    InventoryToolBarButtonHovered.ExecuteIfBound(ItemType);

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(ItemType));
}

void UInventoryToolBar::OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex)
{
    Super::OnToolBarButtonUnhovered_Implementation(ButtonIndex);

    //@Hovered -> Unhovered 된 Item Type 버튼
    EItemType ItemType = IndexToItemType(ButtonIndex);

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼에서 마우스가 벗어났습니다.", *UEnum::GetValueAsString(ItemType));
}

void UInventoryToolBar::CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex)
{
    Super::CancelToolBarButtonSelected_Implementation(PreviousIndex);
}
#pragma endregion

//@Utility
#pragma region Utility
bool UInventoryToolBar::IsValidButtonIndex(uint8 Index) const
{
    return Index < static_cast<uint8>(EItemType::MAX);
}
#pragma endregion