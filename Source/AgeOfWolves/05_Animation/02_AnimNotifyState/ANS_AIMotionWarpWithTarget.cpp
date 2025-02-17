#include "ANS_AIMotionWarpWithTarget.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "10_AI/BaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogANS_AIMotionWarpWithTarget)

void UANS_AIMotionWarpWithTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (!MeshComp)
    {
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyBegin 실패 - MeshComp가 유효하지 않음");
        return;
    }

    //@캐릭터 캐스팅
    auto Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!Character)
    {
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyBegin 실패 - Character 캐스팅 실패. Owner: {0}",
            *GetNameSafe(MeshComp->GetOwner()));
        return;
    }

    //@AI 컨트롤러 캐스팅
    auto AIController = Cast<ABaseAIController>(Character->GetController());
    if (!AIController || !AIController->GetBlackboardComponent())
    {
        if (auto MotionWarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>())
        {
            MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
        }
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyBegin 실패 - AI 컨트롤러 또는 블랙보드가 유효하지 않음");
        return;
    }
}

void UANS_AIMotionWarpWithTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (!MeshComp)
    {
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyTick 실패 - MeshComp가 유효하지 않음");
        return;
    }

    //@캐릭터 캐스팅
    auto Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!Character)
    {
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyTick 실패 - Character 캐스팅 실패. Owner: {0}",
            *GetNameSafe(MeshComp->GetOwner()));
        return;
    }

    //@AI 컨트롤러 캐스팅
    auto AIController = Cast<ABaseAIController>(Character->GetController());
    if (!AIController || !AIController->GetBlackboardComponent())
    {
        if (auto MotionWarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>())
        {
            MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
        }
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyTick 실패 - AI 컨트롤러 또는 블랙보드가 유효하지 않음");
        return;
    }

    //@Motion Warping 컴포넌트 체크
    auto MotionWarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>();
    if (!MotionWarpingComp)
    {
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyTick 실패 - MotionWarpingComponent가 없음");
        return;
    }

    //@블랙보드에서 타겟 액터 가져오기
    auto TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
    if (!TargetActor)
    {
        MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
        UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Warning, "NotifyTick 실패 - 타겟 액터가 없음");
        return;
    }

    //@Warp Target 설정
    FMotionWarpingTarget WarpTarget;
    WarpTarget.Name = WarpTargetName;
    WarpTarget.Location = TargetActor->GetActorLocation();
    WarpTarget.Rotation = AIController->GetControlRotation();

    //@Warp Target 업데이트
    MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

    UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Log, "WarpTarget 업데이트 - 타겟: {0} | 위치: {1} | 회전: {2}",
        *TargetActor->GetName(),
        *WarpTarget.Location.ToString(),
        *WarpTarget.Rotation.ToString());
}

void UANS_AIMotionWarpWithTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    if (!MeshComp)
    {
        return;
    }

    //@캐릭터 캐스팅
    if (auto Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
    {
        //@Warp Target 제거
        if (auto MotionWarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>())
        {
            MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
            UE_LOGFMT(LogANS_AIMotionWarpWithTarget, Log, "WarpTarget 제거 완료");
        }
    }
}