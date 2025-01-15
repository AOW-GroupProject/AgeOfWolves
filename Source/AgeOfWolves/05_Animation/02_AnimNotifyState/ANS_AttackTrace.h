#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_AttackTrace.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_AttackTrace, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UAttackGameplayAbility;
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
 *	@UANS_AttackTrace
 * 
 *	공격 관련 충돌 트레이스를 수행합니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_AttackTrace : public UAnimNotifyState
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
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
private:
    UAttackGameplayAbility* GetAnimatingAbility(USkeletalMeshComponent* MeshComp);
#pragma endregion
	
};
