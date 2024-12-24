#include "MenuUIToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogMenuToolBar)

//@Default Setting
#pragma region Default Setting
UMenuUIToolBar::UMenuUIToolBar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DefaultButtonIndex = MenuCategoryToIndex(DefaultCategory);
    CurrentSelectedIndex = MenuCategoryToIndex(EMenuCategory::MAX);
}

void UMenuUIToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UMenuUIToolBar::InternalBindToButton(UCustomButton* Button, EMenuCategory Category)
{
    if (!Button)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Button이 유효하지 않습니다.");
        return;
    }

    uint8 CategoryIndex = MenuCategoryToIndex(Category);
    Button->ButtonSelected.AddUObject(this, &UMenuUIToolBar::OnToolBarButtonClicked, CategoryIndex);
    Button->ButtonHovered.AddUObject(this, &UMenuUIToolBar::OnToolBarButtonHovered, CategoryIndex);
    Button->ButtonUnhovered.AddUObject(this, &UMenuUIToolBar::OnToolBarButtonUnhovered, CategoryIndex);
}

void UMenuUIToolBar::InitializeToolBar()
{
    Super::InitializeToolBar();
    CurrentCategory = IndexToMenuCategory(CurrentSelectedIndex);
}
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
// MenuUIToolBar.cpp
void UMenuUIToolBar::ResetToolBar()
{
    //@Default Menu Category
    if (DefaultCategory == EMenuCategory::MAX)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Default Category 설정이 필요합니다!");
        return;
    }
    //@Custom Button
    UCustomButton* DefaultCategoryButton = MMenuCategoryButtons.FindRef(DefaultCategory);
    if (!DefaultCategoryButton)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Default 카테고리 버튼을 찾을 수 없습니다. 초기화에 실패했을 수 있습니다.");
        return;
    }
    //@Selected By Keyboard
    if (!DefaultCategoryButton->SetButtonSelectedByKeyboard())
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Default 카테고리 버튼을 Selected로 초기화하는데 실패했습니다.");
        return;
    }

    UE_LOGFMT(LogMenuToolBar, Log, "MenuUIToolBar가 초기 상태로 리셋되었습니다. 현재 카테고리: {0}",
        *UEnum::GetValueAsString(CurrentCategory));
}

void UMenuUIToolBar::CreateButtons()
{
    if (!ButtonBox)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "ButtonBox가 유효하지 않습니다.");
        return;
    }

    ButtonBox->ClearChildren();
    MMenuCategoryButtons.Empty();
    MButtons.Empty();

    CreateAndAddButton(EMenuCategory::Inventory);
    CreateAndAddButton(EMenuCategory::Level);
    CreateAndAddButton(EMenuCategory::Map);
    CreateAndAddButton(EMenuCategory::System);

    ResetToolBar();

    UE_LOGFMT(LogMenuToolBar, Log, "모든 버튼이 생성되고 추가되었습니다.");
}

