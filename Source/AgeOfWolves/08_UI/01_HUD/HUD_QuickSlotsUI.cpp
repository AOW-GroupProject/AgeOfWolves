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
#include "04_Component/BaseInputComponent.h"
#include "04_Component/InventoryComponent.h"
#include "14_Subsystem/ItemManagerSubsystem.h"

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

}

void UHUD_QuickSlotsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    //@외부 바인딩
    ExternalBindToASC();
    ExternalBindToInputComp();
    ExternalBindToInventoryComp();

    // ItemManager 캐싱
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
        ItemManagerRef = GameInstance->GetSubsystem<UItemManagerSubsystem>();
        if (!ItemManagerRef.IsValid())
        {
            UE_LOGFMT(LogQuickSlotsUI, Error, "ItemManagerSubsystem 캐싱 실패");
        }
        else
        {
            UE_LOGFMT(LogQuickSlotsUI, Log, "ItemManagerSubsystem 캐싱 성공");
        }
    }
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

void UHUD_QuickSlotsUI::ExternalBindToInputComp()
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

    UBaseInputComponent* BaseInputComp = BasePC->GetBaseInputComponent();
    if (!BaseInputComp)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Base Input Component가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    //@외부 바인딩
    BaseInputComp->UIInputTagTriggered.AddUFunction(this, "OnChangeJujutsuToPrev");
    BaseInputComp->UIInputTagTriggered.AddUFunction(this, "OnChangeJujutsuToNext");
    BaseInputComp->UIInputTagTriggeredWithValue.AddUFunction(this, "OnUIInputTriggeredWithValue");

}

