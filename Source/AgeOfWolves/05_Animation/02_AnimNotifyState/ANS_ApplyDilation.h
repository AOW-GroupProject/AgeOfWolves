#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "14_Subsystem/TimeManipulationSubsystem.h"

#include "ANS_ApplyDilation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_TimeDilation, Log, All);

/**
 *	@UANS_ApplyDilation
 *
 *	애니메이션 특정 재생 구간 동안 글로벌 타임 딜레이션을 구현합니다.
 */
UCLASS(meta = (DisplayName = "타임 딜레이션 적용"))
class AGEOFWOLVES_API UANS_ApplyDilation : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UANS_ApplyDilation();

protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    virtual FString GetNotifyName_Implementation() const override;

private:
    UPROPERTY(EditAnywhere, Category = "타임 딜레이션", meta = (DisplayName = "글로벌 적용"))
        bool bApplyGlobalDilation = true;

private:
    UPROPERTY(EditAnywhere, Category = "타임 딜레이션", meta = (DisplayName = "딜레이션 설정"))
        FTimeDilationSettings DilationSettings;
};