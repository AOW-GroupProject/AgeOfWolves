#include "DropDownMenu.h"
#include "Logging/StructuredLog.h"

#include "08_UI/CustomButton.h"

DEFINE_LOG_CATEGORY(LogDropDownMenu)

#pragma region Default Setting
UDropDownMenu::UDropDownMenu(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UDropDownMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDropDownMenu::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UDropDownMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDropDownMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDropDownMenu::InternalBindToItemSlotButton(UCustomButton* InItemSlotButton)
{
    if (!InItemSlotButton)
    {
        UE_LOGFMT(LogDropDownMenu, Error, "InItemSlotButton이 null입니다. 바인딩을 수행할 수 없습니다.");
        return;
    }

    //@Button의 상태 별 이벤트에 바인딩 아래에서 수행...
    //@내부 바인딩
    //InItemSlotButton->ButtonHovered.AddUObject(this, &UDropDownMenu::OnItemSlotButtonHovered);
    //InItemSlotButton->ButtonSelected.AddUObject(this, &UDropDownMenu::OnItemSlotButtonClicked);
    //InItemSlotButton->ButtonUnhovered.AddUObject(this, &UDropDownMenu::OnItemSlotButtonUnhovered);

    UE_LOGFMT(LogInteractableItemSlot, Verbose, "CustomButton 이벤트가 성공적으로 바인딩되었습니다.");
}

void UDropDownMenu::InitializeDropDownMenu()
{
}
#pragma endregion
