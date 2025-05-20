#include "ANS_UnarmKatana.h"
#include "Logging/StructuredLog.h"

#include "Components/StaticMeshComponent.h"

#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogKatanaUnarm);

UANS_UnarmKatana::UANS_UnarmKatana()
{
    // 기본 소켓 이름 설정
    WeaponSocketName = "Katana";
    SayaSocketName = "Saya";
    FullWeaponSocketName = "FullWeapon";
}

void UANS_UnarmKatana::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    // 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogKatanaUnarm, Warning, "NotifyBegin 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogKatanaUnarm, Warning, "NotifyBegin 실패 - 사유: 소유 캐릭터가 유효하지 않음");
        return;
    }

    // 무기 컴포넌트 직접 접근
    UStaticMeshComponent* SayaMesh = Character->GetSayaMesh();

    // 컴포넌트 유효성 검사
    if (!SayaMesh)
    {
        UE_LOGFMT(LogKatanaUnarm, Error, "SayaMesh 컴포넌트를 찾을 수 없습니다.");
        return;
    }

    // 1. SayaMesh를 Saya 소켓에 붙이기
    SayaMesh->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SayaSocketName);

    UE_LOGFMT(LogKatanaUnarm, Log,
        "카타나 해제 시작 - SayaMesh를 {0} 소켓에 부착", *SayaSocketName.ToString());
}

void UANS_UnarmKatana::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogKatanaUnarm, Warning, "NotifyEnd 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogKatanaUnarm, Warning, "NotifyEnd 실패 - 사유: 소유 캐릭터가 유효하지 않음");
        return;
    }

    // 무기 컴포넌트 직접 접근
    UStaticMeshComponent* WeaponMesh = Character->GetWeaponMesh();
    UStaticMeshComponent* SayaMesh = Character->GetSayaMesh();
    UStaticMeshComponent* FullWeaponMesh = Character->GetFullWeaponMesh();

    // 컴포넌트 유효성 검사
    if (!WeaponMesh || !SayaMesh || !FullWeaponMesh)
    {
        UE_LOGFMT(LogKatanaUnarm, Error, "필요한 무기 컴포넌트를 찾을 수 없습니다.");
        return;
    }

    // 2. WeaponMesh와 SayaMesh 비활성화
    WeaponMesh->SetVisibility(false);
    SayaMesh->SetVisibility(false);

    // 3. FullWeaponMesh 활성화
    FullWeaponMesh->SetVisibility(true);

    UE_LOGFMT(LogKatanaUnarm, Log,
        "카타나 해제 완료 - WeaponMesh와 SayaMesh 비활성화, FullWeaponMesh 활성화");
}