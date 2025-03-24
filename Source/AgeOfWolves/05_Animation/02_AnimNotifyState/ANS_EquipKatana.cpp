#include "ANS_EquipKatana.h"
#include "Logging/StructuredLog.h"

#include "Components/StaticMeshComponent.h"

#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogKatanaEquip);

UANS_EquipKatana::UANS_EquipKatana()
{
    // 기본 소켓 이름 설정
    WeaponSocketName = "Katana";
    SayaSocketName = "Saya";
    FullWeaponSocketName = "FullWeapon";
}

void UANS_EquipKatana::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    // 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogKatanaEquip, Warning, "NotifyBegin 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogKatanaEquip, Warning, "NotifyBegin 실패 - 사유: 소유 캐릭터가 유효하지 않음");
        return;
    }

    // 무기 컴포넌트 직접 접근
    UStaticMeshComponent* WeaponMesh = Character->GetWeaponMesh();
    UStaticMeshComponent* SayaMesh = Character->GetSayaMesh();
    UStaticMeshComponent* FullWeaponMesh = Character->GetFullWeaponMesh();

    // 컴포넌트 유효성 검사
    if (!WeaponMesh || !SayaMesh || !FullWeaponMesh)
    {
        UE_LOGFMT(LogKatanaEquip, Error, "필요한 무기 컴포넌트를 찾을 수 없습니다.");
        return;
    }

    // 1. FullWeapon 비활성화
    FullWeaponMesh->SetVisibility(false);

    // 2. SayaMesh와 WeaponMesh 활성화
    SayaMesh->SetVisibility(true);
    WeaponMesh->SetVisibility(true);

    UE_LOGFMT(LogKatanaEquip, Log,
        "카타나 장착 시작 - FullWeaponMesh 비활성화, SayaMesh와 WeaponMesh 활성화");
}

void UANS_EquipKatana::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogKatanaEquip, Warning, "NotifyEnd 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogKatanaEquip, Warning, "NotifyEnd 실패 - 사유: 소유 캐릭터가 유효하지 않음");
        return;
    }

    // 무기 컴포넌트 직접 접근
    UStaticMeshComponent* SayaMesh = Character->GetSayaMesh();

    // 컴포넌트 유효성 검사
    if (!SayaMesh)
    {
        UE_LOGFMT(LogKatanaEquip, Error, "SayaMesh 컴포넌트를 찾을 수 없습니다.");
        return;
    }

    // 3. Saya Mesh를 FullWeapon 소켓에 붙이기
    SayaMesh->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FullWeaponSocketName);

    UE_LOGFMT(LogKatanaEquip, Log,
        "카타나 장착 완료 - SayaMesh를 {0} 소켓에 부착", *FullWeaponSocketName.ToString());
}