void UMenuUIToolBar::CreateAndAddButton(EMenuCategory Category)
{
    TSubclassOf<UCustomButton> ButtonClass;
    switch (Category)
    {
    case EMenuCategory::Inventory:
        ButtonClass = InventoryButtonClass;
        break;
    case EMenuCategory::Level:
        ButtonClass = LevelButtonClass;
        break;
    case EMenuCategory::Map:
        ButtonClass = MapButtonClass;
        break;
    case EMenuCategory::System:
        ButtonClass = SystemButtonClass;
        break;
    default:
        UE_LOGFMT(LogMenuToolBar, Error, "알 수 없는 카테고리: {0}", UEnum::GetValueAsString(Category));
        return;
    }

    if (!ButtonClass)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "{0} 카테고리의 버튼 클래스가 설정되지 않았습니다.", UEnum::GetValueAsString(Category));
        return;
    }

    UCustomButton* NewButton = CreateWidget<UCustomButton>(this, ButtonClass);
    if (!NewButton)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "CustomButton 생성 실패: {0}", UEnum::GetValueAsString(Category));
        return;
    }

    InternalBindToButton(NewButton, Category);

    UHorizontalBoxSlot* HorizontalSlot = ButtonBox->AddChildToHorizontalBox(NewButton);
    if (!HorizontalSlot)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "{0} 버튼을 Horizontal Box에 추가하는데 실패했습니다.", UEnum::GetValueAsString(Category));
        return;
    }

    HorizontalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    HorizontalSlot->SetHorizontalAlignment(HAlign_Fill);
    HorizontalSlot->SetVerticalAlignment(VAlign_Fill);

    MMenuCategoryButtons.Add(Category, NewButton);
    MButtons.Add(MenuCategoryToIndex(Category), NewButton);

    UE_LOGFMT(LogMenuToolBar, Log, "{0} 버튼이 생성되고 추가되었습니다.", UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::MoveSelection(int32 Direction)
{
    //@Menu Categories
    TArray<EMenuCategory> MenuCategories;
    MMenuCategoryButtons.GetKeys(MenuCategories);

    //@Current Category Button의 인덱스
    int32 CurrentIndex = MenuCategories.IndexOfByKey(CurrentCategory);
    int32 NewIndex;

    //@오른쪽 방향키 눌림
    if (Direction > 0)
    {
        NewIndex = (CurrentIndex + 1) % MenuCategories.Num();
    }
    //@왼쪽 방향키 눌림
    else
    {
        NewIndex = (CurrentIndex - 1 + MenuCategories.Num()) % MenuCategories.Num();
    }

    if (NewIndex != CurrentIndex)
    {
        //@Current Category Button
        EMenuCategory NewCategory = MenuCategories[NewIndex];

        //@Button의 상태를 Selected로 변경
        if (UCustomButton* NewButton = MMenuCategoryButtons[NewCategory])
        {
            if (!NewButton->SetButtonSelectedByKeyboard())
            {
                UE_LOGFMT(LogMenuToolBar, Warning, "새로운 버튼을 Selected 상태로 설정하는데 실패했습니다.");
                return;
            }
        }

        UE_LOGFMT(LogMenuToolBar, Log, "메뉴 카테고리가 {0}쪽으로 이동했습니다. 새 카테고리: {1}",
            Direction > 0 ? TEXT("오른") : TEXT("왼"),
            *UEnum::GetValueAsString(NewCategory));
    }
}

#pragma endregion

//@Callbacks
#pragma region Callbacks
void UMenuUIToolBar::MenuUIVisibilityChangedNotified(bool bIsVisible)
{
    if (!bIsVisible)
    {
        ResetToolBar();
    }
}

void UMenuUIToolBar::OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{

    Super::OnToolBarButtonClicked_Implementation(InteractionMethodType, ButtonIndex);

    EMenuCategory NewCategory = IndexToMenuCategory(ButtonIndex);
    if (CurrentCategory == NewCategory) return;

    CurrentCategory = NewCategory;
    MenuCategoryButtonClicked.ExecuteIfBound(NewCategory);

    UE_LOGFMT(LogMenuToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(NewCategory));
}

void UMenuUIToolBar::OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    Super::OnToolBarButtonHovered_Implementation(InteractionMethodType, ButtonIndex);

    EMenuCategory Category = IndexToMenuCategory(ButtonIndex);
    UE_LOGFMT(LogMenuToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex)
{
    Super::OnToolBarButtonUnhovered_Implementation(ButtonIndex);

    EMenuCategory Category = IndexToMenuCategory(ButtonIndex);
    UE_LOGFMT(LogMenuToolBar, Log, "{0} 버튼에서 마우스가 벗어났습니다.", *UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex)
{
    Super::CancelToolBarButtonSelected_Implementation(PreviousIndex);
}
#pragma endregion

//@Utility
#pragma region Utility
bool UMenuUIToolBar::IsValidButtonIndex(uint8 Index) const
{
    return Index < static_cast<uint8>(EMenuCategory::MAX);
}
#pragma endregion