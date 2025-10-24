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
    CachedPlayerState = nullptr;
    
    //@캐시 값 초기화
    LastHealthValue = -1.f;
    LastStaminaValue = -1.f;
    LastManaValue = -1.f;
    LastMaxManaValue = -1.f;
    LastUpdateTime = 0.f;
    
    //@보간 설정 초기화
    HealthInterpolationSettings.Duration = 0.5f;
    HealthInterpolationSettings.EasingFunction = EEasingFunc::EaseOut;
    HealthInterpolationSettings.Exponent = 2.0f;
    HealthInterpolationSettings.bEnabled = true;
    HealthInterpolationSettings.bUseExponentialEasing = false;
    
    StaminaInterpolationSettings.Duration = 0.3f;
    StaminaInterpolationSettings.EasingFunction = EEasingFunc::EaseOut;
    StaminaInterpolationSettings.Exponent = 2.0f;
    StaminaInterpolationSettings.bEnabled = true;
    StaminaInterpolationSettings.bUseExponentialEasing = false;
}

void UHUD_StatusUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@MStateBars
    FStateBarInfo& HealthInfo = MStateBars.Add("Health");
    HealthInfo.MainBar = HP;
    HealthInfo.OriginalColor = HP->GetFillColorAndOpacity();

    FStateBarInfo& StaminaInfo = MStateBars.Add("Stamina");
    StaminaInfo.MainBar = SP;
    StaminaInfo.OriginalColor = SP->GetFillColorAndOpacity();

    //@플레이어 스테이트 캐시
    CachePlayerState();

    //@외부 바인딩
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
    
    //@보간 타이머 정리
    CleanupInterpolationTimers();

    Super::NativeDestruct();
}

