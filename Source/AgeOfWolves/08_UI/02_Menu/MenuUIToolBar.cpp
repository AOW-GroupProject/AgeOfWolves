// MenuUIToolBar.cpp

#include "MenuUIToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogToolBar)

//@Defualt Setting
#pragma region Default Setting
UMenuUIToolBar::UMenuUIToolBar(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , DefaultCategory(EMenuCategory::Inventory)
    , CurrentCategory(EMenuCategory::MAX)
{
    //@MMenuCategoryButtons 맵 초기화
    for (uint8 i = 0; i < static_cast<uint8>(EMenuCategory::MAX); ++i)
    {
        MMenuCategoryButtons.Add(static_cast<EMenuCategory>(i), nullptr);
    }

    //@버튼 클래스 초기화 (null로 설정, 실제 값은 블루프린트에서 설정될 것임)
    InventoryButtonClass = nullptr;
    LevelButtonClass = nullptr;
    MapButtonClass = nullptr;
    SystemButtonClass = nullptr;
}

void UMenuUIToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@External Binding
    //@TODO: External Binding to Input Comp(키보드 입력)
}

void UMenuUIToolBar::NativePreConstruct()
{
    Super::NativePreConstruct();

    SetIsFocusable(false);
}

void UMenuUIToolBar::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMenuUIToolBar::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UMenuUIToolBar::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);

}

void UMenuUIToolBar::InternalBindToButton(UCustomButton* Button, EMenuCategory Category)
{
    if (!Button)
    {
        UE_LOGFMT(LogToolBar, Error, "Button이 유효하지 않습니다.");
        return;
    }

    //@내부 바인딩
    Button->ButtonHovered.AddUObject(this, &UMenuUIToolBar::OnMenuUIToolBarButtonHovered, Category);
    Button->ButtonUnhovered.AddUObject(this, &UMenuUIToolBar::OnMenuUIToolBarButtonUnhovered, Category);
    Button->ButtonSelected.AddUObject(this, &UMenuUIToolBar::OnMenuUIToolBarButtonClicked, Category);
}

