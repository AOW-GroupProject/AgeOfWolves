#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_CompensateStrongAttack.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_CompensateStrongAttack, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySystemComponent;
class UAttackGameplayAbility;
class UAT_CompensateDamage;
#pragma endregion

/**
 *  @UANS_CompensateStrongAttack
 *
 *  강공격 파훼 매커니즘을 특정 구간 동안 활성화하는 Anim Notify State를 정의합니다.
 */
UCLASS(meta = (DisplayName = "강공격 파훼 구간"))
class AGEOFWOLVES_API UANS_CompensateStrongAttack : public UAnimNotifyState
{
    GENERATED_BODY()

    //@Defualt Setting
#pragma region Default Setting
public:
    UANS_CompensateStrongAttack();

protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@한 번만 트리거할지 여부
    UPROPERTY(EditAnywhere, Category = "강공격 파훼")
    bool bOnlyTriggerOnce = false;
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    UAttackGameplayAbility* GetAnimatingAbility(USkeletalMeshComponent* MeshComp);

private:
    //@바인딩된 AttackGameplayAbility 참조
    UPROPERTY()
    TWeakObjectPtr<UAttackGameplayAbility> AttackAbilityRef;
#pragma endregion
};