void UHUD_QuickSlotsUI::ExternalBindToInventoryComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawns
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Inven Comp
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogQuickSlotsUI, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@외부 바인딩
    InvenComp->QuickSlotItemsLoaded.AddUFunction(this, "OnQuickSlotItemsLoaded");
    InvenComp->QuickSlotItemUpdated.AddUFunction(this, "OnQuickSlotItemUpdated");
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
    if (!ensureMsgf(JujutsuQuickSlotInformation.JujutsuAbilitySlotClass && JujutsuQuickSlotsOverlay &&
        JujutsuQuickSlotInformation.ExtraJujutsuAbilitySlotClass && JujutsuExtraQuickSlotBox,
        TEXT("Jujutsu 관련 클래스 또는 Box가 유효하지 않습니다.")))
    {
        return;
    }

    //@메인 주술 슬롯(1번 슬롯)
    UHUD_QuickSlotsUI_AbilitySlot* MainJujutsuSlot = CreateWidget<UHUD_QuickSlotsUI_AbilitySlot>(this, JujutsuQuickSlotInformation.JujutsuAbilitySlotClass);
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

    //@AbilityInfos 초기화 (3개의 빈 슬롯 정보 생성)
    JujutsuSlots.AbilityInfos.SetNum(3);
    for (auto& Info : JujutsuSlots.AbilityInfos)
    {
        Info.Clear();
    }

    //@대기 주술 슬롯(2번, 3번 슬롯)
    const int32 ExtraSlotCount = JujutsuQuickSlotInformation.ExtraJujutsuAbilitySlotNum;
    for (int32 i = ExtraSlotCount - 1; i >= 0; --i)
    {
        UAbilitySlot* ExtraSlot = CreateWidget<UAbilitySlot>(this, JujutsuQuickSlotInformation.ExtraJujutsuAbilitySlotClass);
        if (IsValid(ExtraSlot))
        {
            RequestStartInitByQuickSlots.AddUFunction(ExtraSlot, "InitializeAbilitySlot");

            i == (ExtraSlotCount - 1) ? InternalBindToJujutsuAbilitySlots(ExtraSlot) : nullptr;

            //@대기 어빌리티 슬롯 크기 설정
            if (JujutsuQuickSlotInformation.ExtraJujutsuAbilitySlotsSizeInfo.IsValidIndex(i))
            {
                const auto& SlotSizes = JujutsuQuickSlotInformation.ExtraJujutsuAbilitySlotsSizeInfo[i];
                ExtraSlot->SetImagesSize(SlotSizes.BGImageSize, SlotSizes.AbilityImageSize);
            }

            //@Add Child To Vertical Box
            if (UVerticalBoxSlot* VBoxSlot = JujutsuExtraQuickSlotBox->AddChildToVerticalBox(ExtraSlot))
            {
                VBoxSlot->SetHorizontalAlignment(HAlign_Left);
                VBoxSlot->SetVerticalAlignment(VAlign_Bottom);
            }

            //@캐싱
            JujutsuSlots.ExtraSlots.Add(ExtraSlot);
        }
    }

    //@초기 메인 슬롯 인덱스 설정
    JujutsuSlots.CurrentMainIndex = 0;

    UE_LOGFMT(LogQuickSlotsUI, Log, "주술 슬롯 생성 완료 - Main: 1, Extra: {0}", ExtraSlotCount);
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
    //@Ability
    if (!AbilitySpec.Ability)
    {
        UE_LOG(LogQuickSlotsUI, Error, TEXT("%s: Ability가 유효하지 않습니다."), *FString(__FUNCTION__));
        return;
    }

    //@Ability Tags
    FGameplayTagContainer AbilityTags = AbilitySpec.Ability->AbilityTags;
    if (AbilityTags.IsEmpty())
    {
        UE_LOG(LogQuickSlotsUI, Error, TEXT("%s: Ability Tag가 유효하지 않습니다."), *FString(__FUNCTION__));
        return;
    }

    //@Ability Tag
    for (auto AbilityTag : AbilityTags)
    {
        FString TagString = AbilityTag.ToString();

        //@Battoujutsu Ability
        if (TagString.StartsWith("Ability.Passive.Battoujutsu"))
        {
            if (BattoujutsuAbilitySlotRef && !BattoujutsuAbilitySlotRef->GetIsFilled())
            {
                UE_LOG(LogQuickSlotsUI, Log, TEXT("발도술 태그 감지됨: %s"), *TagString);
                BattoujutsuAbilitySlotRef->AssignNewAbility(AbilityTag);
            }
            return;
        }

        //@Jujutsu Ability
        if (TagString.StartsWith("Ability.Active.Jujutsu"))
        {
            UE_LOG(LogQuickSlotsUI, Log, TEXT("주술 태그 감지됨: %s"), *TagString);

            // 빈 슬롯 찾기
            int32 EmptySlotIndex = JujutsuSlots.AbilityInfos.IndexOfByPredicate([](const FJujutsuAbilitySlotInfo& Info) {
                return !Info.bIsFilled;
                });

            if (EmptySlotIndex == INDEX_NONE)
            {
                UE_LOG(LogQuickSlotsUI, Warning, TEXT("모든 주술 슬롯이 찼습니다. Tag: %s를 할당할 수 없습니다."), *TagString);
                return;
            }

            //@정보 저장
            FJujutsuAbilitySlotInfo& SlotInfo = JujutsuSlots.AbilityInfos[EmptySlotIndex];
            SlotInfo.AbilityTag = AbilityTag;
            SlotInfo.bIsFilled = true;

            //@UI 업데이트
            if (EmptySlotIndex == JujutsuSlots.CurrentMainIndex)
            {
                if (JujutsuSlots.MainSlot)
                {
                    JujutsuSlots.MainSlot->AssignNewAbility(AbilityTag);
                }
            }
            else
            {
                int32 ExtraIndex = (EmptySlotIndex < JujutsuSlots.CurrentMainIndex) ?
                    EmptySlotIndex : EmptySlotIndex - 1;

                if (ExtraIndex < JujutsuSlots.ExtraSlots.Num())
                {
                    JujutsuSlots.ExtraSlots[ExtraIndex]->AssignNewAbility(AbilityTag);
                }
            }

            UE_LOG(LogQuickSlotsUI, Log, TEXT("주술 할당됨 - 슬롯 인덱스: %d, Tag: %s"), EmptySlotIndex, *TagString);
            return;
        }
    }
}

