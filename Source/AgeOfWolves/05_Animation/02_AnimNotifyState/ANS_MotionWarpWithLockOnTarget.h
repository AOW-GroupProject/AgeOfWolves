#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"

#include "ANS_MotionWarpWithLockOnTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_MotionWarpWithLockOnTarget, Log, All)

//@전방 선언
#pragma region Forward Declaration
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
 *	@UANS_MotionWarpWithLockOnTarget
 * 
 *	Lock On Target에 Motion Warp 하기 위해 활용되는 ANS를 구현합니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_MotionWarpWithLockOnTarget : public UAnimNotifyState_MotionWarping
{
	GENERATED_BODY()

//@친추 클래스
#pragma region Friend Class
#pragma endregion

//@Defualt Setting
#pragma region Default Setting
protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

	
};
