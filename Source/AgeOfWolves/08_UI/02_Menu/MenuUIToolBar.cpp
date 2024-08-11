#include "MenuUIToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"

#include "Styling/SlateTypes.h"

DEFINE_LOG_CATEGORY(LogToolBar)

#pragma region Default Setting
UMenuUIToolBar::UMenuUIToolBar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UMenuUIToolBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@External Binding
	//@TODO: Exeternal Binding to Input Comp(키보드 입력)


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

void UMenuUIToolBar::InitializeToolBar()
{
	//@Buttons
    CreateButtons();
    //@Update Button Style
    UpdateButtonStyle(InventoryUIButton);
	//@Delegate: 초기화 완료 이벤트
	ToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion

#pragma region SubWidgets
void UMenuUIToolBar::CreateButtons()
{
    if (!CategoryButtonsBox)
    {
        UE_LOGFMT(LogToolBar, Error, "CategoryButtonsBox가 유효하지 않습니다.");
        return;
    }

    //@Inventory UI 버튼
    if (InventoryUIButton)
    {
        //@바인딩
        InventoryUIButton->OnClicked.AddDynamic(this, &UMenuUIToolBar::OnInventoryButtonClicked);
        InventoryUIButton->OnHovered.AddDynamic(this, &UMenuUIToolBar::OnInventoryButtonHovered);
        InventoryUIButton->OnUnhovered.AddDynamic(this, &UMenuUIToolBar::OnInventoryButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(InventoryUIButton, EMenuCategory::Inventory);
    }
    //@Level UI 버튼
    if (LevelUIButton)
    {
        //@바인딩
        LevelUIButton->OnClicked.AddDynamic(this, &UMenuUIToolBar::OnLevelButtonClicked);
        LevelUIButton->OnHovered.AddDynamic(this, &UMenuUIToolBar::OnLevelButtonHovered);
        LevelUIButton->OnUnhovered.AddDynamic(this, &UMenuUIToolBar::OnLevelButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(LevelUIButton, EMenuCategory::Level);
    }
    //@Map UI 버튼
    if (MapUIButton)
    {
        //@바인딩
        MapUIButton->OnClicked.AddDynamic(this, &UMenuUIToolBar::OnMapButtonClicked);
        MapUIButton->OnHovered.AddDynamic(this, &UMenuUIToolBar::OnMapButtonHovered);
        MapUIButton->OnUnhovered.AddDynamic(this, &UMenuUIToolBar::OnMapButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(MapUIButton, EMenuCategory::Map);
    }
    //@System UI 버튼 
    if (SystemUIButton)
    {
        //@바인딩
        SystemUIButton->OnClicked.AddDynamic(this, &UMenuUIToolBar::OnSystemButtonClicked);
        SystemUIButton->OnHovered.AddDynamic(this, &UMenuUIToolBar::OnSystemButtonHovered);
        SystemUIButton->OnUnhovered.AddDynamic(this, &UMenuUIToolBar::OnSystemButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(SystemUIButton, EMenuCategory::System);
    }
}

void UMenuUIToolBar::UpdateButtonStyle(UButton* SelectedButton, UButton* PreviousButton)
{
    //@Previous Button
    if (PreviousButton && PreviousButton != SelectedButton)
    {
        //@Normal의 Original Style
        FButtonStyle OriginalStyle = PreviousButton->WidgetStyle;
        //@Normal의 Tint
        OriginalStyle.Normal.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));  // 완전히 투명하게 설정
        //@Set Style
        PreviousButton->SetStyle(OriginalStyle);
    }
    //@Selected Button
    if (SelectedButton)
    {
        //@Button Style
        FButtonStyle ButtonStyle = SelectedButton->WidgetStyle;
        //@Normal 상태를 Hover 상태로 변경
        ButtonStyle.Normal = ButtonStyle.Hovered;
        //@Normal.Tint를 투명하게
        ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor(1, 1, 1, 1));  // 불투명하게 설정
        //Set Style
        SelectedButton->SetStyle(ButtonStyle);
    }
}

void UMenuUIToolBar::HandleButtonClick(EMenuCategory Category)
{
    //@Selected
    UButton* SelectedButton = *MCategoryButtons.FindKey(Category);
    //@Prvious
    UButton* PreviousButton = *MCategoryButtons.FindKey(CurrentCategory);
    //@Update Style
    if (SelectedButton != PreviousButton)
    {
        UpdateButtonStyle(SelectedButton, PreviousButton);
    }
    //@Current Category
    CurrentCategory = Category;
    //@Delegate
    if (MenuCategoryButtonClikced.IsBound())
    {
        MenuCategoryButtonClikced.Execute(Category);
    }

    UE_LOGFMT(LogToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(Category));
}


void UMenuUIToolBar::HandleButtonHover(EMenuCategory Category)
{
    UE_LOGFMT(LogToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(Category));

    //@TODO: Hover Image 설정
}

void UMenuUIToolBar::HandleButtonUnhover(EMenuCategory Category)
{
    UE_LOGFMT(LogToolBar, Log, "버튼에서 마우스가 벗어났습니다.");

    //@TODO: Hover Image 관련 설정
}
#pragma endregion

#pragma region Callbacks

#pragma endregion
