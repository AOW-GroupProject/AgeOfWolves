// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_StatusUI.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "03_Player/PlayerStateBase.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"
#include "AbilitySystemComponent.h"

#include "08_UI/HorizontalDotGauge.h"

DEFINE_LOG_CATEGORY(LogStatusUI)

//@Defualt Setting
#pragma region Default Setting
UHUD_StatusUI::UHUD_StatusUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    StateBarVerticalBox = nullptr;

    MStateBars.Empty();
    HP = nullptr;
    SP = nullptr;

    ManaDotGaugeOverlay = nullptr;
    ManaDotGaugeRef = nullptr;
    ManaDotGaugeClass = nullptr;

    CachedASC = nullptr;
}

void UHUD_StatusUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // MStateBars 초기화
    FStateBarInfo& HealthInfo = MStateBars.Add("Health");
    HealthInfo.MainBar = HP;
    HealthInfo.OriginalColor = HP->GetFillColorAndOpacity();

    FStateBarInfo& StaminaInfo = MStateBars.Add("Stamina");
    StaminaInfo.MainBar = SP;
    StaminaInfo.OriginalColor = SP->GetFillColorAndOpacity();

    ExternalBindingToAttributeSet();
}

void UHUD_StatusUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UHUD_StatusUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHUD_StatusUI::NativeDestruct()
{
    //@AsyncTask의 End Task 호출
    for (auto* Listener : AttributeListeners)
    {
        if (Listener)
        {
            Listener->EndTask();
        }
    }

    //@Async Task 목록 비워주기
    AttributeListeners.Empty();

    Super::NativeDestruct();
}

void UHUD_StatusUI::InternalBindToManaDotGauge(UHorizontalDotGauge* ManaDotGauge)
{
    if (!ManaDotGauge)
    {
        return;
    }

    //@내부 바인딩
    ManaDotGauge->HorizontalDotGaugeInitFinished.BindUFunction(this, "OnManaDotGaugeInitFinished");
}

void UHUD_StatusUI::ExternalBindingToAttributeSet()
{
    //@Player State
    const auto PS = GetOwningPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogStatusUI, Error, "플레이어 스테이트가 유효하지 않습니다.");
        return;
    }
    //@ASC
    CachedASC = PS->GetAbilitySystemComponent();
    if (!CachedASC.IsValid())
    {
        UE_LOGFMT(LogStatusUI, Error, "어빌리티 시스템 컴포넌트가 유효하지 않습니다.");
        return;
    }
    //@AttributeSet
    const UAttributeSet* AttributeSet = CachedASC->GetAttributeSet(UBaseAttributeSet::StaticClass());
    if (!AttributeSet)
    {
        UE_LOGFMT(LogStatusUI, Warning, "어트리뷰트 셋이 유효하지 않습니다.");
        return;
    }
    //@BaseAttributeSet
    const auto BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
    if (!BaseAttributeSet)
    {
        return;
    }
    //@FGmaeplayAttribute::Health
    FGameplayAttribute HealthAttribute = BaseAttributeSet->GetHealthAttribute();
    if (HealthAttribute.IsValid())
    {
        auto* HealthListener = UAsyncTaskAttributeChanged::ListenToAttributeValueChange(
            CachedASC.Get(),
            HealthAttribute);
        if (IsValid(HealthListener))
        {
            HealthListener->OnAttributeValueChanged.AddDynamic(
                this,
                &UHUD_StatusUI::OnAttributeValueChanged);
            AttributeListeners.Add(HealthListener);
            UE_LOGFMT(LogStatusUI, Log, "체력 어트리뷰트 리스너가 생성되었습니다.");
        }
    }
    //@FGameplayAttribute::Stamina
    FGameplayAttribute StaminaAttribute = BaseAttributeSet->GetStaminaAttribute();
    if (StaminaAttribute.IsValid())
    {
        auto* StaminaListener = UAsyncTaskAttributeChanged::ListenToAttributeValueChange(
            CachedASC.Get(),
            StaminaAttribute);
        if (IsValid(StaminaListener))
        {
            StaminaListener->OnAttributeValueChanged.AddDynamic(
                this,
                &UHUD_StatusUI::OnAttributeValueChanged);
            AttributeListeners.Add(StaminaListener);
            UE_LOGFMT(LogStatusUI, Log, "스태미나 어트리뷰트 리스너가 생성되었습니다.");
        }
    }
    //@FGmaeplayAttribute::MaxMana
    FGameplayAttribute MaxManaAttribute = BaseAttributeSet->GetMaxManaAttribute();
    if (MaxManaAttribute.IsValid())
    {
        auto* MaxManaListener = UAsyncTaskAttributeChanged::ListenToAttributeValueChange(
            CachedASC.Get(),
            MaxManaAttribute);
        if (IsValid(MaxManaListener))
        {
            MaxManaListener->OnAttributeValueChanged.AddDynamic(
                this,
                &UHUD_StatusUI::OnAttributeValueChanged);
            AttributeListeners.Add(MaxManaListener);
            UE_LOGFMT(LogStatusUI, Log, "최대 마나 어트리뷰트 리스너가 생성되었습니다.");
        }
    }

    //@FGmaeplayAttribute::Mana
    FGameplayAttribute ManaAttribute = BaseAttributeSet->GetManaAttribute();
    if (ManaAttribute.IsValid())
    {
        auto* ManaListener = UAsyncTaskAttributeChanged::ListenToAttributeValueChange(
            CachedASC.Get(),
            ManaAttribute);
        if (IsValid(ManaListener))
        {
            ManaListener->OnAttributeValueChanged.AddDynamic(
                this,
                &UHUD_StatusUI::OnAttributeValueChanged);
            AttributeListeners.Add(ManaListener);
            UE_LOGFMT(LogStatusUI, Log, "마나 어트리뷰트 리스너가 생성되었습니다.");
        }
    }

}

