#include "HUD_QuickSlotsUI.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "08_UI/ItemSlot.h"
#include "08_UI/01_HUD/HUD_QuickSlotsUI_AbilitySlot.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogQuickSlotsUI)

//@Defualt Setting
#pragma region Default Setting
UHUD_QuickSlotsUI::UHUD_QuickSlotsUI(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	QuickSlotsBox = nullptr;

	ToolItemQuickSlotBox = nullptr;

    BattoujutsuQuickSlotOverlay = nullptr;

    JujutsuQuickSlotsOverlay = nullptr;
    JujutsuExtraQuickSlotBox = nullptr;

    ToolItemSlots.Reset();
    ToolItemSlotClass = nullptr;

    BattoujutsuAbilitySlotRef = nullptr;
    BattoujutsuAbilitySlotClass = nullptr;

    JujutsuAbilitySlotClass = nullptr;
    ExtraJujutsuAbilitySlotClass = nullptr;
}

void UHUD_QuickSlotsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    //@외부 바인딩
    ExternalBindToASC();
}

void UHUD_QuickSlotsUI::ExternalBindToASC()
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Game Insatnce가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    APlayerController* PC = GameInstance->GetFirstLocalPlayerController(GetWorld());
    if (!PC)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Player Controller가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    ABasePlayerController* BasePC = Cast< ABasePlayerController>(PC);
    if (!BasePC)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Base Player Controller가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    APlayerStateBase* PS = BasePC->GetPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Player State가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    if (!ASC)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Base ASC가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    ASC->AbilitySpecGiven.AddUFunction(this, "OnAbilitySpecGiven");
  
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

void UHUD_QuickSlotsUI::InternalBindToJujutsuAbilitySlots(UAbilitySlot* AbilitySlot)
{
    if (!AbilitySlot)
    {
        return;
    }

    //@내부 바인딩
    AbilitySlot->AbilitySlotInitFinished.BindUFunction(this, "OnJujutsuAbilitySlotsInitFinished");
}

void UHUD_QuickSlotsUI::InitializeQuickSlotsUI()
{
	//@Tool Item 퀵슬롯 생성
	CreateToolItemQuickSlots();
    //@발도술 슬롯 생성
    CreateBattoujutsuAbilitySlot();
    //@주술 슬롯 목록 생성
    CreateJujutsuAbilitySlot();

    //@초기화 요청
    RequestStartInitByQuickSlots.Broadcast();
}

void UHUD_QuickSlotsUI::CheckAllUIsInitFinished()
{
    if (bToolItemSlotInitFinished && bBattouJutsuAbilitySlotInitFinished && bJujutsuAbilitySlotsInitFinished)
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

void UHUD_QuickSlotsUI::CreateBattoujutsuAbilitySlot()
{
    //@BP 클래스, HorizontalBox 체크
    if (!ensureMsgf(BattoujutsuAbilitySlotClass && BattoujutsuQuickSlotOverlay,
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
    if (UOverlaySlot* HorizontalBoxSlot = BattoujutsuQuickSlotOverlay->AddChildToOverlay(BattoujutsuAbilitySlot))
    {
        HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
        HorizontalBoxSlot->SetVerticalAlignment(VAlign_Top);
    }

    //@캐싱
    BattoujutsuAbilitySlotRef = BattoujutsuAbilitySlot;

    UE_LOGFMT(LogQuickSlotsUI, Log, "발도술 Ability Slot이 생성되어 추가되었습니다.");
}

void UHUD_QuickSlotsUI::CreateJujutsuAbilitySlot()
{
    //@BP 클래스, Jujutsu Quick Slot Box(Horizontal), Jujutsu Extra Quick Slot Box(Vertical)
    if (!ensureMsgf(JujutsuAbilitySlotClass && JujutsuQuickSlotsOverlay &&
        ExtraJujutsuAbilitySlotClass && JujutsuExtraQuickSlotBox,
        TEXT("Jujutsu 관련 클래스 또는 Box가 유효하지 않습니다.")))
    {
        return;
    }

    //@메인 주술 슬롯(1번 슬롯)
    UHUD_QuickSlotsUI_AbilitySlot* MainJujutsuSlot = CreateWidget<UHUD_QuickSlotsUI_AbilitySlot>(this, JujutsuAbilitySlotClass);
    if (IsValid(MainJujutsuSlot))
    {
        //@비동기 초기화
        RequestStartInitByQuickSlots.AddUFunction(MainJujutsuSlot, "InitializeAbilitySlot");

        //@Add Child To Overlay
        if (UOverlaySlot* HBoxSlot = JujutsuQuickSlotsOverlay->AddChildToOverlay(MainJujutsuSlot))
        {
            HBoxSlot->SetHorizontalAlignment(HAlign_Fill);
            HBoxSlot->SetVerticalAlignment(VAlign_Fill);
        }

        //@캐싱
        JujutsuSlots.MainSlot = MainJujutsuSlot;
    }

    //@대기 주술 슬롯(2번, 3번 슬롯)
    for (int32 i = 0; i < 2; ++i)
    {
        UAbilitySlot* ExtraSlot = CreateWidget<UAbilitySlot>(this, ExtraJujutsuAbilitySlotClass);
        if (IsValid(ExtraSlot))
        {
            RequestStartInitByQuickSlots.AddUFunction(ExtraSlot, "InitializeAbilitySlot");

            i == 1 ? InternalBindToJujutsuAbilitySlots(ExtraSlot) : nullptr;

            if (UVerticalBoxSlot* VBoxSlot = JujutsuExtraQuickSlotBox->AddChildToVerticalBox(ExtraSlot))
            {
                VBoxSlot->SetHorizontalAlignment(HAlign_Left);
                VBoxSlot->SetVerticalAlignment(VAlign_Bottom);
            }

            //@캐싱
            JujutsuSlots.ExtraSlots.Add(ExtraSlot);
        }
    }
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

void UHUD_QuickSlotsUI::OnAbilitySpecGiven(FGameplayAbilitySpec AbilitySpec)
{
    //@Ability, BattoujutsuRef
    if (!AbilitySpec.Ability || !BattoujutsuAbilitySlotRef)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Ability 또는 BattoujutsuRef가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    if (BattoujutsuAbilitySlotRef->GetIsFilled())
    {
        return;
    }

    //@Ability Tag 목록
    FGameplayTagContainer AbilityTags = AbilitySpec.Ability->AbilityTags;
    if (AbilityTags.IsEmpty())
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Ability Tag가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    //@발도술 태그
    for (auto AbilityTag : AbilityTags)
    {
        FString TagString = AbilityTag.ToString();

        if (TagString.StartsWith("Ability.Active.Battoujutsu"))
        {
            UE_LOGFMT(LogQuickSlotsUI, Log, "발도술 태그 감지됨: {0}", TagString);
            BattoujutsuAbilitySlotRef->AssignNewAbility(AbilityTag);
            break;
        }
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion