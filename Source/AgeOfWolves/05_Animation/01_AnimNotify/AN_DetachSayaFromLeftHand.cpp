#include "AN_DetachSayaFromLeftHand.h"
#include "Logging/StructuredLog.h"

#include "Components/StaticMeshComponent.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogSayaDetach);

UAN_DetachSayaFromLeftHand::UAN_DetachSayaFromLeftHand()
{
    // 기본 소켓 이름 설정
    FullWeaponSocketName = "FullWeapon";
}

void UAN_DetachSayaFromLeftHand::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogSayaDetach, Warning, "Notify 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogSayaDetach, Warning, "Notify 실패 - 사유: 소유 캐릭터가 유효하지 않음");
        return;
    }

    // Saya 메시 컴포넌트 가져오기
    UStaticMeshComponent* SayaMesh = Character->GetSayaMesh();
    if (!SayaMesh)
    {
        UE_LOGFMT(LogSayaDetach, Error, "SayaMesh 컴포넌트를 찾을 수 없습니다.");
        return;
    }

    // Saya 메시를 FullWeapon 소켓에 부착 (왼손에서 떼고 허리로 이동)
    SayaMesh->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FullWeaponSocketName);

    UE_LOGFMT(LogSayaDetach, Log,
        "Saya 분리 및 재부착 완료 - SayaMesh를 {0} 소켓에 부착", *FullWeaponSocketName.ToString());
}