void UHUD_StatusUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    //@보간 업데이트
    for (auto& StateBarPair : MStateBars)
    {
        FStateBarInfo& BarInfo = StateBarPair.Value;
        if (BarInfo.InterpolationData.bIsActive && !BarInfo.InterpolationData.bIsCompleted)
        {
            UpdateInterpolation(BarInfo, InDeltaTime);
        }
    }
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

    float OldPercent = OldValue / MaxValue;
    float NewPercent = NewValue / MaxValue;

    //@기존 보간 중단
    if (BarInfo->InterpolationTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(BarInfo->InterpolationTimer);
    }

    //@감소 발생 (데미지)
    if (NewValue < OldValue)
    {
        float DamagePercent = (OldValue - NewValue) / MaxValue;
        BarInfo->DamageValue = DamagePercent;

        //@기존 데미지 타이머 제거
        if (BarInfo->DamageColorTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(BarInfo->DamageColorTimer);
        }

        //@데미지 색상 설정
        BarInfo->MainBar->SetFillColorAndOpacity(DAMAGE_COLOR);

        //@보간 설정 가져오기
        FInterpolationSettings Settings = (Attribute.AttributeName == "Health") ? 
            HealthInterpolationSettings : StaminaInterpolationSettings;

        //@보간 활성화된 경우
        if (Settings.bEnabled)
        {
            StartInterpolation(*BarInfo, OldPercent, NewPercent, Settings);
        }
        else
        {
            //@즉시 설정
            BarInfo->MainBar->SetPercent(NewPercent);
        }

        //@데미지 색상 복원 타이머
        FTimerDelegate DamageColorCallback;
        DamageColorCallback.BindLambda([this, BarInfo, NewPercent]()
            {
                BarInfo->MainBar->SetFillColorAndOpacity(BarInfo->OriginalColor);
                BarInfo->MainBar->SetPercent(NewPercent);
                BarInfo->Reset();
            });

        GetWorld()->GetTimerManager().SetTimer(
            BarInfo->DamageColorTimer,
            DamageColorCallback,
            DAMAGE_COLOR_DURATION,
            false
        );
    }
    else
    {
        //@회복 또는 증가
        FInterpolationSettings Settings = (Attribute.AttributeName == "Health") ? 
            HealthInterpolationSettings : StaminaInterpolationSettings;

        if (Settings.bEnabled)
        {
            StartInterpolation(*BarInfo, OldPercent, NewPercent, Settings);
        }
        else
        {
            BarInfo->MainBar->SetPercent(NewPercent);
        }
    }

    //@캐시 값 업데이트
    if (Attribute.AttributeName == "Health")
    {
        LastHealthValue = NewValue;
    }
    else if (Attribute.AttributeName == "Stamina")
    {
        LastStaminaValue = NewValue;
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
        //@캐시 값 업데이트
        LastMaxManaValue = NewValue;
    }
    //@현재 Mana 변경 시 (NewValue: 현재 보유 마나 개수)
    else if (Attribute.AttributeName == "Mana")
    {
        UE_LOGFMT(LogStatusUI, Log, "현재 Mana 값 변경: {0}", FString::FromInt(static_cast<int32>(NewValue)));
        //@Update Filled Count
        ManaDotGaugeRef->UpdateFilledCount(static_cast<int32>(NewValue));
        //@캐시 값 업데이트
        LastManaValue = NewValue;
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
    //@성능 최적화: 업데이트 빈도 제한
    OptimizeUpdateFrequency();
    
    //@어트리뷰트 변경 유효성 검사
    if (!ValidateAttributeChange(Attribute, OldValue, NewValue))
    {
        return;
    }
    
    //@중복 업데이트 방지
    if (!ShouldUpdateAttribute(Attribute.AttributeName, NewValue))
    {
        return;
    }

    //@변경 로그
    LogAttributeChange(Attribute.AttributeName, OldValue, NewValue);

    //@Player State (캐시된 것 사용)
    APlayerStateBase* PS = CachedPlayerState.Get();
    if (!IsValid(PS))
    {
        //@캐시가 유효하지 않으면 다시 가져오기
        PS = GetOwningPlayerState<APlayerStateBase>();
        if (!IsValid(PS))
        {
            UE_LOGFMT(LogStatusUI, Warning, "OnAttributeValueChanged에서 플레이어 스테이트가 유효하지 않습니다.");
            return;
        }
        CachedPlayerState = PS;
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

//@Interpolation Methods
#pragma region Interpolation Methods
void UHUD_StatusUI::StartInterpolation(FStateBarInfo& BarInfo, float StartValue, float TargetValue, const FInterpolationSettings& Settings)
{
    //@기존 보간 중단
    if (BarInfo.InterpolationTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(BarInfo.InterpolationTimer);
    }

    //@보간 데이터 설정
    BarInfo.InterpolationData.StartValue = StartValue;
    BarInfo.InterpolationData.TargetValue = TargetValue;
    BarInfo.InterpolationData.CurrentTime = 0.f;
    BarInfo.InterpolationData.Duration = Settings.Duration;
    BarInfo.InterpolationData.Settings = Settings;
    BarInfo.InterpolationData.bIsActive = true;
    BarInfo.InterpolationData.bIsCompleted = false;

    UE_LOGFMT(LogStatusUI, Log, "보간 시작: {0} -> {1}, 지속시간: {2}", 
        FString::SanitizeFloat(StartValue), 
        FString::SanitizeFloat(TargetValue), 
        FString::SanitizeFloat(Settings.Duration));
}

void UHUD_StatusUI::UpdateInterpolation(FStateBarInfo& BarInfo, float DeltaTime)
{
    if (!BarInfo.InterpolationData.bIsActive || BarInfo.InterpolationData.bIsCompleted)
    {
        return;
    }

    //@시간 업데이트
    BarInfo.InterpolationData.CurrentTime += DeltaTime;
    
    //@진행률 계산 (0.0 ~ 1.0)
    float Progress = FMath::Clamp(BarInfo.InterpolationData.CurrentTime / BarInfo.InterpolationData.Duration, 0.0f, 1.0f);
    
    //@이징 함수 적용
    float EasedProgress = CalculateInterpolatedValue(BarInfo.InterpolationData);
    
    //@현재 값 계산
    float CurrentValue = FMath::Lerp(BarInfo.InterpolationData.StartValue, BarInfo.InterpolationData.TargetValue, EasedProgress);
    
    //@ProgressBar 업데이트
    if (BarInfo.MainBar)
    {
        BarInfo.MainBar->SetPercent(CurrentValue);
    }
    
    //@보간 완료 체크
    if (Progress >= 1.0f)
    {
        CompleteInterpolation(BarInfo);
    }
}

void UHUD_StatusUI::CompleteInterpolation(FStateBarInfo& BarInfo)
{
    BarInfo.InterpolationData.bIsActive = false;
    BarInfo.InterpolationData.bIsCompleted = true;
    
    //@최종 값 설정
    if (BarInfo.MainBar)
    {
        BarInfo.MainBar->SetPercent(BarInfo.InterpolationData.TargetValue);
    }
    
    UE_LOGFMT(LogStatusUI, Log, "보간 완료: {0}", FString::SanitizeFloat(BarInfo.InterpolationData.TargetValue));
}

float UHUD_StatusUI::CalculateInterpolatedValue(const FInterpolationData& Data) const
{
    float Progress = FMath::Clamp(Data.CurrentTime / Data.Duration, 0.0f, 1.0f);
    
    //@커스텀 지수적 이징 함수 사용
    if (Data.Settings.bUseExponentialEasing)
    {
        switch (Data.Settings.EasingFunction)
        {
            case EEasingFunc::EaseIn:
                //@EaseInExpo: 시작이 느린 지수적 보간
                return Progress == 0.0f ? 0.0f : FMath::Pow(2.0f, 10.0f * (Progress - 1.0f));
                
            case EEasingFunc::EaseOut:
                //@EaseOutExpo: 끝이 느린 지수적 보간
                return Progress == 1.0f ? 1.0f : 1.0f - FMath::Pow(2.0f, -10.0f * Progress);
                
            case EEasingFunc::EaseInOut:
                //@EaseInOutExpo: 시작과 끝이 느린 지수적 보간
                if (Progress == 0.0f) return 0.0f;
                if (Progress == 1.0f) return 1.0f;
                if (Progress < 0.5f)
                {
                    return 0.5f * FMath::Pow(2.0f, 20.0f * Progress - 10.0f);
                }
                else
                {
                    return 1.0f - 0.5f * FMath::Pow(2.0f, -20.0f * Progress + 10.0f);
                }
                
            default:
                return Progress;
        }
    }
    
    //@기본 이징 함수 적용
    switch (Data.Settings.EasingFunction)
    {
        case EEasingFunc::Linear:
            return Progress;
            
        case EEasingFunc::EaseIn:
            return FMath::Pow(Progress, Data.Settings.Exponent);
            
        case EEasingFunc::EaseOut:
            return 1.0f - FMath::Pow(1.0f - Progress, Data.Settings.Exponent);
            
        case EEasingFunc::EaseInOut:
            if (Progress < 0.5f)
            {
                return 0.5f * FMath::Pow(2.0f * Progress, Data.Settings.Exponent);
            }
            else
            {
                return 1.0f - 0.5f * FMath::Pow(2.0f * (1.0f - Progress), Data.Settings.Exponent);
            }
            
        default:
            return Progress;
    }
}
#pragma endregion

//@Performance Optimization Methods
#pragma region Performance Optimization Methods
bool UHUD_StatusUI::ShouldUpdateAttribute(const FString& AttributeName, float NewValue) const
{
    //@값이 실제로 변경되었는지 확인
    if (AttributeName == "Health")
    {
        if (FMath::IsNearlyEqual(LastHealthValue, NewValue, 0.01f))
        {
            return false;
        }
    }
    else if (AttributeName == "Stamina")
    {
        if (FMath::IsNearlyEqual(LastStaminaValue, NewValue, 0.01f))
        {
            return false;
        }
    }
    else if (AttributeName == "Mana")
    {
        if (FMath::IsNearlyEqual(LastManaValue, NewValue, 0.01f))
        {
            return false;
        }
    }
    else if (AttributeName == "MaxMana")
    {
        if (FMath::IsNearlyEqual(LastMaxManaValue, NewValue, 0.01f))
        {
            return false;
        }
    }
    
    return true;
}

void UHUD_StatusUI::CachePlayerState()
{
    CachedPlayerState = GetOwningPlayerState<APlayerStateBase>();
    if (CachedPlayerState.IsValid())
    {
        UE_LOGFMT(LogStatusUI, Log, "플레이어 스테이트가 캐시되었습니다.");
    }
}

void UHUD_StatusUI::OptimizeUpdateFrequency()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float MinUpdateInterval = 1.0f / MaxUpdateFrequency;
    
    if (CurrentTime - LastUpdateTime < MinUpdateInterval)
    {
        //@업데이트 빈도 제한에 걸림
        return;
    }
    
    LastUpdateTime = CurrentTime;
}

bool UHUD_StatusUI::ValidateAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
    //@어트리뷰트 유효성 검사
    if (!Attribute.IsValid())
    {
        UE_LOGFMT(LogStatusUI, Warning, "유효하지 않은 어트리뷰트입니다.");
        return false;
    }
    
    //@값 유효성 검사
    if (FMath::IsNaN(NewValue) || FMath::IsNaN(OldValue))
    {
        UE_LOGFMT(LogStatusUI, Warning, "NaN 값이 감지되었습니다. OldValue: {0}, NewValue: {1}", 
            FString::SanitizeFloat(OldValue), FString::SanitizeFloat(NewValue));
        return false;
    }
    
    //@음수 값 검사 (일부 어트리뷰트는 음수가 될 수 있음)
    if (NewValue < 0.0f && (Attribute.AttributeName == "Health" || Attribute.AttributeName == "Stamina"))
    {
        UE_LOGFMT(LogStatusUI, Warning, "음수 값이 감지되었습니다: {0} = {1}", 
            Attribute.AttributeName, FString::SanitizeFloat(NewValue));
        return false;
    }
    
    return true;
}

