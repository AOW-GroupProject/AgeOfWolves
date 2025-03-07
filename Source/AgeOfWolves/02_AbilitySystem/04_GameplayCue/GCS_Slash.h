#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"

#include "GCS_Slash.generated.h"

/**
 *	@UGCS_Slash
 * 
 *	회전 값 설정이 가능한 Slash 이팩트 구현을 위한 Gameplay Cue Notify Static
 */
UCLASS()
class AGEOFWOLVES_API UGCS_Slash : public UGameplayCueNotify_Static
{
    GENERATED_BODY()

public:
    UGCS_Slash();

    virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
        const FGameplayCueParameters& Parameters) override;

    // 이펙트 관련 변수들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
        UParticleSystem* SlashEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
        USoundBase* SlashSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
        float EffectScale;

    // 회전 관련 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
        FRotator DefaultRotation;
};