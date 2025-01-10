#include "ANS_MotionWarpWithLockOnTarget.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "04_Component/LockOnComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogANS_MotionWarpWithLockOnTarget)

//@Defualt Setting
#pragma region Default Setting
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UANS_MotionWarpWithLockOnTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UANS_MotionWarpWithLockOnTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (auto Character = Cast<APlayerCharacter>(MeshComp->GetOwner()))
    {
        if (auto LockOnComp = Character->GetLockOnComponent())
        {
            if (auto MotionWarpingComp = Character->GetMotionWarpingComponent())
            {
                FMotionWarpingTarget WarpTarget;
                WarpTarget.Name = FName("LockOnTarget");
                WarpTarget.Rotation = LockOnComp->GetFinalRotation();
                WarpTarget.Location = LockOnComp->GetTargetEnemy()->GetActorLocation();

                MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);
            }
        }
    }
}

void UANS_MotionWarpWithLockOnTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility

#pragma endregion
