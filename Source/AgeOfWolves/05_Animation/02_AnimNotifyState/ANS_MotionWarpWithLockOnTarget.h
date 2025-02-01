#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"

#include "ANS_MotionWarpWithLockOnTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_MotionWarpWithLockOnTarget, Log, All)

//@���� ����
#pragma region Forward Declaration
#pragma endregion

//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
#pragma endregion

/**
 *	@UANS_MotionWarpWithLockOnTarget
 *
 *	Lock On Target�� Motion Warp �ϱ� ���� Ȱ��Ǵ� ANS�� �����մϴ�.
 */
	UCLASS()
	class AGEOFWOLVES_API UANS_MotionWarpWithLockOnTarget : public UAnimNotifyState_MotionWarping
{
	GENERATED_BODY()

		//@ģ�� Ŭ����
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
