#include "ANS_InteractionTrace.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "02_AbilitySystem/02_GamePlayAbility/InteractionGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogANS_InteractionTrace)

//@Defualt Setting
#pragma region Default Setting
void UANS_InteractionTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    UInteractionGameplayAbility* Ability = GetAnimatingAbility(MeshComp);
    if (!Ability)
    {
        UE_LOGFMT(LogANS_InteractionTrace, Warning, "NotifyBegin 실패 - 사유: 유효한 상호작용 어빌리티가 없음");
        return;
    }

    UE_LOGFMT(LogANS_InteractionTrace, Log, "상호작용 트레이스 시작 - Ability: {0}", *Ability->GetName());
    
    //@Start Trace
    Ability->StartInteractionTrace();
}

void UANS_InteractionTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    UInteractionGameplayAbility* Ability = GetAnimatingAbility(MeshComp);
    if (!Ability)
    {
        return;
    }

    //@Process Trace
    Ability->ProcessInteractionTrace();
}

void UANS_InteractionTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    UInteractionGameplayAbility* Ability = GetAnimatingAbility(MeshComp);
    if (!Ability)
    {
        UE_LOGFMT(LogANS_InteractionTrace, Warning, "NotifyEnd 실패 - 사유: 유효한 상호작용 어빌리티가 없음");
        return;
    }

    UE_LOGFMT(LogANS_InteractionTrace, Log, "상호작용 트레이스 종료 - Ability: {0}", *Ability->GetName());

    //@End Trace
    Ability->EndInteractionTrace(false);
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
FString UANS_InteractionTrace::GetNotifyName_Implementation() const
{
    return FString(TEXT("상호작용 트레이스 활성화"));
}

UInteractionGameplayAbility* UANS_InteractionTrace::GetAnimatingAbility(USkeletalMeshComponent* MeshComp)
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
        return Cast<UInteractionGameplayAbility>(ASC->GetAnimatingAbility());
    }

    return nullptr;
}
#pragma endregion