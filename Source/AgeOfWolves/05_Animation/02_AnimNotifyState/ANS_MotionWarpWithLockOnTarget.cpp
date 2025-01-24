#include "ANS_MotionWarpWithLockOnTarget.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "04_Component/LockOnComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogANS_MotionWarpWithLockOnTarget)

//@Defualt Setting
#pragma region Default Setting
void UANS_MotionWarpWithLockOnTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    if (!MeshComp)
    {
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - MeshComp가 유효하지 않음");
        return;
    }

    auto Character = Cast<APlayerCharacter>(MeshComp->GetOwner());
    if (!Character)
    {
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - Character 캐스팅 실패. Owner: {0}",
            *GetNameSafe(MeshComp->GetOwner()));
        return;
    }

    auto LockOnComp = Character->GetLockOnComponent();
    if (!LockOnComp || !LockOnComp->GetbLockOn())
    {
        if (auto MotionWarpingComp = Character->GetMotionWarpingComponent())
        {
            MotionWarpingComp->RemoveWarpTarget(FName("LockOnTarget"));
        }
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - LockOn 상태가 아님. WarpTarget 제거");
        return;
    }
}

void UANS_MotionWarpWithLockOnTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (!MeshComp)
    {
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - MeshComp가 유효하지 않음");
        return;
    }

    auto Character = Cast<APlayerCharacter>(MeshComp->GetOwner());
    if (!Character)
    {
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - Character 캐스팅 실패. Owner: {0}",
            *GetNameSafe(MeshComp->GetOwner()));
        return;
    }

    auto LockOnComp = Character->GetLockOnComponent();
    if (!LockOnComp || !LockOnComp->GetbLockOn())
    {
        if (auto MotionWarpingComp = Character->GetMotionWarpingComponent())
        {
            MotionWarpingComp->RemoveWarpTarget(FName("LockOnTarget"));
        }
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - LockOn 상태가 아님. WarpTarget 제거");
        return;
    }

    auto MotionWarpingComp = Character->GetMotionWarpingComponent();
    if (!MotionWarpingComp)
    {
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - MotionWarpingComponent가 없음");
        return;
    }

    auto TargetEnemy = LockOnComp->GetTargetEnemy();
    if (!TargetEnemy)
    {
        UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Warning, "NotifyTick 실패 - 타겟 적이 없음");
        return;
    }

    FMotionWarpingTarget WarpTarget;
    WarpTarget.Name = FName("LockOnTarget");
    WarpTarget.Rotation = LockOnComp->GetFinalRotation();
    WarpTarget.Location = TargetEnemy->GetActorLocation();

    MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

    UE_LOGFMT(LogANS_MotionWarpWithLockOnTarget, Log, "WarpTarget 업데이트 - 타겟: {0} | 위치: {1} | 회전: {2}",
        *TargetEnemy->GetName(),
        *WarpTarget.Location.ToString(),
        *WarpTarget.Rotation.ToString());
}

void UANS_MotionWarpWithLockOnTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc

#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility

#pragma endregion