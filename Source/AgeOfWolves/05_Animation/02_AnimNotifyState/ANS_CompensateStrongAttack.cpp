#include "ANS_CompensateStrongAttack.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/02_GamePlayAbility/AttackGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogANS_CompensateStrongAttack)

UANS_CompensateStrongAttack::UANS_CompensateStrongAttack()
{
    AttackAbilityRef.Reset();
}

void UANS_CompensateStrongAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    UAttackGameplayAbility* AttackAbility = GetAnimatingAbility(MeshComp);
    if (!AttackAbility)
    {
        return;
    }

    if (!AttackAbility)
    {
        UE_LOGFMT(LogANS_CompensateStrongAttack, Warning, "강공격 파훼 구간 시작 실패 - 활성화된 AttackGameplayAbility를 찾을 수 없음");
        return;
    }

    //@CompensationTask 활성화
    AttackAbility->ActivateCompensationTask(bOnlyTriggerOnce);

    //@참조 저장
    AttackAbilityRef = AttackAbility;

    UE_LOGFMT(LogANS_CompensateStrongAttack, Log, "강공격 파훼 구간 시작 성공 - 캐릭터: {0}, 어빌리티: {1}, OnlyTriggerOnce: {2}",
        *AttackAbility->GetAvatarActorFromActorInfo()->GetName(),
        *AttackAbility->GetName(),
        bOnlyTriggerOnce);
}

void UANS_CompensateStrongAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@CompensationTask 비활성화
    if (AttackAbilityRef.IsValid())
    {
        AttackAbilityRef->DeactivateCompensationTask();
        UE_LOGFMT(LogANS_CompensateStrongAttack, Log, "강공격 파훼 태스크 비활성화 완료");
    }

    //@참조 정리
    AttackAbilityRef.Reset();

    UE_LOGFMT(LogANS_CompensateStrongAttack, Log, "강공격 파훼 구간 종료 완료");
}

FString UANS_CompensateStrongAttack::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("강공격 파훼 구간"));
}

UAttackGameplayAbility* UANS_CompensateStrongAttack::GetAnimatingAbility(USkeletalMeshComponent* MeshComp)
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