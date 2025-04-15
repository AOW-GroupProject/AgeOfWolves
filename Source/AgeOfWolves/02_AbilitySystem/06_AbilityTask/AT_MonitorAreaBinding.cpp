#include "AT_MonitorAreaBinding.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemComponent.h"
#include "03_Player/BasePlayerController.h"
#include "04_Component/ObjectiveDetectionComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogAreaBindingMonitor)

//@Defualt Setting
#pragma region Default Setting
UAT_MonitorAreaBinding::UAT_MonitorAreaBinding(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

UAT_MonitorAreaBinding* UAT_MonitorAreaBinding::MonitorAreaBinding(UGameplayAbility* OwningAbility)
{
    UAT_MonitorAreaBinding* Task = NewAbilityTask<UAT_MonitorAreaBinding>(OwningAbility);
    UE_LOGFMT(LogAreaBindingMonitor, Log, "Area 바인딩 모니터링 태스크 생성됨");
    return Task;
}


void UAT_MonitorAreaBinding::Activate()
{
    Super::Activate();

    // 아바타 유효성 확인
    AActor* Avatar = GetAvatarActor();
    if (!Avatar)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "유효하지 않은 아바타 액터");
        EndTask();
        return;
    }

    // 컨트롤러 가져오기
    APawn* AvatarPawn = Cast<APawn>(Avatar);
    if (!AvatarPawn)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "유효하지 않은 Pawn: {0}", *Avatar->GetName());
        EndTask();
        return;
    }

    // BasePlayerController 가져오기
    ABasePlayerController* PC = Cast<ABasePlayerController>(AvatarPawn->GetController());
    if (!PC)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "BasePlayerController로 캐스팅 실패: {0}",
            AvatarPawn->GetController() ? *AvatarPawn->GetController()->GetName() : TEXT("컨트롤러 없음"));
        EndTask();
        return;
    }

    // OD 컴포넌트 가져오기
    UObjectiveDetectionComponent* ODComp = PC->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ODComp)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "ObjectiveDetectionComponent를 찾을 수 없음: {0}", *PC->GetName());
        EndTask();
        return;
    }

    // Area 바인딩 이벤트에 콜백 등록
    ODComp->PlyaerBoundToArea.AddUObject(this, &UAT_MonitorAreaBinding::OnPlayerBoundToArea);
    UE_LOGFMT(LogAreaBindingMonitor, Log, "PlyaerBoundToArea 이벤트 콜백 등록 완료");

    // 현재 바인딩된 Area 수 초기화
    int32 BoundAreaCount = ODComp->GetBoundAreas().Num();
    UE_LOGFMT(LogAreaBindingMonitor, Log, "초기 바인딩된 Area 수: {0}", BoundAreaCount);

    // 이미 Area에 있는 경우 효과 적용
    ApplyAlertStateEffect(BoundAreaCount > 0);
}

void UAT_MonitorAreaBinding::OnDestroy(bool bInOwnerFinished)
{
    UE_LOGFMT(LogAreaBindingMonitor, Log, "Area 바인딩 모니터링 태스크 종료 (소유자 완료: {0})", bInOwnerFinished);

    // 아바타 유효성 확인
    AActor* Avatar = GetAvatarActor();
    if (Avatar)
    {
        // 컨트롤러 가져오기
        APawn* AvatarPawn = Cast<APawn>(Avatar);
        if (AvatarPawn)
        {
            // BasePlayerController 가져오기
            ABasePlayerController* PC = Cast<ABasePlayerController>(AvatarPawn->GetController());
            if (PC)
            {
                // OD 컴포넌트 가져오기
                UObjectiveDetectionComponent* ODComp = PC->FindComponentByClass<UObjectiveDetectionComponent>();
                if (ODComp)
                {
                    // 콜백 등록 해제
                    ODComp->PlyaerBoundToArea.RemoveAll(this);
                    UE_LOGFMT(LogAreaBindingMonitor, Log, "PlyaerBoundToArea 이벤트 콜백 등록 해제");
                }
            }
        }
    }

    //@경고 상태 효과 제거
    ApplyAlertStateEffect(false);

    Super::OnDestroy(bInOwnerFinished);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAT_MonitorAreaBinding::ApplyAlertStateEffect(bool bIsInDanger)
{
    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (!ASC)
    {
        UE_LOGFMT(LogAreaBindingMonitor, Error, "유효하지 않은 AbilitySystemComponent");
        return;
    }

    // 기존 효과 제거
    if (AlertStateEffectHandle.IsValid())
    {
        ASC->RemoveActiveGameplayEffect(AlertStateEffectHandle);
        AlertStateEffectHandle.Invalidate();
        UE_LOGFMT(LogAreaBindingMonitor, Log, "기존 AlertState 효과 제거됨");
    }

    // 새 효과 적용
    if (bIsInDanger)
    {
        // 경고 상태 GE 생성 및 적용
        UGameplayEffect* AlertStateGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AlertState")));
        AlertStateGE->DurationPolicy = EGameplayEffectDurationType::Infinite;

        // AlertState 속성 설정 (1.0f)
        FGameplayModifierInfo ModifierInfo;
        ModifierInfo.Attribute = UBaseAttributeSet::GetAlertLevelAttribute();
        ModifierInfo.ModifierOp = EGameplayModOp::Override;
        ModifierInfo.ModifierMagnitude = FScalableFloat(bIsInDanger ? 1.f : 0.f);
        AlertStateGE->Modifiers.Add(ModifierInfo);

        // GE 적용
        AlertStateEffectHandle = ASC->ApplyGameplayEffectToSelf(AlertStateGE, 1.0f, ASC->MakeEffectContext());
        UE_LOGFMT(LogAreaBindingMonitor, Log, "AlertState 효과 적용됨 (위험 상태: {0})", bIsInDanger);
    }
    else
    {
        UE_LOGFMT(LogAreaBindingMonitor, Log, "AlertState 효과 제거됨 (안전 상태)");
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UAT_MonitorAreaBinding::OnPlayerBoundToArea_Implementation(FAreaBindingInfo AreaInfo, bool bIsBound)
{
    UE_LOGFMT(LogAreaBindingMonitor, Log, "Area 바인딩 상태 변경: {0}",
        bIsBound ? TEXT("진입") : TEXT("이탈"));

    //@GE 적용 - Alert 1 혹은 0으로 설정
    ApplyAlertStateEffect(bIsBound);

    //@위험 지역 입장 이벤트
    EnterDangerZone.Broadcast(bIsBound);

    //@블루프린트에서 추가 구현...

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion