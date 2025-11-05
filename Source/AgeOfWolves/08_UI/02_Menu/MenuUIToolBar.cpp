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
        UE_LOGFMT(LogMenuToolBar, Error, "버튼이 유효하지 않습니다.");
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
void UMenuUIToolBar::ResetToolBar()
{
    if (DefaultCategory == EMenuCategory::MAX)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Default Category가 설정되지 않았습니다!");
        return;
    }

    UCustomButton* DefaultButton = MMenuCategoryButtons.FindRef(DefaultCategory);
    if (!DefaultButton)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Default 카테고리 버튼을 찾을 수 없습니다.");
        return;
    }

    if (!DefaultButton->SetButtonSelectedByKeyboard())
    {
        UE_LOGFMT(LogMenuToolBar, Error, "Default 카테고리 버튼 선택 실패");
        return;
    }

    UE_LOGFMT(LogMenuToolBar, Log, "MenuUIToolBar 리셋 완료: {0}", *UEnum::GetValueAsString(CurrentCategory));
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

    //@카테고리별 버튼 생성
    CreateAndAddButton(EMenuCategory::Inventory);
    CreateAndAddButton(EMenuCategory::Level);
    CreateAndAddButton(EMenuCategory::Map);
    CreateAndAddButton(EMenuCategory::System);

    //@초기 상태로 리셋
    ResetToolBar();

    UE_LOGFMT(LogMenuToolBar, Log, "버튼 생성 완료");
}

void UMenuUIToolBar::CreateAndAddButton(EMenuCategory Category)
{
    //@버튼 클래스 선택
    TSubclassOf<UCustomButton> ButtonClass = nullptr;
    switch (Category)
    {
    case EMenuCategory::Inventory:  ButtonClass = InventoryButtonClass; break;
    case EMenuCategory::Level:      ButtonClass = LevelButtonClass; break;
    case EMenuCategory::Map:        ButtonClass = MapButtonClass; break;
    case EMenuCategory::System:     ButtonClass = SystemButtonClass; break;
    default:
        UE_LOGFMT(LogMenuToolBar, Error, "알 수 없는 카테고리: {0}", *UEnum::GetValueAsString(Category));
        return;
    }

    if (!ButtonClass)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "{0} 카테고리 버튼 클래스가 설정되지 않았습니다.", *UEnum::GetValueAsString(Category));
        return;
    }

    //@버튼 생성
    UCustomButton* NewButton = CreateWidget<UCustomButton>(this, ButtonClass);
    if (!NewButton)
    {
        UE_LOGFMT(LogMenuToolBar, Error, "버튼 생성 실패: {0}", *UEnum::GetValueAsString(Category));
        return;
    }

    //@이벤트 바인딩
    InternalBindToButton(NewButton, Category);

    //@HorizontalBox에 추가
    UHorizontalBoxSlot* ButtonSlot = ButtonBox->AddChildToHorizontalBox(NewButton);
    if (ButtonSlot)
    {
        ButtonSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        ButtonSlot->SetHorizontalAlignment(HAlign_Fill);
        ButtonSlot->SetVerticalAlignment(VAlign_Fill);
    }

    //@맵에 저장
    MMenuCategoryButtons.Add(Category, NewButton);

    UE_LOGFMT(LogMenuToolBar, Log, "{0} 버튼 추가됨", *UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::MoveLeft()
{
    MoveSelection(-1);

}

void UMenuUIToolBar::MoveRight()
{
    MoveSelection(1);
}

void UMenuUIToolBar::MoveSelection(int32 Direction)
{
    TArray<EMenuCategory> MenuCategories;
    MMenuCategoryButtons.GetKeys(MenuCategories);

    if (MenuCategories.Num() == 0)
    {
        UE_LOGFMT(LogMenuToolBar, Warning, "이동할 버튼이 없습니다.");
        return;
    }

    //@현재 인덱스 찾기
    int32 CurrentIndex = MenuCategories.IndexOfByKey(CurrentCategory);
    if (CurrentIndex == INDEX_NONE)
    {
        UE_LOGFMT(LogMenuToolBar, Warning, "현재 선택된 카테고리가 유효하지 않습니다.");
        return;
    }

    //@다음 인덱스 계산 (순환)
    int32 NewIndex = (CurrentIndex + Direction + MenuCategories.Num()) % MenuCategories.Num();
    EMenuCategory NewCategory = MenuCategories[NewIndex];

    //@버튼 선택
    if (UCustomButton* NewButton = MMenuCategoryButtons.FindRef(NewCategory))
    {
        if (NewButton->SetButtonSelectedByKeyboard())
        {
            UE_LOGFMT(LogMenuToolBar, Log, "{0} 카테고리 선택됨", *UEnum::GetValueAsString(NewCategory));
        }
        else
        {
            UE_LOGFMT(LogMenuToolBar, Warning, "버튼 선택 실패");
        }
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
    if (CurrentCategory == NewCategory)
    {
        return;
    }

    CurrentCategory = NewCategory;
    MenuCategoryButtonClicked.ExecuteIfBound(NewCategory);

    UE_LOGFMT(LogMenuToolBar, Log, "{0} 카테고리 클릭됨", *UEnum::GetValueAsString(NewCategory));
}

void UMenuUIToolBar::OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex)
{
    Super::OnToolBarButtonHovered_Implementation(InteractionMethodType, ButtonIndex);

    EMenuCategory Category = IndexToMenuCategory(ButtonIndex);
    UE_LOGFMT(LogMenuToolBar, Log, "{0} 카테고리 호버됨", *UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex)
{
    Super::OnToolBarButtonUnhovered_Implementation(ButtonIndex);

    EMenuCategory Category = IndexToMenuCategory(ButtonIndex);
    UE_LOGFMT(LogMenuToolBar, Log, "{0} 카테고리 언호버됨", *UEnum::GetValueAsString(Category));
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

UCustomButton* UMenuUIToolBar::GetButtonByIndex(uint8 Index) const
{
    EMenuCategory Category = IndexToMenuCategory(Index);
    return MMenuCategoryButtons.FindRef(Category);
}
#pragma endregion