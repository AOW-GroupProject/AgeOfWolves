#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_NotifyPlayingRootMotion.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_NotifyPlayingRootMotion, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAnimInstance;
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
 *	@UANS_NotifyPlayingRootMotion
 * 
 *	캐릭터 애니메이션 상태 기계에게 현재 사용자 입력이 아닌, 루트 모션 재생에 의한 이동이라는 것을 알립니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_NotifyPlayingRootMotion : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_NotifyPlayingRootMotion();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


private:
	UPROPERTY()
		TWeakObjectPtr<UBaseAnimInstance> AnimInstanceRef;
#pragma endregion
};
