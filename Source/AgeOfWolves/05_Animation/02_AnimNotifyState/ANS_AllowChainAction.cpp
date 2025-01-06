#include "ANS_AllowChainAction.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogANS_AllowChainAction)

UANS_AllowChainAction::UANS_AllowChainAction()
{
    BaseASCRef.Reset();
}

void UANS_AllowChainAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    //@Owner
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 Owner");
        return;
    }

    //@Character
    ACharacterBase* Character = Cast<ACharacterBase>(Owner);
    if (!Character)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 소유자: {0}가 CharacterBase가 아님",
            *Owner->GetName());
        return;
    }

    //@ASC
    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 소유자: {0}에서 AbilitySystemComponent를 찾을 수 없음",
            *Character->GetName());
        return;
    }

    //@Base ASC
    UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(ASC);
    if (!BaseASC)
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 시작 실패 - 소유자: {0}의 AbilitySystemComponent가 BaseAbilitySystemComponent가 아님",
            *Character->GetName());
        return;
    }

    //@캐싱
    BaseASCRef = BaseASC;

    //@Chain Window 허용
    BaseASCRef->StartChainWindow();
}

void UANS_AllowChainAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@Base ASC Ref
    if (!BaseASCRef.IsValid())
    {
        UE_LOGFMT(LogANS_AllowChainAction, Warning, "체인 윈도우 종료 실패 - 유효하지 않은 AbilitySystemComponent 레퍼런스");
        return;
    }

    UE_LOGFMT(LogANS_AllowChainAction, Log, "체인 윈도우 종료");

    //@Chain Window 종료
    BaseASCRef->EndChainWindow();

    //@Reset
    BaseASCRef.Reset();
}