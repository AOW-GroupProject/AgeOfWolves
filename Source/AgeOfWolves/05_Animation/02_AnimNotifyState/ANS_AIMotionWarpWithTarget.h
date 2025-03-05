#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"

#include "ANS_AIMotionWarpWithTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_AIMotionWarpWithTarget, Log, All)

/**
 * @class UANS_AIMotionWarpWithTarget
 * AI가 블랙보드의 타겟 액터에 대해 Motion Warp를 수행하는 ANS입니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_AIMotionWarpWithTarget : public UAnimNotifyState_MotionWarping
{
    GENERATED_BODY()

        //@Default Setting
#pragma region Default Setting
protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

    //@Property
#pragma region Property
protected:
    //@Warp Target의 이름
    UPROPERTY(EditAnywhere, Category = "AI | Motion Warp")
        FName WarpTargetName = FName("AITarget");

    UPROPERTY(EditAnywhere, Category = "AI | Motion Warp")
        float OffsetDistance = 100.0f;

    //@Z축 높이 보존 여부
    UPROPERTY(EditAnywhere, Category = "AI | Motion Warp")
        bool bPreserveZHeight = true;
#pragma endregion
};