void UHUD_QuickSlotsUI::OnChangeJujutsuToPrev(const FGameplayTag& InputTag)
{
    //@Tag 유효성 검사
    if (!InputTag.IsValid() || !InputTag.ToString().StartsWith("Input.UI.QuickSlots.MoveJujutsuSlotToPrev"))
    {
        return;
    }

    //@슬롯 유효성 검사
    if (JujutsuSlots.AbilityInfos.Num() < 3 || !JujutsuSlots.MainSlot || JujutsuSlots.ExtraSlots.Num() < 2)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "주술 슬롯 순환에 필요한 슬롯이 부족합니다.");
        return;
    }

    //@반시계 방향 회전 (마우스 휠 위로 올리는 것과 동일)
    JujutsuSlots.CurrentMainIndex = (JujutsuSlots.CurrentMainIndex + 2) % 3;  // -1 + 3

    //@메인 주술 어빌리티 슬롯
    if (JujutsuSlots.AbilityInfos[JujutsuSlots.CurrentMainIndex].bIsFilled)
    {
        JujutsuSlots.MainSlot->AssignNewAbility(JujutsuSlots.AbilityInfos[JujutsuSlots.CurrentMainIndex].AbilityTag);
    }
    else
    {
        JujutsuSlots.MainSlot->ClearAssignedAbility();
    }

    //@대기 주술 어빌리티 슬롯
    int32 firstExtraIndex = (JujutsuSlots.CurrentMainIndex + 1) % 3;
    int32 secondExtraIndex = (JujutsuSlots.CurrentMainIndex + 2) % 3;

    if (JujutsuSlots.AbilityInfos[firstExtraIndex].bIsFilled)
    {
        JujutsuSlots.ExtraSlots[0]->AssignNewAbility(JujutsuSlots.AbilityInfos[firstExtraIndex].AbilityTag);
    }
    else
    {
        JujutsuSlots.ExtraSlots[0]->ClearAssignedAbility();
    }

    if (JujutsuSlots.AbilityInfos[secondExtraIndex].bIsFilled)
    {
        JujutsuSlots.ExtraSlots[1]->AssignNewAbility(JujutsuSlots.AbilityInfos[secondExtraIndex].AbilityTag);
    }
    else
    {
        JujutsuSlots.ExtraSlots[1]->ClearAssignedAbility();
    }

    UE_LOGFMT(LogQuickSlotsUI, Log, "주술 슬롯 반시계 방향 순환 완료 - Main Index: {0}, Extra Indices: {1}, {2}",
        JujutsuSlots.CurrentMainIndex, firstExtraIndex, secondExtraIndex);
}

void UHUD_QuickSlotsUI::OnChangeJujutsuToNext(const FGameplayTag& InputTag)
{
    //@Tag 유효성 검사
    if (!InputTag.IsValid() || !InputTag.ToString().StartsWith("Input.UI.QuickSlots.MoveJujutsuSlotToNext"))
    {
        return;
    }

    //@슬롯 유효성 검사
    if (JujutsuSlots.AbilityInfos.Num() < 3 || !JujutsuSlots.MainSlot || JujutsuSlots.ExtraSlots.Num() < 2)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "주술 슬롯 순환에 필요한 슬롯이 부족합니다.");
        return;
    }

    //@시계 방향 회전 (마우스 휠 아래로 내리는 것과 동일)
    JujutsuSlots.CurrentMainIndex = (JujutsuSlots.CurrentMainIndex + 1) % 3;

    //@메인 주술 어빌리티 슬롯
    if (JujutsuSlots.AbilityInfos[JujutsuSlots.CurrentMainIndex].bIsFilled)
    {
        JujutsuSlots.MainSlot->AssignNewAbility(JujutsuSlots.AbilityInfos[JujutsuSlots.CurrentMainIndex].AbilityTag);
    }
    else
    {
        JujutsuSlots.MainSlot->ClearAssignedAbility();
    }

    //@대기 주술 어빌리티 슬롯
    int32 firstExtraIndex = (JujutsuSlots.CurrentMainIndex + 1) % 3;
    int32 secondExtraIndex = (JujutsuSlots.CurrentMainIndex + 2) % 3;

    if (JujutsuSlots.AbilityInfos[firstExtraIndex].bIsFilled)
    {
        JujutsuSlots.ExtraSlots[0]->AssignNewAbility(JujutsuSlots.AbilityInfos[firstExtraIndex].AbilityTag);
    }
    else
    {
        JujutsuSlots.ExtraSlots[0]->ClearAssignedAbility();
    }

    if (JujutsuSlots.AbilityInfos[secondExtraIndex].bIsFilled)
    {
        JujutsuSlots.ExtraSlots[1]->AssignNewAbility(JujutsuSlots.AbilityInfos[secondExtraIndex].AbilityTag);
    }
    else
    {
        JujutsuSlots.ExtraSlots[1]->ClearAssignedAbility();
    }

    UE_LOGFMT(LogQuickSlotsUI, Log, "주술 슬롯 시계 방향 순환 완료 - Main Index: {0}, Extra Indices: {1}, {2}",
        JujutsuSlots.CurrentMainIndex, firstExtraIndex, secondExtraIndex);
}

