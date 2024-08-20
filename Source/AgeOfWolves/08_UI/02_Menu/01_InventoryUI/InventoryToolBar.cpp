#include "InventoryToolBar.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"

#include "Styling/SlateTypes.h"

DEFINE_LOG_CATEGORY(LogInventoryToolBar)

#pragma region Default Setting
UInventoryToolBar::UInventoryToolBar(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UInventoryToolBar::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@External Binding
    //@TODO: External Binding to Input Comp(키보드 입력)
}

void UInventoryToolBar::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UInventoryToolBar::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryToolBar::NativeDestruct()
{
    Super::NativeDestruct();
}

void UInventoryToolBar::InitializeInventoryToolBar()
{
    //@Buttons
    CreateButtons();
    //@Update Button Style
    UpdateButtonStyle(ToolTypeButton);
    //@Delegate: 초기화 완료 이벤트
    InventoryToolBarInitFinished.ExecuteIfBound();
}
#pragma endregion

#pragma region Widgets
void UInventoryToolBar::CreateButtons()
{
    if (!ItemTypeButtonBox)
    {
        UE_LOGFMT(LogInventoryToolBar, Error, "ItemTypeButtonsBox가 유효하지 않습니다.");
        return;
    }
    //@Tool 버튼
    if (ToolTypeButton)
    {
        //@바인딩
        ToolTypeButton->OnClicked.AddDynamic(this, &UInventoryToolBar::OnToolButtonClicked);
        ToolTypeButton->OnHovered.AddDynamic(this, &UInventoryToolBar::OnToolButtonHovered);
        ToolTypeButton->OnUnhovered.AddDynamic(this, &UInventoryToolBar::OnToolButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(ToolTypeButton, EItemType::Tool);
    }
    //@Material 버튼
    if (MaterialTypeButton)
    {
        //@바인딩
        MaterialTypeButton->OnClicked.AddDynamic(this, &UInventoryToolBar::OnMaterialButtonClicked);
        MaterialTypeButton->OnHovered.AddDynamic(this, &UInventoryToolBar::OnMaterialButtonHovered);
        MaterialTypeButton->OnUnhovered.AddDynamic(this, &UInventoryToolBar::OnMaterialButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(MaterialTypeButton, EItemType::Material);
    }
    //@Memory 버튼
    if (MemoryTypeButton)
    {
        //@바인딩
        MemoryTypeButton->OnClicked.AddDynamic(this, &UInventoryToolBar::OnMemoryButtonClicked);
        MemoryTypeButton->OnHovered.AddDynamic(this, &UInventoryToolBar::OnMemoryButtonHovered);
        MemoryTypeButton->OnUnhovered.AddDynamic(this, &UInventoryToolBar::OnMemoryButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(MemoryTypeButton, EItemType::Memory);
    }
    //@Equipment 버튼 
    if (EquipmentTypeButton)
    {
        //@바인딩
        EquipmentTypeButton->OnClicked.AddDynamic(this, &UInventoryToolBar::OnEquipmentButtonClicked);
        EquipmentTypeButton->OnHovered.AddDynamic(this, &UInventoryToolBar::OnEquipmentButtonHovered);
        EquipmentTypeButton->OnUnhovered.AddDynamic(this, &UInventoryToolBar::OnEquipmentButtonUnhovered);
        //@MCategoryButtons
        MCategoryButtons.Add(EquipmentTypeButton, EItemType::Equipment);
    }
}

void UInventoryToolBar::UpdateButtonStyle(UButton* SelectedButton, UButton* PreviousButton)
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

void UInventoryToolBar::HandleButtonClick(EItemType Category)
{
    //@Selected
    UButton* SelectedButton = *MCategoryButtons.FindKey(Category);
    //@Previous
    UButton* PreviousButton = *MCategoryButtons.FindKey(CurrentCategory);
    //@Update Style
    if (SelectedButton != PreviousButton)
    {
        UpdateButtonStyle(SelectedButton, PreviousButton);
    }
    //@Current Category
    CurrentCategory = Category;
    //@Delegate
    if (InventoryToolBarButtonClikced.IsBound())
    {
        InventoryToolBarButtonClikced.Execute(Category);
    }

    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼이 클릭되었습니다.", *UEnum::GetValueAsString(Category));
}

void UInventoryToolBar::HandleButtonHover(EItemType Category)
{
    UE_LOGFMT(LogInventoryToolBar, Log, "{0} 버튼에 마우스가 올라갔습니다.", *UEnum::GetValueAsString(Category));

    //@TODO: Hover Image 설정
}

void UInventoryToolBar::HandleButtonUnhover(EItemType Category)
{
    UE_LOGFMT(LogInventoryToolBar, Log, "버튼에서 마우스가 벗어났습니다.");

    //@TODO: Hover Image 관련 설정
}
#pragma endregion

#pragma region Callbacks
// Callbacks are already implemented in the header file using FORCEINLINE
#pragma endregion