#include "ANS_DisableLockOn.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "05_Animation/BaseAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogANS_DisableLockOn)

// ANS_DisableLockOn.cpp
void UANS_DisableLockOn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    //@컴포넌트와 소유자 유효성 체크
    if (!MeshComp || !MeshComp->GetOwner())
    {
        UE_LOGFMT(LogANS_DisableLockOn, Warning, "메시 컴포넌트 또는 소유자가 유효하지 않습니다");
        return;
    }

    //@플레이어 캐릭터 캐스팅
    PlayerCharacterRef = Cast<APlayerCharacter>(MeshComp->GetOwner());
    if (!PlayerCharacterRef.IsValid())
    {
        UE_LOGFMT(LogANS_DisableLockOn, Warning, "플레이어 캐릭터 레퍼런스를 가져오는데 실패했습니다");
        return;
    }

    //@캐릭터 무브먼트 컴포넌트 가져오기
    CharacterMovementCompRef = PlayerCharacterRef->GetCharacterMovement();
    if (!CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogANS_DisableLockOn, Warning, "캐릭터 무브먼트 레퍼런스를 가져오는데 실패했습니다");
        return;
    }

    //@현재 회전 설정 저장
    bWasUsingControllerRotation = CharacterMovementCompRef->bUseControllerDesiredRotation;
    bWasOrientRotationToMovement = CharacterMovementCompRef->bOrientRotationToMovement;

    //@회전 제어 비활성화
    CharacterMovementCompRef->bUseControllerDesiredRotation = false;
    CharacterMovementCompRef->bOrientRotationToMovement = false;

    UE_LOGFMT(LogANS_DisableLockOn, Log, "캐릭터 회전 제어가 비활성화되었습니다");
}

void UANS_DisableLockOn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    //@필요한 경우 프레임별 업데이트 추가
}

void UANS_DisableLockOn::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    //@무브먼트 컴포넌트 유효성 체크
    if (!CharacterMovementCompRef.IsValid())
    {
        UE_LOGFMT(LogANS_DisableLockOn, Warning, "노티파이 종료 시 캐릭터 무브먼트 레퍼런스가 유효하지 않습니다");
        return;
    }

    //@이전 회전 설정 복원
    CharacterMovementCompRef->bUseControllerDesiredRotation = bWasUsingControllerRotation;
    CharacterMovementCompRef->bOrientRotationToMovement = bWasOrientRotationToMovement;

    UE_LOGFMT(LogANS_DisableLockOn, Log, "캐릭터 회전 제어가 복원되었습니다");
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
#pragma endregion