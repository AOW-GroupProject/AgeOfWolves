#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"
#include "05_Animation/BaseAnimInstance.h"

#include "ANS_DirectionalMotionWarping.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_DirectionalMotionWarping, Log, All)

/**
*   @UANS_DirectionalMotionWarping
* 
*   방향과 거리 값을 사용하여 캐릭터를 이동시키는 모션 워핑 ANS
*/
UCLASS(meta = (DisplayName = "방향 설정 모션 워핑"))
class AGEOFWOLVES_API UANS_DirectionalMotionWarping : public UAnimNotifyState_MotionWarping
{
    GENERATED_BODY()

    //@Default Setting
#pragma region Default Setting
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Settings
protected:
    //@이동 방향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "모션 워핑", meta = (AllowPrivateAccess = "true"))
        EMovementDirection MovementDirection;

    //@이동 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "모션 워핑", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
        float MovementDistance = 200.0f;

    //@디버그 그리기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "디버그", meta = (AllowPrivateAccess = "true"))
        bool bDrawDebug = false;
#pragma endregion

//@Utility
#pragma region Utility
private:
    //@방향에 따른 벡터 계산
    FVector CalculateDirectionVector(ACharacterBase* Character);

    //@방향에 따른 회전 계산
    FRotator CalculateTargetRotation(ACharacterBase* Character);

    //@디버그 정보 그리기
    void DrawDebugInfo(ACharacterBase* Character, const FVector& StartLocation, const FVector& TargetLocation, float Duration);
#pragma endregion
};