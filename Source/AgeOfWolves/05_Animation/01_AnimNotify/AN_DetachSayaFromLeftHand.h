#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DetachSayaFromLeftHand.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSayaDetach, Log, All);

/**
 * @UAN_DetachSayaFromLeftHand
 *
 * Saya를 왼손에서 떼고 FullWeapon 소켓에 부착하는 애니메이션 노티파이
 */
UCLASS(meta = (DisplayName = "사야를 왼손에서 떼기"))
class AGEOFWOLVES_API UAN_DetachSayaFromLeftHand : public UAnimNotify
{
    GENERATED_BODY()

public:
    UAN_DetachSayaFromLeftHand();

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    UPROPERTY(EditAnywhere, Category = "사야 분리")
    FName FullWeaponSocketName = "FullWeapon";
};