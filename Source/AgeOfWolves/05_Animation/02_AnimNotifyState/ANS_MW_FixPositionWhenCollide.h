#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"

#include "ANS_MW_FixPositionWhenCollide.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_MW_FixPosition, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class UMotionWarpingComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UANS_MW_FixPositionWhenCollide
 *
 *	전방 충돌 시 Motion Warping을 활용하여 캐릭터 위치를 고정합니다.
 *  표면을 따라 미끄러지는 루트 모션 동작을 방지합니다.
 */
UCLASS(meta = (DisplayName = "표면 미끄러짐 이동 방지"))
class AGEOFWOLVES_API UANS_MW_FixPositionWhenCollide : public UAnimNotifyState_MotionWarping
{
    //@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

    //@Defualt Setting
#pragma region Default Setting
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    // 충돌 감지 거리 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float ForwardCheckDistance = 100.0f;

    // Motion Warping 타겟 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionWarping")
    FName WarpTargetName = "BlockedPosition";

    // 디버그 시각화 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bEnableDebugDraw = false;

protected:
    // 캐싱된 참조
    UPROPERTY(Transient)
    TWeakObjectPtr<ACharacterBase> OwnerCharacter;

    UPROPERTY(Transient)
    TWeakObjectPtr<UMotionWarpingComponent> MotionWarpingComp;

    // 마지막 안전 위치
    UPROPERTY(Transient)
    FVector LastSafePosition;

    // 충돌 상태
    UPROPERTY(Transient)
    bool bIsBlocked;
#pragma endregion

    //@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    // 전방 충돌 감지 함수
    bool DetectForwardCollision(USkeletalMeshComponent* MeshComp);
#pragma endregion

};