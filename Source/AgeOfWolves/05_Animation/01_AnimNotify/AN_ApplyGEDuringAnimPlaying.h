#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AN_ApplyGEDuringAnimPlaying.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAN_ApplyGEDuringAnimPlaying, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UGameplayEffect;
#pragma endregion

/**
 *	@UAN_ApplyGEDuringAnimPlaying
 *
 *	Anim Montage 플레이 중 GE를 적용할 때 활용하는 Anim Notify
 *  에디터에서 직접 GE를 설정하거나, 현재 실행 중인 어빌리티의 GE를 사용할 수 있습니다.
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

    //@Property
#pragma region Property
protected:
    /** 에디터에서 직접 설정할 수 있는 GameplayEffect 클래스 */
    UPROPERTY(EditAnywhere, Category = "GameplayEffect")
        TSubclassOf<UGameplayEffect> GameplayEffectClass;
#pragma endregion
};