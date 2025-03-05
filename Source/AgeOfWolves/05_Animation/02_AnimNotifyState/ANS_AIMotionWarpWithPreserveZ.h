#pragma once

#include "CoreMinimal.h"
#include "ANS_AIMotionWarpWithTarget.h"
#include "RootMotionModifier_SkewWarp.h"

#include "ANS_AIMotionWarpWithPreserveZ.generated.h" 

DECLARE_LOG_CATEGORY_EXTERN(LogMotionWarpWithPreserveZ, Log, All);


// Z축 보존을 위한 커스텀 루트 모션 모디파이어
UCLASS()
class AGEOFWOLVES_API URootMotionModifier_SkewWarpWithZ : public URootMotionModifier_SkewWarp
{
    GENERATED_BODY()

public:
    // Z축 변위를 보존할지 여부
    UPROPERTY(EditAnywhere, Category = "Config")
        bool bPreserveZAxis = true;

    // 루트 모션 변환을 계산할 때 오버라이드
    virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;
};

// Z축 보존을 위한 커스텀 애니메이션 노티파이 스테이트
UCLASS(meta = (DisplayName = "AI Motion Warp (Z축 보존)"))
class AGEOFWOLVES_API UANS_AIMotionWarpWithPreserveZ : public UANS_AIMotionWarpWithTarget
{
    GENERATED_BODY()

public:
    UANS_AIMotionWarpWithPreserveZ(const FObjectInitializer& ObjectInitializer);

    // Z축 보존 여부
    UPROPERTY(EditAnywhere, Category = "AI | Motion Warp")
        bool bPreserveZAxis = true;

    // 루트 모션 모디파이어 생성 오버라이드
    virtual URootMotionModifier* AddRootMotionModifier_Implementation(
        UMotionWarpingComponent* MotionWarpingComp,
        const UAnimSequenceBase* Animation,
        float StartTime,
        float EndTime) const override;
};