void UMenuUIToolBar::InitializeToolBar()
{
    //@Buttons
    CreateButtons();
    //@Delegate: 초기화 완료 이벤트
    ToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UMenuUIToolBar::ResetToolBar()
{

    EMenuCategory PreviousCategory = CurrentCategory;

    if (CurrentCategory == EMenuCategory::MAX)
    {
        //@Current Category
        CurrentCategory = DefaultCategory;

        //@Default Category Button
        UCustomButton* DefaultCategoryButton = MMenuCategoryButtons.FindRef(DefaultCategory);
        if (!DefaultCategoryButton)
        {
            UE_LOGFMT(LogToolBar, Error, "Default 카테고리 버튼을 찾을 수 없습니다. 초기화에 실패했을 수 있습니다.");
            return;
        }

        //@Set Button State
        if (!DefaultCategoryButton->SetButtonSelectedByKeyboard())
        {
            UE_LOGFMT(LogToolBar, Error, "Default 카테고리 버튼을 Selected로 초기화하는데 실패했습니다.");
            return;
        }

        //@Menu Category Button 선택 이벤트 호출
        MenuCategoryButtonClicked.ExecuteIfBound(CurrentCategory);
    }
    else
    {
        //@Current Category != Default Category
        if (PreviousCategory != DefaultCategory)
        {
            CancelMenuUIToolBarButtonSelected(PreviousCategory);
        }

        CurrentCategory = DefaultCategory;

        UCustomButton* DefaultCategoryButton = MMenuCategoryButtons.FindRef(CurrentCategory);
        if (!DefaultCategoryButton)
        {
            UE_LOGFMT(LogToolBar, Error, "Default 카테고리 버튼을 찾을 수 없습니다. 초기화에 실패했을 수 있습니다.");
            return;
        }

        if (!DefaultCategoryButton->SetButtonSelectedByKeyboard())
        {
            UE_LOGFMT(LogToolBar, Error, "Default 카테고리 버튼을 Selected로 초기화하는데 실패했습니다.");
            return;
        }
    }

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

void UMenuUIToolBar::MoveCategoryLeft()
{
    //@Menu Categories
    TArray<EMenuCategory> MenuCategories;
    MMenuCategoryButtons.GetKeys(MenuCategories);

    //@Current Category Button의 인덱스
    int32 CurrentIndex = MenuCategories.IndexOfByKey(CurrentCategory);
    int32 NewIndex = (CurrentIndex - 1 + MenuCategories.Num()) % MenuCategories.Num();

    if (NewIndex != CurrentIndex)
    {
        //@Previous Category Button
        if (UCustomButton* PreviousButton = MMenuCategoryButtons[CurrentCategory])
        {
            CancelMenuUIToolBarButtonSelected(CurrentCategory);

            //PreviousButton->SetButtonState(EButtonState::Normal);
        }

        //@Current Category Button
        EMenuCategory NewCategory = MenuCategories[NewIndex];
        CurrentCategory = NewCategory;

        //@Button의 상태를 Selected로 변경
        if (UCustomButton* NewButton = MMenuCategoryButtons[NewCategory])
        {
            NewButton->SetButtonSelectedByKeyboard();

            //NewButton->SetButtonState(EButtonState::Selected);
        }

        //@Menu Category 버튼 클릭 이벤트
        MenuCategoryButtonClicked.ExecuteIfBound(NewCategory);

        UE_LOGFMT(LogToolBar, Log, "메뉴 카테고리가 왼쪽으로 이동했습니다. 새 카테고리: {0}", *UEnum::GetValueAsString(NewCategory));
    }
}

void UMenuUIToolBar::MoveCategoryRight()
{
    //@Menu Categories
    TArray<EMenuCategory> MenuCategories;
    MMenuCategoryButtons.GetKeys(MenuCategories);

    //@Current Category Button의 인덱스
    int32 CurrentIndex = MenuCategories.IndexOfByKey(CurrentCategory);
    int32 NewIndex = (CurrentIndex + 1) % MenuCategories.Num();

    if (NewIndex != CurrentIndex)
    {
        //@Previous Category Button
        if (UCustomButton* PreviousButton = MMenuCategoryButtons[CurrentCategory])
        {
            //@Cancel
            CancelMenuUIToolBarButtonSelected(CurrentCategory);
        }

        //@Current Category Button
        EMenuCategory NewCategory = MenuCategories[NewIndex];
        CurrentCategory = NewCategory;

        //@Button의 상태를 Selected로 변경
        if (UCustomButton* NewButton = MMenuCategoryButtons[NewCategory])
        {
            if (!NewButton->SetButtonSelectedByKeyboard())
            {
                UE_LOGFMT(LogToolBar, Log, "새로운 버튼을 Selected 상태로 설정하는데 실패했습니다.");
                return;
            }
        }

        //@Menu Category 버튼 클릭 이벤트
        MenuCategoryButtonClicked.ExecuteIfBound(NewCategory);

        UE_LOGFMT(LogToolBar, Log, "메뉴 카테고리가 오른쪽으로 이동했습니다. 새 카테고리: {0}", *UEnum::GetValueAsString(NewCategory));
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
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

void UMenuUIToolBar::OnMenuUIToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, EMenuCategory Category)
{
    if (CurrentCategory == Category) return;

    //@Cancel
    CancelMenuUIToolBarButtonSelected(CurrentCategory);

    //@Current Category
    CurrentCategory = Category;

    //@Delegate
    MenuCategoryButtonClicked.ExecuteIfBound(Category);

    UE_LOGFMT(LogToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(Category));
}

void UMenuUIToolBar::OnMenuUIToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, EMenuCategory Category)
{
    UE_LOGFMT(LogToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(Category));

    //@TODO: Hover 상태와 관련된 처리...
}

void UMenuUIToolBar::OnMenuUIToolBarButtonUnhovered_Implementation(EMenuCategory Category)
{
    UE_LOGFMT(LogToolBar, Log, "{0} 버튼에서 마우스가 벗어났습니다.", *UEnum::GetValueAsString(Category));

    //@TODO: Unhover 상태와 관련된 처리...
}

void UMenuUIToolBar::CancelMenuUIToolBarButtonSelected_Implementation(EMenuCategory PreviousCategory)
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

//@Utility(Setter, Getter,...etc)
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