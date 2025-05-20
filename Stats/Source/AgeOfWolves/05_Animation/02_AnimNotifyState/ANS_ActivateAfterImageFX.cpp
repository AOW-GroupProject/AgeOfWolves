#include "ANS_ActivateAfterImageFX.h"
#include "Logging/StructuredLog.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"

#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogANS_AfterImageFX)

//@Defualt Setting
#pragma region Default Setting
UANS_ActivateAfterImageFX::UANS_ActivateAfterImageFX()
{
    // 기본 생성자 - 특별한 초기화가 필요 없음
}

void UANS_ActivateAfterImageFX::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_AfterImageFX, Warning, "NotifyBegin 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@캐릭터 베이스 참조 획득
    ACharacterBase* CharacterBase = GetCharacterBase(MeshComp);
    if (!CharacterBase)
    {
        UE_LOGFMT(LogANS_AfterImageFX, Warning, "NotifyBegin 실패 - 사유: 캐릭터 베이스를 찾을 수 없음");
        return;
    }

    //@SkeletalFXComponent 가져오기
    UNiagaraComponent* SkeletalFXComponent = CharacterBase->GetSkeletalFXComponent();
    if (!SkeletalFXComponent)
    {
        UE_LOGFMT(LogANS_AfterImageFX, Warning, "NotifyBegin 실패 - 사유: SkeletalFXComponent가 유효하지 않음");
        return;
    }

    //@컴포넌트 활성화
    SkeletalFXComponent->Activate();

    UE_LOGFMT(LogANS_AfterImageFX, Log, "잔상 효과 활성화 - Owner: {0}", *CharacterBase->GetName());
}

void UANS_ActivateAfterImageFX::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_AfterImageFX, Warning, "NotifyEnd 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@캐릭터 베이스 참조 획득
    ACharacterBase* CharacterBase = GetCharacterBase(MeshComp);
    if (!CharacterBase)
    {
        UE_LOGFMT(LogANS_AfterImageFX, Warning, "NotifyEnd 실패 - 사유: 캐릭터 베이스를 찾을 수 없음");
        return;
    }

    //@SkeletalFXComponent 가져오기
    UNiagaraComponent* SkeletalFXComponent = CharacterBase->GetSkeletalFXComponent();
    if (!SkeletalFXComponent)
    {
        UE_LOGFMT(LogANS_AfterImageFX, Warning, "NotifyEnd 실패 - 사유: SkeletalFXComponent가 유효하지 않음");
        return;
    }

    //@컴포넌트 비활성화
    SkeletalFXComponent->Deactivate();

    UE_LOGFMT(LogANS_AfterImageFX, Log, "잔상 효과 비활성화 - Owner: {0}", *CharacterBase->GetName());
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
ACharacterBase* UANS_ActivateAfterImageFX::GetCharacterBase(USkeletalMeshComponent* MeshComp)
{
    if (!IsValid(MeshComp))
    {
        return nullptr;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        return nullptr;
    }

    return Cast<ACharacterBase>(Owner);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FString UANS_ActivateAfterImageFX::GetNotifyName_Implementation() const
{
    return FString(TEXT("잔상 효과 활성화"));
}
#pragma endregion