#include "HUD_ManaStackUI.h"
#include "Logging/StructuredLog.h"    

#include "03_Player/PlayerStateBase.h"
#include "07_BlueprintNode/AsyncTaskAttributeChanged.h"
#include "AbilitySystemComponent.h"

#include "NiagaraSystemWidget.h"
#include "NiagaraComponent.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Engine/Texture2D.h"

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
        //@모든 나이아가라를 비활성화하여 동시 활성화 방지
        TArray<UNiagaraSystemWidget*> AllWidgets = { KI_NiagaraWidget, KYU_NiagaraWidget, KON_NiagaraWidget };
        for (UNiagaraSystemWidget* Widget : AllWidgets)
        {
            if (Widget && Widget != TargetWidget)
            {
                Widget->SetVisibility(ESlateVisibility::Collapsed);
                Widget->DeactivateSystem();
            }
        }

        //@모든 오버레이 숨김 및 초기화
        TArray<UOverlay*> AllOverlays = { KI_Overlay, KYU_Overlay, KON_Overlay };
        for (UOverlay* Overlay : AllOverlays)
        {
            if (Overlay)
            {
                Overlay->SetVisibility(ESlateVisibility::Collapsed);
                Overlay->SetRenderOpacity(1.f);
            }
        }

        //@타겟 위젯 시퀀스 실행: 1.5s Niagara -> 1.5s Image -> 0.5s FadeOut
        TargetWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        TargetWidget->ActivateSystem(true); // bReset = true로 최초 상태부터 재생

        UOverlay* PairedOverlay = nullptr;
        UTexture2D* PairedTexture = nullptr;
        if (TargetWidget == KI_NiagaraWidget)      
        {
            PairedOverlay = KI_Overlay;
            PairedTexture = KI_Texture;
        }
        else if (TargetWidget == KYU_NiagaraWidget) 
        {
            PairedOverlay = KYU_Overlay;
            PairedTexture = KYU_Texture;
        }
        else if (TargetWidget == KON_NiagaraWidget) 
        {
            PairedOverlay = KON_Overlay;
            PairedTexture = KON_Texture;
        }

        //@텍스처가 없어도 시퀀스는 실행 (Niagara만 표시)
        PlayStackSequence(TargetWidget, PairedOverlay, PairedTexture);

        //@현재 활성화된 위젯 업데이트
        CurrentActiveNiagara = TargetWidget;
    }
}

void UHUD_ManaStackUI::PlayStackSequence(UNiagaraSystemWidget* NiagaraWidget, UOverlay* OverlayWidget, UTexture2D* Texture)
{
    if (!NiagaraWidget)
    {
        return;
    }

    //@텍스처가 없으면 Niagara만 표시하고 종료
    if (!Texture)
    {
        UE_LOGFMT(LogHUD_ManaStack, Warning, "텍스처가 설정되지 않았습니다. Niagara만 표시합니다.");
        NiagaraWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        return;
    }

    //@0.0s ~ 1.5s: Niagara만 표시
    NiagaraWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    if (OverlayWidget)
    {
        OverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
        OverlayWidget->SetRenderOpacity(1.f);
    }

    //@1.5s 후: Overlay 표시 (텍스처를 Image로 설정)
    FTimerDelegate ShowOverlayDelegate;
    ShowOverlayDelegate.BindLambda([this, OverlayWidget, Texture]()
    {
        if (OverlayWidget && Texture)
        {
            // Overlay 내부의 Image 위젯에 텍스처 설정
            if (UImage* ImageWidget = Cast<UImage>(OverlayWidget->GetChildAt(0)))
            {
                ImageWidget->SetBrushFromTexture(Texture);
            }
            OverlayWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
    });

    FTimerHandle ShowOverlayHandle;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(ShowOverlayHandle, ShowOverlayDelegate, 1.5f, false);
    }

    //@3.0s 후: 0.5s 동안 Overlay 페이드아웃
    FTimerDelegate FadeOutDelegate;
    FadeOutDelegate.BindLambda([this, OverlayWidget]()
    {
        if (!OverlayWidget) return;
        StartBlendOut(OverlayWidget, 0.5f);
    });

    FTimerHandle FadeStartHandle;
    if (UWorld* World2 = GetWorld())
    {
        World2->GetTimerManager().SetTimer(FadeStartHandle, FadeOutDelegate, 3.0f, false);
    }
}

void UHUD_ManaStackUI::StartBlendOut(UOverlay* OverlayWidget, float DurationSeconds)
{
    if (!OverlayWidget || DurationSeconds <= 0.f)
    {
        return;
    }

    BlendOutElapsedSeconds.FindOrAdd(OverlayWidget) = 0.f;

    FTimerHandle* HandlePtr = nullptr;
    if (OverlayWidget == KI_Overlay) HandlePtr = &KI_BlendTimerHandle;
    else if (OverlayWidget == KYU_Overlay) HandlePtr = &KYU_BlendTimerHandle;
    else if (OverlayWidget == KON_Overlay) HandlePtr = &KON_BlendTimerHandle;

    if (!HandlePtr)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        FTimerDelegate TickDelegate;
        TickDelegate.BindLambda([this, OverlayWidget, DurationSeconds, HandlePtr]()
        {
            TickBlendOut(OverlayWidget, DurationSeconds, *HandlePtr);
        });
        World->GetTimerManager().SetTimer(*HandlePtr, TickDelegate, 0.016f, true);
    }
}

void UHUD_ManaStackUI::TickBlendOut(UOverlay* OverlayWidget, float DurationSeconds, FTimerHandle& TimerHandle)
{
    if (!OverlayWidget)
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(TimerHandle);
        }
        return;
    }

    float& Elapsed = BlendOutElapsedSeconds.FindOrAdd(OverlayWidget);
    Elapsed += 0.016f;

    float Alpha = FMath::Clamp(Elapsed / DurationSeconds, 0.f, 1.f);
    float Opacity = FMath::Lerp(1.f, -1.f, Alpha); // 1 -> -1
    OverlayWidget->SetRenderOpacity(Opacity);

    if (Alpha >= 1.f)
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(TimerHandle);
        }
        OverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
        BlendOutElapsedSeconds.Remove(OverlayWidget);
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