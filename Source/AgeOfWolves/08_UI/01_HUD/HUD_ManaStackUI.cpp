#include "HUD_ManaStackUI.h"
#include "Logging/StructuredLog.h"    

#include "03_Player/PlayerStateBase.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"
#include "AbilitySystemComponent.h"

#include "NiagaraSystemWidget.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY(LogHUD_ManaStack)

UHUD_ManaStackUI::UHUD_ManaStackUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    CachedASC.Reset();
    CurrentActiveNiagara.Reset();
}

void UHUD_ManaStackUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //외부 바인딩...
    ExternalBindingToAttributeSet();
}

void UHUD_ManaStackUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UHUD_ManaStackUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHUD_ManaStackUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void UHUD_ManaStackUI::ExternalBindingToAttributeSet()
{
    //@Player State
    const auto PS = GetOwningPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogHUD_ManaStack, Error, "플레이어 스테이트가 유효하지 않습니다.");
        return;
    }
    //@ASC
    CachedASC = PS->GetAbilitySystemComponent();
    if (!CachedASC.IsValid())
    {
        UE_LOGFMT(LogHUD_ManaStack, Error, "어빌리티 시스템 컴포넌트가 유효하지 않습니다.");
        return;
    }
    //@AttributeSet
    const UAttributeSet* AttributeSet = CachedASC->GetAttributeSet(UBaseAttributeSet::StaticClass());
    if (!AttributeSet)
    {
        UE_LOGFMT(LogHUD_ManaStack, Warning, "어트리뷰트 셋이 유효하지 않습니다.");
        return;
    }
    //@BaseAttributeSet
    const auto BaseAttributeSet = Cast<UBaseAttributeSet>(AttributeSet);
    if (!BaseAttributeSet)
    {
        return;
    }
    //@FGmaeplayAttribute::Mana Stack
    FGameplayAttribute ManaStackAttribute = BaseAttributeSet->GetManaStackAttribute();
    if (ManaStackAttribute.IsValid())
    {
        ManaStackListener = UAsyncTaskAttributeChanged::ListenToAttributeValueChange(
            CachedASC.Get(),
            ManaStackAttribute);
        if (IsValid(ManaStackListener))
        {
            ManaStackListener->OnAttributeValueChanged.AddDynamic(
                this,
                &UHUD_ManaStackUI::OnAttributeValueChanged);
            UE_LOGFMT(LogHUD_ManaStack, Log, "마나 스택 어트리뷰트 리스너가 생성되었습니다.");
        }
    }
}

void UHUD_ManaStackUI::ActivateNiagaraForStack(int32 StackLevel)
{
    UNiagaraSystemWidget* TargetWidget = nullptr;

    switch (StackLevel)
    {
    case 1:
        TargetWidget = KI_NiagaraWidget;
        UE_LOGFMT(LogHUD_ManaStack, Log, "마나 스택 1단계 '기' 나이아가라 활성화");
        break;
    case 2:
        TargetWidget = KYU_NiagaraWidget;
        UE_LOGFMT(LogHUD_ManaStack, Log, "마나 스택 2단계 '흡' 나이아가라 활성화");
        break;
    case 3:
        TargetWidget = KON_NiagaraWidget;
        UE_LOGFMT(LogHUD_ManaStack, Log, "마나 스택 3단계 '혼' 나이아가라 활성화");
        break;
    default:
        UE_LOGFMT(LogHUD_ManaStack, Warning, "유효하지 않은 스택 레벨: {0}", StackLevel);
        return;
    }

    if (TargetWidget)
    {
        //@이전에 활성화된 나이아가라 비활성화
        if (CurrentActiveNiagara.IsValid() && CurrentActiveNiagara.Get() != TargetWidget)
        {
            CurrentActiveNiagara->DeactivateSystem();
            CurrentActiveNiagara->SetVisibility(ESlateVisibility::Collapsed);
        }

        //@새로운 나이아가라 활성화
        TargetWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        TargetWidget->ActivateSystem(false);

        //@현재 활성화된 위젯 업데이트
        CurrentActiveNiagara = TargetWidget;
    }
}

void UHUD_ManaStackUI::OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
    //@마나 스택 변화 감지
    const int32 NewStackLevel = FMath::RoundToInt(NewValue);

    UE_LOGFMT(LogHUD_ManaStack, Log, "마나 스택 변화: {0} -> {1}", OldValue, NewValue);

    //@변화 없으면 종료
    if (OldValue == NewValue) return;

    //@스택 레벨에 따른 나이아가라 활성화
    if (NewStackLevel >= 1 && NewStackLevel <= 3)
    {
        ActivateNiagaraForStack(NewStackLevel);
    }
}
