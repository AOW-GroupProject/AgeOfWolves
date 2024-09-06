// MenuUIToolBar.cpp

#include "MenuUIToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogToolBar)

#pragma region Default Setting
UMenuUIToolBar::UMenuUIToolBar(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UMenuUIToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@External Binding
    //@TODO: External Binding to Input Comp(키보드 입력)
}

void UMenuUIToolBar::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UMenuUIToolBar::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMenuUIToolBar::NativeDestruct()
{
    Super::NativeDestruct();
}

void UMenuUIToolBar::InternalBindToButton(UCustomButton* Button, EMenuCategory Category)
{
    if (!Button)
    {
        UE_LOGFMT(LogToolBar, Error, "Button이 유효하지 않습니다.");
        return;
    }

    Button->ButtonSelected.AddUObject(this, &UMenuUIToolBar::HandleButtonClick, Category);
    Button->ButtonHovered.AddUObject(this, &UMenuUIToolBar::HandleButtonHover, Category);
    Button->ButtonUnhovered.AddUObject(this, &UMenuUIToolBar::HandleButtonUnhover, Category);
}

void UMenuUIToolBar::InitializeToolBar()
{
    //@Buttons
    CreateButtons();
    //@Delegate: 초기화 완료 이벤트
    ToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion

#pragma region Widgets
void UMenuUIToolBar::ResetToolBar()
{
    //@Previous Category
    EMenuCategory PreviousCategory = CurrentCategory;
    //@Current Category를 Default Category로 설정
    CurrentCategory = DefaultCategory;

    //@Previous Category Button
    if (PreviousCategory != EMenuCategory::MAX
        && PreviousCategory != DefaultCategory)
    {
        //@Button Canceled
        HandleButtonCanceled(PreviousCategory);
    }

    //@Default Category Button
    UCustomButton* DefaultCategoryButton = MMenuCategoryButtons.FindRef(CurrentCategory);
    if (!DefaultCategoryButton)
    {
        UE_LOGFMT(LogToolBar, Error, "Default 카테고리 버튼을 찾을 수 없습니다. 초기화에 실패했을 수 있습니다.");
        return;
    }

    //@Set Button State
    DefaultCategoryButton->SetButtonState(EButtonState::Selected);

    //@Menu Category Button 선택 이벤트 호출
    MenuCategoryButtonClicked.ExecuteIfBound(CurrentCategory);

    UE_LOGFMT(LogToolBar, Log, "MenuUIToolBar가 초기 상태로 리셋되었습니다. 현재 카테고리: {0}",
        *UEnum::GetValueAsString(CurrentCategory));
}

void UMenuUIToolBar::CreateButtons()
{
    if (!CategoryButtonsBox)
    {
        UE_LOGFMT(LogToolBar, Error, "CategoryButtonsBox가 유효하지 않습니다.");
        return;
    }

    CategoryButtonsBox->ClearChildren();
    MMenuCategoryButtons.Empty();

    CreateAndAddButton(EMenuCategory::Inventory, InventoryButtonClass);
    CreateAndAddButton(EMenuCategory::Level, LevelButtonClass);
    CreateAndAddButton(EMenuCategory::Map, MapButtonClass);
    CreateAndAddButton(EMenuCategory::System, SystemButtonClass);

    //@ToolBar를 초기 상태로 설정합니다.
    ResetToolBar();

    UE_LOGFMT(LogToolBar, Log, "모든 버튼이 생성되고 추가되었습니다.");
}

void UMenuUIToolBar::CreateAndAddButton(EMenuCategory Category, TSubclassOf<UCustomButton> ButtonClass)
{
    //@Button 블루프린트 클래스
    if (!ButtonClass)
    {
        UE_LOGFMT(LogToolBar, Error, "{0} 버튼 클래스가 설정되지 않았습니다.", *UEnum::GetValueAsString(Category));
        return;
    }

    //@Create Widget
    UCustomButton* NewButton = CreateWidget<UCustomButton>(this, ButtonClass);
    if (!NewButton)
    {
        UE_LOGFMT(LogToolBar, Error, "Button 생성에 실패했습니다.");
        return;
    }

    //@Internal Binding
    InternalBindToButton(NewButton, Category);

    //@AddChild
    UHorizontalBoxSlot* HorizontalSlot = CategoryButtonsBox->AddChildToHorizontalBox(NewButton);
    //@Alignment
    if (HorizontalSlot)
    {
        //@Size
        HorizontalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        //@Alignment
        HorizontalSlot->SetHorizontalAlignment(HAlign_Fill);
        HorizontalSlot->SetVerticalAlignment(VAlign_Fill);
    }
    //@TMap
    MMenuCategoryButtons.Add(Category, NewButton);
}

void UMenuUIToolBar::MenuUIVisibilityChangedNotified(bool bIsVisible)
{
    //@Menu UI가 열리면 하는 동작
    if (bIsVisible)
    {

    }
    //@Menu UI가 닫히면 하는 동작
    else
    {

    }
}

void UMenuUIToolBar::MoveCategoryLeft()
{
    //@Current Index
    int32 NewIndex = (GetCurrentCategoryIndex() - 1 + 4) % 4;
    //@New Category
    EMenuCategory NewCategory = GetMenuCategoryFromIndex(NewIndex);
    //@Cancel Button
    HandleButtonCanceled(CurrentCategory);
    //@Click Button
    if (MMenuCategoryButtons.Contains(NewCategory))
    {
        auto NewButton = *MMenuCategoryButtons.Find(NewCategory);
        if (!NewButton)
        {
            return;
        }
        //@TODO: Button 상태를 Selected로 변경합니다.
        NewButton->OnButtonClicked();
    }
    
    UE_LOGFMT(LogToolBar, Log, "메뉴 카테고리가 왼쪽으로 이동했습니다. 새 카테고리: {0}", *UEnum::GetValueAsString(NewCategory));
}

void UMenuUIToolBar::MoveCategoryRight()
{
    //@Current Index
    int32 NewIndex = (GetCurrentCategoryIndex() + 1) % 4;
    //@New Category
    EMenuCategory NewCategory = GetMenuCategoryFromIndex(NewIndex);
    //@Cancel Button
    HandleButtonCanceled(CurrentCategory);
    //@Click Button
    if (MMenuCategoryButtons.Contains(NewCategory))
    {
        auto NewButton = *MMenuCategoryButtons.Find(NewCategory);
        if (!NewButton)
        {
            return;
        }
        //@TODO: Button 상태를 Selected로 변경합니다.
        NewButton->OnButtonClicked();
    }
    
    UE_LOGFMT(LogToolBar, Log, "메뉴 카테고리가 오른쪽으로 이동했습니다. 새 카테고리: {0}", *UEnum::GetValueAsString(NewCategory));
}

void UMenuUIToolBar::HandleButtonClick(EMenuCategory Category)
{
    if (CurrentCategory == Category) return;

    if (UCustomButton* PreviousButton = MMenuCategoryButtons.FindRef(CurrentCategory))
    {
        HandleButtonCanceled(CurrentCategory);
    }

    //@Current Category
    CurrentCategory = Category;

    //@Delegate
    MenuCategoryButtonClicked.ExecuteIfBound(Category);

    UE_LOGFMT(LogToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::HandleButtonHover(EMenuCategory Category)
{
    UE_LOGFMT(LogToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(Category));

    //@TODO: Hover 상태와 관련된 처리...
}

void UMenuUIToolBar::HandleButtonUnhover(EMenuCategory Category)
{
    UE_LOGFMT(LogToolBar, Log, "{0} 버튼에서 마우스가 벗어났습니다.", *UEnum::GetValueAsString(Category));

    //@TODO: Unhover 상태와 관련된 처리...
}

void UMenuUIToolBar::HandleButtonCanceled(EMenuCategory PreviousCategory)
{
    //@Previous Button
    UCustomButton* PreviousButton = MMenuCategoryButtons.FindRef(PreviousCategory);
    if (!PreviousButton)
    {
        UE_LOGFMT(LogToolBar, Error, "Menu Category Button이 유효하지 않습니다!");
        return;
    }

    //@Button Canceled Notified
    PreviousButton->CancelSelectedButton();

    UE_LOGFMT(LogToolBar, Log, "{0} 버튼이 취소되었습니다.", *UEnum::GetValueAsString(PreviousCategory));
}
#pragma endregion

#pragma region Utility
int32 UMenuUIToolBar::GetCurrentCategoryIndex() const
{
    return static_cast<int32>(CurrentCategory);
}

EMenuCategory UMenuUIToolBar::GetMenuCategoryFromIndex(int32 Index) const
{
    return static_cast<EMenuCategory>(FMath::Clamp(Index, 0, 3));
}
#pragma endregion