void UHUD_StatusUI::InitializeStatusUI()
{

    //@Memory 아이템 생성
    CreateAndSetupMemoryImage();
    //@마나 카운트 이미지 생성
    CreateManaDotGauge();

    //@초기화 요청 이벤트
    RequestStartInitByStatusUI.Broadcast();
}

void UHUD_StatusUI::CheckAllUIsInitFinished()
{
    if (bManaDotGaugeInitFinished)
    {
        bManaDotGaugeInitFinished = false;

        StatusUIInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UHUD_StatusUI::CreateAndSetupMemoryImage()
{

    //@현재 캐릭터에 장착한 Memory 아이템 가져와서 업데이트

}

void UHUD_StatusUI::CreateManaDotGauge()
{
    //@ManaDotGaugeClass, ManaDotGaugeOverlay
    if (!IsValid(ManaDotGaugeClass) || !ManaDotGaugeOverlay)
    {
        UE_LOGFMT(LogStatusUI, Warning, "ManaDotGaugeClass 또는 ManaDotGaugeOverlay가 유효하지 않습니다.");
        return;
    }

    //@Create Widget
    UHorizontalDotGauge* ManaDotGauge = CreateWidget<UHorizontalDotGauge>(this, ManaDotGaugeClass);
    if (!IsValid(ManaDotGauge))
    {
        UE_LOGFMT(LogStatusUI, Error, "ManaDotGauge 위젯 생성에 실패했습니다.");
        return;
    }

    //@비동기 초기화
    RequestStartInitByStatusUI.AddUFunction(ManaDotGauge, "InitializeHorizontalDotGauge");

    //@내부 바인딩
    InternalBindToManaDotGauge(ManaDotGauge);

    //@Add Child To Overlay
    UOverlaySlot* OverlaySlot = ManaDotGaugeOverlay->AddChildToOverlay(ManaDotGauge);
    if (OverlaySlot)
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    //@Ref
    ManaDotGaugeRef = ManaDotGauge;

    UE_LOGFMT(LogStatusUI, Log, "마나 닷 게이지가 성공적으로 생성되었습니다.");
}

bool UHUD_StatusUI::UpdateStateBarAttribute(const FGameplayAttribute& Attribute, float OldValue, float NewValue, APlayerStateBase* PS)
{
    if (Attribute.AttributeName != "Health" && Attribute.AttributeName != "Stamina")
    {
        return false;
    }

    FStateBarInfo* BarInfo = MStateBars.Find(Attribute.AttributeName);
    if (!BarInfo || !BarInfo->MainBar)
    {
        UE_LOGFMT(LogStatusUI, Warning, "다음 어트리뷰트에 대한 상태바를 찾을 수 없습니다: {0}", Attribute.AttributeName);
        return true;
    }

    FString AttributeLookingFor = "Max" + Attribute.AttributeName;
    float MaxValue = PS->GetAttributeValue<float>(AttributeLookingFor);
    if (MaxValue == 0)
    {
        UE_LOGFMT(LogStatusUI, Warning, "다음 어트리뷰트의 최대값이 0입니다: {0}", AttributeLookingFor);
        return true;
    }

    //@감소 발생
    if (NewValue < OldValue)
    {
        float DamagePercent = (OldValue - NewValue) / MaxValue;
        float NewPercent = NewValue / MaxValue;

        //@기존 타이머 제거
        if (BarInfo->DamageColorTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(BarInfo->DamageColorTimer);
        }

        //@Set Fill Color And Opacity
        BarInfo->MainBar->SetFillColorAndOpacity(DAMAGE_COLOR);
        BarInfo->CurrentValue = NewPercent;
        BarInfo->DamageValue = DamagePercent;

        //@Timer
        FTimerDelegate TimerCallback;
        TimerCallback.BindLambda([this, BarInfo, NewPercent]()
            {
                BarInfo->MainBar->SetFillColorAndOpacity(BarInfo->OriginalColor);
                BarInfo->MainBar->SetPercent(NewPercent);
                BarInfo->Reset();
            });

        GetWorld()->GetTimerManager().SetTimer(
            BarInfo->DamageColorTimer,
            TimerCallback,
            DAMAGE_COLOR_DURATION,
            false
        );
    }
    else
    {
        //@Set Percent
        BarInfo->MainBar->SetPercent(NewValue / MaxValue);
    }

    UE_LOGFMT(LogStatusUI, Log, "{0} 현재 값: {1}", Attribute.AttributeName, FString::SanitizeFloat(NewValue));
    return true;
}

void UHUD_StatusUI::UpdateManaAttribute(const FGameplayAttribute& Attribute, float NewValue)
{
    //@Mana Dot Gauge Ref
    if (!ManaDotGaugeRef)
    {
        UE_LOGFMT(LogStatusUI, Warning, "ManaDotGaugeRef가 유효하지 않습니다.");
        return;
    }

    //@MaxMana 변경 시 (NewValue: 최대 마나 개수)
    if (Attribute.AttributeName == "MaxMana")
    {
        UE_LOGFMT(LogStatusUI, Log, "MaxMana 값 변경: {0}", FString::FromInt(static_cast<int32>(NewValue)));
        //@Update Max Count
        ManaDotGaugeRef->UpdateMaxCount(static_cast<int32>(NewValue));
    }
    //@현재 Mana 변경 시 (NewValue: 현재 보유 마나 개수)
    else if (Attribute.AttributeName == "Mana")
    {
        UE_LOGFMT(LogStatusUI, Log, "현재 Mana 값 변경: {0}", FString::FromInt(static_cast<int32>(NewValue)));
        //@Update Filled Count
        ManaDotGaugeRef->UpdateFilledCount(static_cast<int32>(NewValue));
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UHUD_StatusUI::OnManaDotGaugeInitFinished()
{
    bManaDotGaugeInitFinished = true;

    CheckAllUIsInitFinished();
}

void UHUD_StatusUI::OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
    //@Player State
    const auto PS = GetOwningPlayerState<APlayerStateBase>();
    if (!IsValid(PS))
    {
        UE_LOGFMT(LogStatusUI, Warning, "OnAttributeValueChanged에서 플레이어 스테이트가 유효하지 않습니다.");
        return;
    }
    //@Attribute
    if (!Attribute.IsValid())
    {
        UE_LOGFMT(LogStatusUI, Warning, "OnAttributeValueChanged에서 어트리뷰트가 유효하지 않습니다.");
        return;
    }

    //@HP, SP 어트리뷰트 처리
    if (UpdateStateBarAttribute(Attribute, OldValue, NewValue, PS))
    {
        return;
    }

    //@마나 어트리뷰트 처리
    UpdateManaAttribute(Attribute, NewValue);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion