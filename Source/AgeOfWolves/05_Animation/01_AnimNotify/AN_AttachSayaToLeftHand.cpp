#include "AN_AttachSayaToLeftHand.h"
#include "Logging/StructuredLog.h"

#include "Components/StaticMeshComponent.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogSayaAttach);

UAN_AttachSayaToLeftHand::UAN_AttachSayaToLeftHand()
{
    // 기본 소켓 이름 설정
    SayaSocketName = "Saya";
}

void UAN_AttachSayaToLeftHand::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogSayaAttach, Warning, "Notify 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogSayaAttach, Warning, "Notify 실패 - 사유: 소유 캐릭터가 유효하지 않음");
        return;
    }

    // Saya 메시 컴포넌트 가져오기
    UStaticMeshComponent* SayaMesh = Character->GetSayaMesh();
    if (!SayaMesh)
    {
        UE_LOGFMT(LogSayaAttach, Error, "SayaMesh 컴포넌트를 찾을 수 없습니다.");
        return;
    }

    // Saya 메시를 지정된 소켓에 부착
    SayaMesh->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SayaSocketName);

    UE_LOGFMT(LogSayaAttach, Log,
        "Saya 부착 완료 - SayaMesh를 {0} 소켓에 부착", *SayaSocketName.ToString());
}