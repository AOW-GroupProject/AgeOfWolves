#include "AsyncTask_MonitorAreaBinding.h"
#include "Logging/StructuredLog.h"

#include "04_Component/ObjectiveDetectionComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "16_Level/Area.h"

DEFINE_LOG_CATEGORY_STATIC(LogAreaBindingMonitor, Log, All);

UAsyncTask_MonitorAreaBinding* UAsyncTask_MonitorAreaBinding::ListenForAreaBindingChanges(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController))
    {
        return nullptr;
    }

    //@OD Comp
    UObjectiveDetectionComponent* ODComp = PlayerController->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ODComp)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "OD 컴포넌트를 찾을 수 없음");
        return nullptr;
    }

    //@PlayerState
    APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
    if (!PlayerState)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "유효한 PlayerState를 찾을 수 없음");
        return nullptr;
    }

    // PlayerStateBase로 캐스팅
    APlayerStateBase* PlayerStateBase = Cast<APlayerStateBase>(PlayerState);
    if (!PlayerStateBase)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "PlayerState를 PlayerStateBase로 캐스팅할 수 없음");
        return nullptr;
    }

    //@ASC
    UAbilitySystemComponent* AbilitySystemComp = PlayerStateBase->GetAbilitySystemComponent();
    if (!AbilitySystemComp)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "PlayerState에서 ASC를 찾을 수 없음");
        return nullptr;
    }

    //@AsyncTask 인스턴스 생성
    UAsyncTask_MonitorAreaBinding* AsyncTask = NewObject<UAsyncTask_MonitorAreaBinding>();
    AsyncTask->ODComponent = ODComp;
    AsyncTask->ASC = AbilitySystemComp;

    //@이벤트 리스닝 등록
    ODComp->PlyaerBoundToArea.AddUObject(AsyncTask, &UAsyncTask_MonitorAreaBinding::OnPlayerBoundToArea);

    //@현재 상태에 따라 초기 효과 적용
    AsyncTask->ApplyAlertStateEffect(ODComp->GetBoundAreas().Num() > 0);

    UE_LOGFMT(LogAreaBindingMonitor, Log, "Area 바인딩 이벤트 리스닝 AsyncTask 생성됨");
    return AsyncTask;
}

void UAsyncTask_MonitorAreaBinding::EndTask()
{
    //@이벤트 리스닝 해제
    if (ODComponent.IsValid())
    {
        ODComponent->PlyaerBoundToArea.RemoveAll(this);
    }

    //@GE 제거
    if (ASC.IsValid() && AlertStateEffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(AlertStateEffectHandle);
    }

    UE_LOGFMT(LogAreaBindingMonitor, Log, "Area 바인딩 AsyncTask 종료됨");

    SetReadyToDestroy();
    MarkAsGarbage();
}

void UAsyncTask_MonitorAreaBinding::OnPlayerBoundToArea_Implementation(FAreaBindingInfo AreaInfo, bool bIsBound)
{
    UE_LOGFMT(LogAreaBindingMonitor, Log, "Area 바인딩 변경: {0} - {1}",
        *AreaInfo.AreaID.ToString(), bIsBound ? TEXT("진입") : TEXT("이탈"));

    // GE 적용
    ApplyAlertStateEffect(bIsBound);

    // 델리게이트 호출
    OnAreaBindingChanged.Broadcast(bIsBound);

    //@오버라이딩 구현...
}

void UAsyncTask_MonitorAreaBinding::ApplyAlertStateEffect(bool bIsInDanger)
{
    if (!ASC.IsValid())
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "유효하지 않은 ASC");
        return;
    }

    //@기존 효과 제거
    if (AlertStateEffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(AlertStateEffectHandle);
        AlertStateEffectHandle.Invalidate();
    }

    //@경고 상태 GE 생성 및 적용
    UGameplayEffect* AlertStateGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AlertState")));
    AlertStateGE->DurationPolicy = EGameplayEffectDurationType::Infinite;

    // AlertState 속성 설정 (1.0f)
    FGameplayModifierInfo ModifierInfo;
    ModifierInfo.Attribute = UBaseAttributeSet::GetAlertLevelAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Override;
    ModifierInfo.ModifierMagnitude = FScalableFloat(bIsInDanger? 1.f: 0.f);
    AlertStateGE->Modifiers.Add(ModifierInfo);

    // GE 적용
    AlertStateEffectHandle = ASC->ApplyGameplayEffectToSelf(AlertStateGE, 1.0f, ASC->MakeEffectContext());
    UE_LOGFMT(LogAreaBindingMonitor, Log, "AlertState 효과 적용됨: {0}", bIsInDanger);
}