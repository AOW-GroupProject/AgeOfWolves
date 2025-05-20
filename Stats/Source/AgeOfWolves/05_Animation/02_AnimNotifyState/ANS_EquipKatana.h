#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_EquipKatana.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKatanaEquip, Log, All);

/**
 * @UANS_EquipKatana
 * 
 * 카타나 장착 과정을 처리하는 애니메이션 노티파이 스테이트
 * 애니메이션 시작 시 FullWeapon 비활성화 및 개별 부품 활성화
 * 애니메이션 종료 시 Saya를 소켓에 부착
 */
UCLASS(meta = (DisplayName = "카타나 장착"))
class AGEOFWOLVES_API UANS_EquipKatana : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UANS_EquipKatana();

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    UPROPERTY(EditAnywhere, Category = "카타나 장착")
        FName WeaponSocketName = "Katana";

    UPROPERTY(EditAnywhere, Category = "카타나 장착")
        FName SayaSocketName = "Saya";

    UPROPERTY(EditAnywhere, Category = "카타나 장착")
        FName FullWeaponSocketName = "FullWeapon";
};