#include "HUD_QuickSlotsUI.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/ItemSlot.h"

DEFINE_LOG_CATEGORY(LogQuickSlotsUI)

//@Defualt Setting
#pragma region Default Setting
UHUD_QuickSlotsUI::UHUD_QuickSlotsUI(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	QuickSlotsBox = nullptr;
	ToolItemQuickSlotBox = nullptr;
	BattoujutsuQuickSlotBox = nullptr;
	JujutsuQuickSlotBox = nullptr;
}

void UHUD_QuickSlotsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UHUD_QuickSlotsUI::InitializeQuickSlotsUI()
{
	//@Tool Item 퀵슬롯 생성
	CreateToolItemQuickSlots();

	//@TODO: 임시 초기화 완료 이벤트 호출
	QuickSlotsInitFinished.ExecuteIfBound();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UHUD_QuickSlotsUI::CreateToolItemQuickSlots()
{
    //@Tool Item Slots 배열 초기화
    ToolItemSlots.Empty();

    //@Tool Item Slot Class, Tool Item Quick Slot Box 체크
    if (!ensureMsgf(ToolItemSlotClass && ToolItemQuickSlotBox, TEXT("ToolItemSlotClass 또는 ToolItemQuickSlotBox가 유효하지 않습니다.")))
    {
        return;
    }

    //@Tool Item Slots 생성 및 추가
    for (int32 i = 0; i < 2; ++i)
    {
        UItemSlot* ToolItemSlot = CreateWidget<UItemSlot>(this, ToolItemSlotClass);
        if (!IsValid(ToolItemSlot))
        {
            UE_LOGFMT(LogQuickSlotsUI, Error, "Tool Item Slot 위젯 생성에 실패했습니다.");
            continue;
        }

        //@Alignment 설정
        if (UHorizontalBoxSlot* HorizontalBoxSlot = ToolItemQuickSlotBox->AddChildToHorizontalBox(ToolItemSlot))
        {
            HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            HorizontalBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }

        //@배열에 추가
        ToolItemSlots.Add(ToolItemSlot);

        UE_LOGFMT(LogQuickSlotsUI, Log, "Tool Item Slot {0}이(가) 생성되어 추가되었습니다.", i);
    }

    UE_LOGFMT(LogQuickSlotsUI, Log, "총 {0}개의 Tool Item Slots가 생성되었습니다.", ToolItemSlots.Num());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

