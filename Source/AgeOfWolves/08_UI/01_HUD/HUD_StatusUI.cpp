// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_StatusUI.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "03_Player/PlayerStateBase.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"
#include "AbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogStatusUI)


UHUD_StatusUI::UHUD_StatusUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    StateBarVerticalBox = nullptr;

    MStateBars.Empty();
    HP = nullptr;
    SP = nullptr;

    ManaCountBox = nullptr;

    CachedASC = nullptr;
}

void UHUD_StatusUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

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
    //@ProgressBar 이미지 확인 로그
    if (!HP->WidgetStyle.FillImage.GetResourceObject())
    {
        UE_LOGFMT(LogStatusUI, Warning, "HP바의 전경 이미지가 설정되지 않았습니다.");
    }
    if (!SP->WidgetStyle.FillImage.GetResourceObject())
    {
        UE_LOGFMT(LogStatusUI, Warning, "SP바의 전경 이미지가 설정되지 않았습니다.");
    }

    //@MStateBars
    MStateBars.Add({ "Health", HP });
    MStateBars.Add({ "Stamina", SP });

    //@Memory 아이템 생성
    CreateAndSetupMemoryImage();
    //@마나 카운트 이미지 생성
    CreateManaCountImages();

    //@초기화 완료 이벤트
    StatusUIInitFinished.ExecuteIfBound();
}

void UHUD_StatusUI::CreateAndSetupMemoryImage()
{

    //@현재 캐릭터에 장착한 Memory 아이템 가져와서 업데이트
    
}

void UHUD_StatusUI::CreateManaCountImages()
{
    //@TODO: Max Mana 가져와서 Image 추가


}

void UHUD_StatusUI::OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
    const auto PS = GetOwningPlayerState<APlayerStateBase>();
    if (!IsValid(PS))
    {
        UE_LOGFMT(LogStatusUI, Warning, "OnAttributeValueChanged에서 플레이어 스테이트가 유효하지 않습니다.");
        return;
    }

    if (!Attribute.IsValid())
    {
        UE_LOGFMT(LogStatusUI, Warning, "OnAttributeValueChanged에서 어트리뷰트가 유효하지 않습니다.");
        return;
    }

    //@State Bar
    if (Attribute.AttributeName == "Health" || Attribute.AttributeName == "Stamina")
    {
        auto* StateBar = MStateBars.Find(Attribute.AttributeName);
        if (!StateBar)
        {
            UE_LOGFMT(LogStatusUI, Warning, "다음 어트리뷰트에 대한 상태바를 찾을 수 없습니다: {0}", Attribute.AttributeName);
            return;
        }

        FString AttributeLookingFor = "Max" + Attribute.AttributeName;
        float MaxValue = PS->GetAttributeValue<float>(AttributeLookingFor);
        if (MaxValue == 0)
        {
            UE_LOGFMT(LogStatusUI, Warning, "다음 어트리뷰트의 최대값이 0입니다: {0}", AttributeLookingFor);
            return;
        }

        (*StateBar)->SetPercent(NewValue / MaxValue);
        UE_LOGFMT(LogStatusUI, Error, "{0} 현재 값: {1}", Attribute.AttributeName, FString::SanitizeFloat(NewValue));

        return;
    }

    //@Count Image
    if (Attribute.AttributeName == "MaxMana" || Attribute.AttributeName == "Mana")
    {
        //@TODO: Mana Count Image 업데이트
        UE_LOGFMT(LogStatusUI, Error, "{0} 현재 값: {1}", Attribute.AttributeName, FString::SanitizeFloat(NewValue));
    }

}
