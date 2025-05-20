#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "02_AbilitySystem/02_GamePlayAbility/InteractionGameplayAbility.h"

#include "ANS_InteractionTrace.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_InteractionTrace, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UInteractionGameplayAbility;
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
 *	@UANS_InteractionTrace
 *
 *	상호작용 관련 충돌 트레이스를 수행합니다.
 */
UCLASS(meta = (DisplayName = "상호작용 트레이스 활성화"))
class AGEOFWOLVES_API UANS_InteractionTrace : public UAnimNotifyState
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
	virtual FString GetNotifyName_Implementation() const override;

private:
	UInteractionGameplayAbility* GetAnimatingAbility(USkeletalMeshComponent* MeshComp);
#pragma endregion

};