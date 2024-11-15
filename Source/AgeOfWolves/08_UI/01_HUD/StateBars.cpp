#include "StateBars.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"

#include "03_Player/PlayerStateBase.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"
#include "AbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogStateBars)

UStateBars::UStateBars(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    MStateBars.Empty();
    StateBarUIVerticalBox = nullptr;
    HP = nullptr;
    SP = nullptr;
    CachedASC = nullptr;
}

void UStateBars::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    ExternalBindingToAttributeSet();
}

void UStateBars::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UStateBars::NativeConstruct()
{
    Super::NativeConstruct();
}

void UStateBars::NativeDestruct()
{
    // AsyncTask 정리
    for (auto* Listener : AttributeListeners)
    {
        if (Listener)
        {
            Listener->EndTask();
        }
    }
    AttributeListeners.Empty();

    Super::NativeDestruct();
}

void UStateBars::ExternalBindingToAttributeSet()
{
    const auto PS = GetOwningPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogStateBars, Error, "Player State가 유효하지 않습니다.");
        return;
    }

    CachedASC = PS->GetAbilitySystemComponent();
    if (!CachedASC.IsValid())
    {
        UE_LOGFMT(LogStateBars, Error, "ASC가 유효하지 않습니다.");
        return;
    }

    const UAttributeSet* AttributeSet = CachedASC->GetAttributeSet(UBaseAttributeSet::StaticClass());
    if (!AttributeSet)
    {
        UE_LOGFMT(LogStateBars, Warning, "AttributeSet is invalid");
        return;
    }

    const auto BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
    if (!BaseAttributeSet)
    {
        return;
    }

    // Health 속성 리스너 생성
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
                &UStateBars::OnAttributeValueChanged);
            AttributeListeners.Add(HealthListener);
            UE_LOGFMT(LogStateBars, Log, "Health attribute listener created");
        }
    }

    // Stamina 속성 리스너 생성
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
                &UStateBars::OnAttributeValueChanged);
            AttributeListeners.Add(StaminaListener);
            UE_LOGFMT(LogStateBars, Log, "Stamina attribute listener created");
        }
    }

}

void UStateBars::InitializeStateBars()
{
    MStateBars.Add({ "Health", HP });
    MStateBars.Add({ "Stamina", SP });

    StateBarsInitFinished.ExecuteIfBound();
}

void UStateBars::OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
    const auto PS = GetOwningPlayerState<APlayerStateBase>();
    if (!IsValid(PS))
    {
        UE_LOGFMT(LogStateBars, Warning, "PlayerState is invalid in OnAttributeValueChanged");
        return;
    }

    if (!Attribute.IsValid())
    {
        UE_LOGFMT(LogStateBars, Warning, "Invalid attribute in OnAttributeValueChanged");
        return;
    }

    auto* StateBar = MStateBars.Find(Attribute.AttributeName);
    if (!StateBar)
    {
        UE_LOGFMT(LogStateBars, Warning, "StateBar not found for attribute: {0}", Attribute.AttributeName);
        return;
    }

    FString AttributeLookingFor = "Max" + Attribute.AttributeName;
    float MaxValue = PS->GetAttributeValue<float>(AttributeLookingFor);
    if (MaxValue == 0)
    {
        UE_LOGFMT(LogStateBars, Warning, "MaxValue is zero for attribute: {0}", AttributeLookingFor);
        return;
    }

    (*StateBar)->SetPercent(NewValue / MaxValue);
    UE_LOGFMT(LogStateBars, Error, "{0} : {1}", Attribute.AttributeName, FString::SanitizeFloat(NewValue));
}