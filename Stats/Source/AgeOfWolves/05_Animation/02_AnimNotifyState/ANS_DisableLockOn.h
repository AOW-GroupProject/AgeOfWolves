#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_DisableLockOn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_DisableLockOn, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UCharacterMovementComponent;
class APlayerCharacter;
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
 *	@UANS_DisableLockOn
 * 
 *	ANS 실행 시간 동안 LockOn을 해제합니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_DisableLockOn : public UAnimNotifyState
{
	GENERATED_BODY()
	
//@Defualt Setting
#pragma region Default Setting
protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    //@캐릭터 레퍼런스
    UPROPERTY()
        TWeakObjectPtr<APlayerCharacter> PlayerCharacterRef;

    //@캐릭터 무브먼트 레퍼런스
    UPROPERTY()
        TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementCompRef;

    //@원래 회전 설정 저장용 변수들
    bool bWasUsingControllerRotation;
    bool bWasOrientRotationToMovement;
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