void UHUD_QuickSlotsUI::OnUIInputTriggeredWithValue(const FGameplayTag& InputTag, const float AxisValue)
{
    //@Tag 유효성 검사
    if (!InputTag.IsValid())
    {
        return;
    }

    //@Change Jujutsu
    FString TagString = InputTag.ToString();
    if (!TagString.StartsWith("Input.UI.QuickSlots.ChangeJujutsu"))
    {
        return;
    }

    //@시간 체크 및 누적값 계산
    double CurrentTime = FPlatformTime::Seconds();
    if (!JujutsuRotationState.bIsAccumulating ||
        (CurrentTime - JujutsuRotationState.LastInputTime) >= AXIS_ACCUMULATE_TIME)
    {
        JujutsuRotationState.Reset();
        JujutsuRotationState.bIsAccumulating = true;
    }

    JujutsuRotationState.LastInputTime = CurrentTime;
    JujutsuRotationState.AccumulatedValue += AxisValue;

    //@임계값 체크
    if (FMath::Abs(JujutsuRotationState.AccumulatedValue) >= MAX_THRESHOLD)
    {
        //@슬롯 유효성 검사
        if (JujutsuSlots.AbilityInfos.Num() < 3 || !JujutsuSlots.MainSlot || JujutsuSlots.ExtraSlots.Num() < 2)
        {
            UE_LOGFMT(LogQuickSlotsUI, Warning, "주술 슬롯 순환에 필요한 슬롯이 부족합니다.");
            JujutsuRotationState.Reset();
            return;
        }

        //@회전 방향 결정
        bool bForward = JujutsuRotationState.AccumulatedValue > 0;
        UE_LOGFMT(LogQuickSlotsUI, Log, "주술 슬롯 {0} 방향 순환", bForward ? TEXT("시계") : TEXT("반시계"));

        //@마우스 휠 위로(양수) -> 반 시계 방향 회전
        if (bForward)
        {
            JujutsuSlots.CurrentMainIndex = (JujutsuSlots.CurrentMainIndex + 2) % 3;  // -1 + 3
        }
        //@마우스 휠 아래(음수) -> 시계 방향 회전
        else 
        {
            JujutsuSlots.CurrentMainIndex = (JujutsuSlots.CurrentMainIndex + 1) % 3;
        }

        //@메인 주술 어빌리티 슬롯
        if (JujutsuSlots.AbilityInfos[JujutsuSlots.CurrentMainIndex].bIsFilled)
        {
            JujutsuSlots.MainSlot->AssignNewAbility(JujutsuSlots.AbilityInfos[JujutsuSlots.CurrentMainIndex].AbilityTag);
        }
        else
        {
            JujutsuSlots.MainSlot->ClearAssignedAbility();
        }

        //@대기 주술 어빌리티 슬롯
        int32 firstExtraIndex = (JujutsuSlots.CurrentMainIndex + 1) % 3;
        int32 secondExtraIndex = (JujutsuSlots.CurrentMainIndex + 2) % 3;

        if (JujutsuSlots.AbilityInfos[firstExtraIndex].bIsFilled)
        {
            JujutsuSlots.ExtraSlots[0]->AssignNewAbility(JujutsuSlots.AbilityInfos[firstExtraIndex].AbilityTag);
        }
        else
        {
            JujutsuSlots.ExtraSlots[0]->ClearAssignedAbility();
        }

        if (JujutsuSlots.AbilityInfos[secondExtraIndex].bIsFilled)
        {
            JujutsuSlots.ExtraSlots[1]->AssignNewAbility(JujutsuSlots.AbilityInfos[secondExtraIndex].AbilityTag);
        }
        else
        {
            JujutsuSlots.ExtraSlots[1]->ClearAssignedAbility();
        }

        UE_LOGFMT(LogQuickSlotsUI, Log, "주술 슬롯 순환 완료 - Main Index: {0}, Extra Indices: {1}, {2}",
            JujutsuSlots.CurrentMainIndex, firstExtraIndex, secondExtraIndex);

        JujutsuRotationState.Reset();
    }
}

