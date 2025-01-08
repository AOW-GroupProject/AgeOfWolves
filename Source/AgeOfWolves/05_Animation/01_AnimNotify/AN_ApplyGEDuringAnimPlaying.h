#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AN_ApplyGEDuringAnimPlaying.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAN_ApplyGEDuringAnimPlaying, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UGameplayEffect;
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
 *	@UAN_ApplyGEDuringAnimPlaying
 * 
 *	Anim Montage 플레이 중 GE를 적용할 때 활용하는 Anim Notify
 */
UCLASS()
class AGEOFWOLVES_API UAN_ApplyGEDuringAnimPlaying : public UAnimNotify
{
	GENERATED_BODY()
	
//@Defualt Setting
#pragma region Default Setting
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
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
