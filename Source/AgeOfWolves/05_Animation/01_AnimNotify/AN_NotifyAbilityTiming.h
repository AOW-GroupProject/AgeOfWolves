#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AN_NotifyAbilityTiming.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAN_NotifyAbilityTiming, Log, All)

/**
 *	@UAN_NotifyAbilityTiming
 * 
 *	애니메이션을 재생 중인 어빌리티에게 특정 시점을 알리는 Anim Notify입니다.
 */
UCLASS()
class AGEOFWOLVES_API UAN_NotifyAbilityTiming : public UAnimNotify
{
	GENERATED_BODY()
        //@Defualt Setting
#pragma region Default Setting
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

};
