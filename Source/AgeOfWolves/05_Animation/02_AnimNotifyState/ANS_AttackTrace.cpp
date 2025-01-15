#include "ANS_AttackTrace.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/02_GamePlayAbility/AttackGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogANS_AttackTrace)

//@Defualt Setting
#pragma region Default Setting
void UANS_AttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    UAttackGameplayAbility* Ability = GetAnimatingAbility(MeshComp);
    if (!Ability)
    {
        return;
    }

    Ability->StartWeaponTrace();
}

void UANS_AttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    UAttackGameplayAbility* Ability = GetAnimatingAbility(MeshComp);
    if (!Ability)
    {
        return;
    }

    Ability->ProcessWeaponTrace();
}

void UANS_AttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    UAttackGameplayAbility* Ability = GetAnimatingAbility(MeshComp);
    if (!Ability)
    {
        return;
    }

    Ability->EndWeaponTrace();
}
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
UAttackGameplayAbility* UANS_AttackTrace::GetAnimatingAbility(USkeletalMeshComponent* MeshComp)
{
    //@Skeletal Mesh Comp, Owner
    if (!MeshComp || !MeshComp->GetOwner())
        return nullptr;

    //@Owner Character
    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!Character)
        return nullptr;

    //@ASC
    if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
    {
        return Cast<UAttackGameplayAbility>(ASC->GetAnimatingAbility());
    }

    return nullptr;
}
#pragma endregion