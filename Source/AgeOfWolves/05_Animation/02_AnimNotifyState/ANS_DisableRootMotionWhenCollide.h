#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_DisableRootMotionWhenCollide.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_DisableRootMotion, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class UAnimInstance;
#pragma endregion

/**
 *	@UANS_DisableRootMotionOnCollision
 *
 *	전방 충돌 감지 시 몽타주의 루트 모션을 비활성화합니다.
 *  충돌이 해제되면 루트 모션을 자동으로 복원합니다.
 *  표면을 따라 미끄러지는 루트 모션 동작을 방지합니다.
 */
UCLASS(meta = (DisplayName = "충돌 시 루트 모션 비활성화"))
class AGEOFWOLVES_API UANS_DisableRootMotionWhenCollide : public UAnimNotifyState
{
    //@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

    //@Defualt Setting
#pragma region Default Setting
public:
    UANS_DisableRootMotionWhenCollide(const FObjectInitializer& ObjectInitializer);

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

#if WITH_EDITOR
    virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override;
#endif

    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    // 충돌 감지 거리 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float ForwardCheckDistance = 100.0f;

    // 충돌 시 위치 고정 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bFixPositionOnCollision = false;

    // 안전 위치 뒤로 이동 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (EditCondition = "bFixPositionOnCollision"))
    float SafePositionBackOffset = 5.0f;

    // 디버그 시각화 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bEnableDebugDraw = false;

protected:
    // 캐싱된 참조
    UPROPERTY(Transient)
    TWeakObjectPtr<ACharacterBase> OwnerCharacter;

    // 애님 인스턴스 참조
    UPROPERTY(Transient)
    TWeakObjectPtr<UAnimInstance> AnimInstance;

    // 마지막 안전 위치
    UPROPERTY(Transient)
    FVector LastSafePosition;

    // 충돌 상태
    UPROPERTY(Transient)
    bool bIsBlocked;

    // 루트 모션 비활성화 여부
    UPROPERTY(Transient)
    bool bHasDisabledRootMotion;
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    // 전방 충돌 감지 함수
    bool DetectForwardCollision(USkeletalMeshComponent* MeshComp);
#pragma endregion

};