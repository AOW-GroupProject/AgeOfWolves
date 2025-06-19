#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "05_Animation/BaseAnimInstance.h"

#include "ANS_DirectionalMovement.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_DirectionalMovement, Log, All)

class ACharacterBase;

/**
*   @UANS_DirectionalMovement
*
*   방향, 거리, 속도를 사용하여 캐릭터를 VInterpTo로 부드럽게 이동시키는 ANS
*   충돌 감지 시 이동 중단
*/
UCLASS(meta = (DisplayName = "방향 설정 부드러운 이동"))
class AGEOFWOLVES_API UANS_DirectionalMovement : public UAnimNotifyState
{
    GENERATED_BODY()

    //@Default Setting
#pragma region Default Setting
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Settings
protected:
    //@이동 방향
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "방향 이동", meta = (AllowPrivateAccess = "true"))
    EMovementDirection MovementDirection = EMovementDirection::Fwd;

    //@이동 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "방향 이동", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
    float MovementDistance = 200.0f;

    //@이동 속도 (VInterpTo Speed)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "방향 이동", meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
    float MovementSpeed = 5.0f;

    //@충돌 감지 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "충돌 감지", meta = (AllowPrivateAccess = "true", ClampMin = "10.0"))
    float CollisionCheckDistance = 100.0f;

    //@디버그 그리기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "디버그", meta = (AllowPrivateAccess = "true"))
    bool bDrawDebug = false;
#pragma endregion

    //@Private Variables
#pragma region Private Variables
private:
    //@시작 위치
    FVector StartLocation;

    //@목표 위치
    FVector TargetLocation;

    //@이동 완료 여부
    bool bMovementCompleted;
#pragma endregion

    //@Utility
#pragma region Utility
private:
    //@방향에 따른 벡터 계산
    FVector CalculateDirectionVector(ACharacterBase* Character);

    //@충돌 감지 (MovementDirection 기준)
    bool DetectCollisionInDirection(ACharacterBase* Character);

    //@디버그 정보 그리기
    void DrawDebugInfo(ACharacterBase* Character, const FVector& CurrentLocation, float Duration);
#pragma endregion
};