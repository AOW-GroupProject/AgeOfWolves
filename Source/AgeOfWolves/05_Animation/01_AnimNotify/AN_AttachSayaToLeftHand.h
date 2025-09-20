#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_AttachSayaToLeftHand.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSayaAttach, Log, All);

/**
 * @UAN_AttachSayaToLeftHand
 *
 * Saya를 왼손 소켓에 부착하는 애니메이션 노티파이
 */
UCLASS(meta = (DisplayName = "사야를 왼손에 부착"))
class AGEOFWOLVES_API UAN_AttachSayaToLeftHand : public UAnimNotify
{
    GENERATED_BODY()

public:
    UAN_AttachSayaToLeftHand();

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    UPROPERTY(EditAnywhere, Category = "사야 부착")
    FName SayaSocketName = "Saya";
};