void UHUD_StatusUI::LogAttributeChange(const FString& AttributeName, float OldValue, float NewValue) const
{
    if (FMath::IsNearlyEqual(OldValue, NewValue, 0.01f))
    {
        return; //@변화가 미미하면 로그하지 않음
    }
    
    float ChangePercent = 0.0f;
    if (OldValue != 0.0f)
    {
        ChangePercent = ((NewValue - OldValue) / OldValue) * 100.0f;
    }
    
    UE_LOGFMT(LogStatusUI, Log, "{0} 변경: {1} -> {2} ({3}%)", 
        AttributeName,
        FString::SanitizeFloat(OldValue),
        FString::SanitizeFloat(NewValue),
        FString::SanitizeFloat(ChangePercent));
}

void UHUD_StatusUI::CleanupInterpolationTimers()
{
    for (auto& StateBarPair : MStateBars)
    {
        FStateBarInfo& BarInfo = StateBarPair.Value;
        if (BarInfo.InterpolationTimer.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(BarInfo.InterpolationTimer);
        }
    }
}
#pragma endregion

//@Blueprint Exposed Methods
#pragma region Blueprint Exposed Methods
void UHUD_StatusUI::SetHealthInterpolationSettings(const FInterpolationSettings& NewSettings)
{
    HealthInterpolationSettings = NewSettings;
    UE_LOGFMT(LogStatusUI, Log, "Health 보간 설정이 변경되었습니다. Duration: {0}, Easing: {1}", 
        FString::SanitizeFloat(NewSettings.Duration), 
        FString::FromInt(static_cast<int32>(NewSettings.EasingFunction)));
}

void UHUD_StatusUI::SetStaminaInterpolationSettings(const FInterpolationSettings& NewSettings)
{
    StaminaInterpolationSettings = NewSettings;
    UE_LOGFMT(LogStatusUI, Log, "Stamina 보간 설정이 변경되었습니다. Duration: {0}, Easing: {1}", 
        FString::SanitizeFloat(NewSettings.Duration), 
        FString::FromInt(static_cast<int32>(NewSettings.EasingFunction)));
}

void UHUD_StatusUI::SetMaxUpdateFrequency(float NewFrequency)
{
    MaxUpdateFrequency = FMath::Clamp(NewFrequency, 1.0f, 120.0f);
    UE_LOGFMT(LogStatusUI, Log, "최대 업데이트 빈도가 {0} FPS로 설정되었습니다.", FString::SanitizeFloat(MaxUpdateFrequency));
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion