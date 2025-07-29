#include "LoadingUI.h"
#include "Logging/StructuredLog.h"    

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogLoading)

ULoadingUI::ULoadingUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void ULoadingUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

}

void ULoadingUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void ULoadingUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 기존 가시성 변경 델리게이트 바인딩
    if (IsValid(this))
    {
        OnVisibilityChanged.AddDynamic(this, &ULoadingUI::HandleVisibilityChanged);
        PreviousVisibility = GetVisibility();

        // 애니메이션 완료 델리게이트 바인딩 추가
        // 이 바인딩을 통해 블루프린트에서 애니메이션이 완료되었다고 알릴 때
        // C++의 OnBlendOutAnimationCompleted 함수가 자동으로 호출됩니다
        OnBlendAnimationCompleted.AddDynamic(this, &ULoadingUI::OnBlendOutAnimationCompleted);

        UE_LOGFMT(LogLoading, Log, "LoadingUI 델리게이트 바인딩 완료 - 초기 상태: {0}",
            static_cast<int32>(PreviousVisibility));
    }
}

void ULoadingUI::NativeDestruct()
{

    Super::NativeDestruct();
}

void ULoadingUI::SetLoadingUIVisibility(ESlateVisibility NewVisibility)
{
    ESlateVisibility CurrentVisibility = GetVisibility();

    if (CurrentVisibility != NewVisibility)
    {
        UE_LOGFMT(LogLoading, Log, "LoadingUI 가시성 변경 요청: {0} -> {1}",
            static_cast<int32>(CurrentVisibility), static_cast<int32>(NewVisibility));

        // Collapsed로 변경하는 경우: 애니메이션을 먼저 재생하고 나중에 실제 변경
        if (NewVisibility == ESlateVisibility::Collapsed)
        {
            // 이미 애니메이션 대기 중이라면 중복 처리를 방지합니다
            if (bIsWaitingForBlendOutAnimation)
            {
                UE_LOGFMT(LogLoading, Warning, "이미 Blend Out 애니메이션 대기 중 - 요청 무시");
                return;
            }

            // 현재 보이는 상태에서만 애니메이션을 재생합니다
            // 이미 숨겨진 상태라면 애니메이션 없이 바로 처리합니다
            if (CurrentVisibility == ESlateVisibility::SelfHitTestInvisible ||
                CurrentVisibility == ESlateVisibility::Visible)
            {
                UE_LOGFMT(LogLoading, Log, "Blend Out 애니메이션 재생 준비 - 완료 후 Collapsed로 변경 예정");

                // 애니메이션 대기 상태로 설정
                bIsWaitingForBlendOutAnimation = true;
                PendingVisibilityAfterAnimation = NewVisibility;

                OnLoadingUIHidden();

                UE_LOGFMT(LogLoading, Log, "OnLoadingUIHidden 이벤트 발생 - 블루프린트에서 애니메이션 재생 대기");
            }
            else
            {
                // 이미 숨겨진 상태라면 애니메이션 없이 바로 Collapsed로 설정
                UE_LOGFMT(LogLoading, Log, "이미 숨겨진 상태 - 애니메이션 없이 바로 Collapsed 적용");
                SetVisibility(NewVisibility);
            }
        }
        else
        {
            // Collapsed가 아닌 다른 상태로 변경하는 경우: 기존 방식대로 즉시 변경
            // 이 경우에는 애니메이션 대기 상태를 해제하고 즉시 처리합니다
            if (bIsWaitingForBlendOutAnimation)
            {
                UE_LOGFMT(LogLoading, Log, "애니메이션 대기 중이었지만 다른 가시성 요청으로 인해 즉시 처리");
                bIsWaitingForBlendOutAnimation = false;
            }

            SetVisibility(NewVisibility);
            UE_LOGFMT(LogLoading, Log, "LoadingUI 가시성 즉시 변경 완료");
        }
    }
    else
    {
        UE_LOGFMT(LogLoading, Verbose, "LoadingUI 가시성 변경 요청 무시됨 - 이미 동일한 상태: {0}",
            static_cast<int32>(CurrentVisibility));
    }
}

void ULoadingUI::HandleVisibilityChanged(ESlateVisibility InVisibility)
{
    // 가시성이 실제로 변경되었는지 확인합니다
    // 델리게이트가 여러 번 호출될 수 있으므로 중복 처리를 방지합니다
    if (PreviousVisibility != InVisibility)
    {
        UE_LOGFMT(LogLoading, Log, "LoadingUI 가시성 변경 감지: {0} -> {1}",
            static_cast<int32>(PreviousVisibility), static_cast<int32>(InVisibility));

        // 새로운 가시성 상태에 따라 적절한 블루프린트 이벤트를 호출합니다
        if (InVisibility == ESlateVisibility::SelfHitTestInvisible)
        {
            UE_LOGFMT(LogLoading, Log, "LoadingUI 표시됨 - OnLoadingUIShown 이벤트 발생");
            OnLoadingUIShown();
        }
        else if (InVisibility == ESlateVisibility::Collapsed)
        {
            UE_LOGFMT(LogLoading, Log, "LoadingUI 숨겨짐 - OnLoadingUIHidden 이벤트 발생");
            OnLoadingUIHidden();
        }

        // 현재 상태를 다음 비교를 위해 저장합니다
        PreviousVisibility = InVisibility;
    }
}

void ULoadingUI::OnBlendOutAnimationCompleted()
{
    // 애니메이션 대기 상태가 아니라면 무시합니다
    // 이는 예상치 못한 호출을 방지하는 안전장치입니다
    if (!bIsWaitingForBlendOutAnimation)
    {
        UE_LOGFMT(LogLoading, Warning, "애니메이션 완료 콜백이 호출되었지만 대기 상태가 아님 - 무시");
        return;
    }

    UE_LOGFMT(LogLoading, Log, "Blend Out 애니메이션 완료 - 실제 가시성 변경 수행: {0}",
        static_cast<int32>(PendingVisibilityAfterAnimation));

    // 실제 가시성 변경을 수행합니다
    // 이 시점에서 SetVisibility를 호출하면 HandleVisibilityChanged가 호출되지만,
    // 이미 애니메이션이 완료된 상태이므로 추가적인 애니메이션은 발생하지 않습니다
    SetVisibility(PendingVisibilityAfterAnimation);

    // 애니메이션 대기 상태를 해제합니다
    bIsWaitingForBlendOutAnimation = false;
    PendingVisibilityAfterAnimation = ESlateVisibility::Collapsed;

    UE_LOGFMT(LogLoading, Log, "애니메이션 완료 후 가시성 변경 및 상태 정리 완료");
}

ESlateVisibility ULoadingUI::GetLoadingUIVisibility() const
{
    // 현재 위젯의 가시성 상태를 반환합니다
    // 이 함수는 const로 선언되어 객체의 상태를 변경하지 않음을 보장합니다
    return GetVisibility();
}

bool ULoadingUI::IsLoadingUIVisible() const
{
    // 위젯이 사용자에게 보이는 상태인지 확인합니다
    // SelfHitTestInvisible과 Visible 상태를 모두 "보이는" 상태로 간주합니다
    ESlateVisibility CurrentVisibility = GetVisibility();
    return (CurrentVisibility == ESlateVisibility::SelfHitTestInvisible ||
        CurrentVisibility == ESlateVisibility::Visible);
}

bool ULoadingUI::IsLoadingUIHidden() const
{
    // 위젯이 숨겨진 상태인지 확인합니다
    // Collapsed와 Hidden 상태를 모두 "숨겨진" 상태로 간주합니다
    ESlateVisibility CurrentVisibility = GetVisibility();
    return (CurrentVisibility == ESlateVisibility::Collapsed ||
        CurrentVisibility == ESlateVisibility::Hidden);
}