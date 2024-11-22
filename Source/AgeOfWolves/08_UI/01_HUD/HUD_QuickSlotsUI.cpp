#include "HUD_QuickSlotsUI.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/ItemSlot.h"
#include "08_UI/01_HUD/HUD_QuickSlotsUI_AbilitySlot.h"


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

    ToolItemSlotClass = nullptr;
    BattoujutsuAbilitySlotClass = nullptr;
    JujutsuAbilitySlotClass = nullptr;

    ToolItemSlots.Reset();
}

void UHUD_QuickSlotsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UHUD_QuickSlotsUI::InternalBindToToolItemSlots(UItemSlot* ItemSlot, bool bLast)
{
    if (!ItemSlot)
    {
        return;
    }

    //@내부 바인딩
    ItemSlot->ItemSlotInitFinished.BindUFunction(this, "OnToolItemSlotsInitFinished");
}

void UHUD_QuickSlotsUI::InternalBindToBattoujutsuAbilitySlot(UHUD_QuickSlotsUI_AbilitySlot* AbilitySlot)
{
    if (!AbilitySlot)
    {
        return;
    }

    //@내부 바인딩
    AbilitySlot->AbilitySlotInitFinished.BindUFunction(this, "OnBattoujutsuAbilitySlotInitFinished");
}

void UHUD_QuickSlotsUI::InternalBindToJujutsuAbilitySlots(UHUD_QuickSlotsUI_AbilitySlot* AbilitySlot)
{
    
}

void UHUD_QuickSlotsUI::InitializeQuickSlotsUI()
{
	//@Tool Item 퀵슬롯 생성
	CreateToolItemQuickSlots();
    //@발도술 슬롯 생성
    CreateBattouJutsuAbilitySlot();
    //@주술 슬롯 목록 생성
    CreateJujutsuAbilitySlot();

    //@초기화 요청
    RequestStartInitByQuickSlots.Broadcast();
}

void UHUD_QuickSlotsUI::CheckAllUIsInitFinished()
{
    if (bToolItemSlotInitFinished && bBattouJutsuAbilitySlotInitFinished /* && bJujutsuAbilitySlotsInitFinished*/)
    {
        bToolItemSlotInitFinished = false;
        bBattouJutsuAbilitySlotInitFinished = false;
        bJujutsuAbilitySlotsInitFinished = false;

        //@초기화 완료 이벤트
	    QuickSlotsInitFinished.ExecuteIfBound();
    }
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

        //@초기화 요청 이벤트
        RequestStartInitByQuickSlots.AddUFunction(ToolItemSlot, "InitializeItemSlot");

        //@내부 바인딩
        InternalBindToToolItemSlots(ToolItemSlot);

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

void UHUD_QuickSlotsUI::CreateBattouJutsuAbilitySlot()
{
    //@BP 클래스, HorizontalBox 체크
    if (!ensureMsgf(BattoujutsuAbilitySlotClass && BattoujutsuQuickSlotBox,
        TEXT("BattoujutsuAbilitySlotClass 또는 BattoujutsuQuickSlotBox가 유효하지 않습니다.")))
    {
        return;
    }

    //@발도술 Ability Slot 생성
    UHUD_QuickSlotsUI_AbilitySlot* BattoujutsuAbilitySlot = CreateWidget<UHUD_QuickSlotsUI_AbilitySlot>(this, BattoujutsuAbilitySlotClass);
    if (!IsValid(BattoujutsuAbilitySlot))
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "발도술 Ability Slot 위젯 생성에 실패했습니다.");
        return;
    }

    //@초기화 요청 이벤트
    RequestStartInitByQuickSlots.AddUFunction(BattoujutsuAbilitySlot, "InitializeAbilitySlot");

    //@내부 바인딩
    InternalBindToBattoujutsuAbilitySlot(BattoujutsuAbilitySlot);

    //@Alignment 설정
    if (UHorizontalBoxSlot* HorizontalBoxSlot = BattoujutsuQuickSlotBox->AddChildToHorizontalBox(BattoujutsuAbilitySlot))
    {
        HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        HorizontalBoxSlot->SetVerticalAlignment(VAlign_Top);
    }

    UE_LOGFMT(LogQuickSlotsUI, Log, "발도술 Ability Slot이 생성되어 추가되었습니다.");
}

void UHUD_QuickSlotsUI::CreateJujutsuAbilitySlot()
{
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UHUD_QuickSlotsUI::OnToolItemSlotsInitFinished()
{
    bToolItemSlotInitFinished = true;

    CheckAllUIsInitFinished();
}
void UHUD_QuickSlotsUI::OnBattoujutsuAbilitySlotInitFinished()
{
    bBattouJutsuAbilitySlotInitFinished = true;

    CheckAllUIsInitFinished();
}
void UHUD_QuickSlotsUI::OnJujutsuAbilitySlotsInitFinished()
{
    bJujutsuAbilitySlotsInitFinished = true;

    CheckAllUIsInitFinished();
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

