#include "ANS_DisableRootMotionWhenCollide.h"
#include "Logging/StructuredLog.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogANS_DisableRootMotion)

UANS_DisableRootMotionWhenCollide::UANS_DisableRootMotionWhenCollide(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsNativeBranchingPoint = true;
}

void UANS_DisableRootMotionWhenCollide::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    //@소유 캐릭터 찾기
    if (!MeshComp || !MeshComp->GetOwner())
        return;

    OwnerCharacter = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!OwnerCharacter.IsValid())
        return;

    //@초기화 - 충돌 감지 플래그 리셋
    bIsBlocked = false;
    bHasDisabledRootMotion = false;

    //@초기 안전 위치 저장
    LastSafePosition = OwnerCharacter->GetActorLocation();

    //@애님 인스턴스 캐싱
    AnimInstance = MeshComp->GetAnimInstance();

    if (bEnableDebugDraw)
    {
        UE_LOGFMT(LogANS_DisableRootMotion, Log, "충돌 시 루트 모션 비활성화 감지 시작: {0}",
            *Animation->GetName());
    }
}

void UANS_DisableRootMotionWhenCollide::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    //@필수 객체 유효성 검사
    if (!OwnerCharacter.IsValid() || !AnimInstance.IsValid())
        return;

    //@현재 프레임의 충돌 상태 확인
    bool bCurrentlyBlocked = DetectForwardCollision(MeshComp);

    //@충돌 상태 변화 감지 및 루트 모션 제어
    if (bCurrentlyBlocked != bIsBlocked)
    {
        bIsBlocked = bCurrentlyBlocked;

        if (bIsBlocked)
        {
            //@충돌 감지 시 루트 모션 비활성화
            if (!bHasDisabledRootMotion)
            {
                // 현재 재생 중인 몽타주 찾기
                UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
                if (CurrentMontage)
                {
                    // 현재 몽타주 인스턴스 찾기
                    FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance();
                    if (MontageInstance)
                    {
                        // 루트 모션 비활성화 스택 증가
                        MontageInstance->PushDisableRootMotion();
                        bHasDisabledRootMotion = true;

                        if (bEnableDebugDraw)
                        {
                            UE_LOGFMT(LogANS_DisableRootMotion, Log, "전방 충돌 감지: 루트 모션 비활성화");
                        }

                        //@충돌 시 위치 고정 (선택적)
                        if (bFixPositionOnCollision)
                        {
                            FVector Forward = OwnerCharacter->GetActorForwardVector();
                            FVector SafePosition = LastSafePosition - (Forward * SafePositionBackOffset);
                            OwnerCharacter->SetActorLocation(SafePosition, false);

                            if (bEnableDebugDraw)
                            {
                                UE_LOGFMT(LogANS_DisableRootMotion, Log, "캐릭터 위치 안전 위치로 고정: {0}",
                                    *SafePosition.ToString());
                                DrawDebugSphere(MeshComp->GetWorld(), SafePosition, 20.0f, 8, FColor::Red, false, 0.5f);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            //@충돌 해제 시 루트 모션 복원
            if (bHasDisabledRootMotion)
            {
                // 현재 몽타주 인스턴스 찾기
                FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance();
                if (MontageInstance)
                {
                    // 루트 모션 비활성화 스택 감소
                    MontageInstance->PopDisableRootMotion();
                    bHasDisabledRootMotion = false;

                    if (bEnableDebugDraw)
                    {
                        UE_LOGFMT(LogANS_DisableRootMotion, Log, "충돌 해제 감지: 루트 모션 복원");
                    }
                }
            }
        }
    }

    //@안전 위치 업데이트 (충돌이 없을 때만)
    if (!bIsBlocked)
    {
        LastSafePosition = OwnerCharacter->GetActorLocation();
    }
}

void UANS_DisableRootMotionWhenCollide::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    //@종료 시 루트 모션 복원 확인
    if (bHasDisabledRootMotion && AnimInstance.IsValid())
    {
        FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance();
        if (MontageInstance)
        {
            // 루트 모션 비활성화 스택 감소
            MontageInstance->PopDisableRootMotion();
            bHasDisabledRootMotion = false;

            if (bEnableDebugDraw)
            {
                UE_LOGFMT(LogANS_DisableRootMotion, Log, "노티파이 종료: 루트 모션 복원됨");
            }
        }
    }
}

#if WITH_EDITOR
bool UANS_DisableRootMotionWhenCollide::CanBePlaced(UAnimSequenceBase* Animation) const
{
    return (Animation && Animation->IsA(UAnimMontage::StaticClass()));
}
#endif

FString UANS_DisableRootMotionWhenCollide::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("충돌 시 루트 모션 비활성화"));
}

bool UANS_DisableRootMotionWhenCollide::DetectForwardCollision(USkeletalMeshComponent* MeshComp)
{
    //@캐릭터 유효성 검사
    if (!OwnerCharacter.IsValid())
        return false;

    //@캐릭터 위치 및 전방 방향
    FVector Start = OwnerCharacter->GetActorLocation();

    //@캐릭터 눈높이 정도에서 시작
    float HalfHeight = OwnerCharacter->GetDefaultHalfHeight();
    Start.Z += HalfHeight * 0.5f;

    FVector Forward = OwnerCharacter->GetActorForwardVector();
    FVector End = Start + Forward * ForwardCheckDistance;

    //@충돌 설정
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter.Get());

    //@충돌 체크 수행
    bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

    //@디버그 시각화
    if (bEnableDebugDraw)
    {
        FColor LineColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugLine(MeshComp->GetWorld(), Start, End, LineColor, false, 0.1f, 0, 2.0f);

        if (bHit)
        {
            DrawDebugSphere(MeshComp->GetWorld(), Hit.Location, 10.0f, 8, FColor::Orange, false, 0.1f);
            UE_LOGFMT(LogANS_DisableRootMotion, Log, "전방 충돌 감지: {0}",
                Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("Unknown"));
        }
    }

    return bHit;
}