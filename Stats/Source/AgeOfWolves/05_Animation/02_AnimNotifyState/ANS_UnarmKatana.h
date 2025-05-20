#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_UnarmKatana.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKatanaUnarm, Log, All);

/**
 * @ANS_UnarmKatana
 * 
 * 카타나 해제 과정을 처리하는 애니메이션 노티파이 스테이트
 * 애니메이션 시작 시 Saya를 소켓에 부착
 * 애니메이션 종료 시 개별 부품 비활성화 및 전체 무기 활성화
 */
UCLASS(meta = (DisplayName = "카타나 해제"))
class AGEOFWOLVES_API UANS_UnarmKatana : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UANS_UnarmKatana();

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    UPROPERTY(EditAnywhere, Category = "카타나 해제")
        FName WeaponSocketName = "Katana";

    UPROPERTY(EditAnywhere, Category = "카타나 해제")
        FName SayaSocketName = "Saya";

    UPROPERTY(EditAnywhere, Category = "카타나 해제")
        FName FullWeaponSocketName = "FullWeapon";
};