void UHUD_QuickSlotsUI::OnQuickSlotItemsLoaded(int32 QuickSlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
    //@Item Type
    if (ItemType != EItemType::Tool)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 로드 실패: 잘못된 아이템 타입 - {0}", UEnum::GetValueAsString(ItemType));
        return;
    }

    //@Quick Slot 번호
    if (QuickSlotNum < 0 || QuickSlotNum > 2)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 로드 실패: 잘못된 슬롯 번호 - {0}", QuickSlotNum);
        return;
    }

    //@ToolItemSlots
    if (!ToolItemSlots.IsValidIndex(QuickSlotNum - 1))
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 로드 실패: 유효하지 않은 Tool Item Slots 인덱스 - {0}", QuickSlotNum - 1);
        return;
    }

    //@Item Info
    if (!ItemManagerRef.IsValid())
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 로드 실패: Item Manager가 유효하지 않음");
        return;
    }

    const FItemInformation* ItemInfoPtr = ItemManagerRef->GetItemInformation<FItemInformation>(ItemType, ItemTag);
    if (!ItemInfoPtr)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 로드 실패: 아이템 정보를 찾을 수 없음 - Tag: {0}", ItemTag.ToString());
        return;
    }

    FItemInformation ItemInfo = *ItemInfoPtr;
    ToolItemSlots[QuickSlotNum - 1]->AssignNewItem(UniqueItemID, ItemInfo);

    UE_LOGFMT(LogQuickSlotsUI, Log, "퀵슬롯 {0}번에 아이템 할당 성공 - ID: {1}, 타입: {2}, 태그: {3}, 개수: {4}",
        QuickSlotNum, UniqueItemID.ToString(), UEnum::GetValueAsString(ItemType), ItemTag.ToString(), ItemCount);
}

void UHUD_QuickSlotsUI::OnQuickSlotItemUpdated(int32 QuickSlotNum, const FGuid& UniqueItemID, int32 ItemNum)
{
    int QuickSlotIndex = QuickSlotNum - 1;

    //@Quick Slot 번호
    if (QuickSlotIndex < 0 || QuickSlotIndex > 2)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 업데이트 실패: 잘못된 슬롯 인덱스 - {0}", QuickSlotIndex);
        return;
    }

    //@Tool Item Slots
    if (!ToolItemSlots.IsValidIndex(QuickSlotIndex))
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 업데이트 실패: 유효하지 않은 Tool Item Slots 인덱스 - {0}", QuickSlotIndex);
        return;
    }

    //@FGuid
    if (ToolItemSlots[QuickSlotIndex]->GetUniqueItemID() != UniqueItemID)
    {
        UE_LOGFMT(LogQuickSlotsUI, Warning, "퀵슬롯 업데이트 실패: ID 불일치 - 슬롯 ID: {0}, 요청 ID: {1}",
            ToolItemSlots[QuickSlotIndex]->GetUniqueItemID().ToString(), UniqueItemID.ToString());
        return;
    }

    //@Update
    ToolItemSlots[QuickSlotIndex]->UpdateItemCount(ItemNum);

    UE_LOGFMT(LogQuickSlotsUI, Log, "퀵슬롯 {0}번 아이템 업데이트 성공 - ID: {1}, 새 개수: {2}",
        QuickSlotNum, UniqueItemID.ToString(), ItemNum);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion