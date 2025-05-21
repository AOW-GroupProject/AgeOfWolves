//#include "ANS_MW_FixPositionWhenCollide.h"
//#include "Logging/StructuredLog.h"
//
//#include "01_Character/CharacterBase.h"
//#include "MotionWarpingComponent.h"
//
//DEFINE_LOG_CATEGORY(LogANS_MW_FixPosition)
//
////@Defualt Setting
//#pragma region Default Setting
//void UANS_MW_FixPositionWhenCollide::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
//{
//    //@소유 캐릭터 찾기
//    if (!MeshComp || !MeshComp->GetOwner())
//        return;
//
//    OwnerCharacter = Cast<ACharacterBase>(MeshComp->GetOwner());
//    if (!OwnerCharacter.IsValid())
//        return;
//
//    //@Motion Warping 컴포넌트 가져오기
//    MotionWarpingComp = OwnerCharacter->FindComponentByClass<UMotionWarpingComponent>();
//    if (!MotionWarpingComp.IsValid())
//        return;
//
//    //@초기화 - 충돌 감지 플래그 리셋
//    bIsBlocked = false;
//
//    //@초기 안전 위치 저장
//    LastSafePosition = OwnerCharacter->GetActorLocation();
//
//    if (bEnableDebugDraw)
//    {
//        UE_LOGFMT(LogANS_MW_FixPosition, Log, "전방 충돌 감지 시작: {0}",
//            *Animation->GetName());
//    }
//}
//
//void UANS_MW_FixPositionWhenCollide::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
//{
//    //@필수 객체 유효성 검사
//    if (!OwnerCharacter.IsValid() || !MotionWarpingComp.IsValid())
//        return;
//
//    //@현재 프레임의 충돌 상태 확인
//    bool bCurrentlyBlocked = DetectForwardCollision(MeshComp);
//
//    //@충돌 상태 변화 감지
//    if (bCurrentlyBlocked != bIsBlocked)
//    {
//        bIsBlocked = bCurrentlyBlocked;
//
//        if (bIsBlocked)
//        {
//            // 충돌 시작 - 마지막 안전 위치에서 약간 뒤로 조정된 위치 계산
//            // 이 위치는 충돌 상태가 지속되는 동안 변경되지 않음
//            FVector Forward = OwnerCharacter->GetActorForwardVector();
//            LastSafePosition -= (Forward * SafePositionBackOffset);
//
//            if (bEnableDebugDraw)
//            {
//                UE_LOGFMT(LogANS_MW_FixPosition, Log, "전방 충돌 감지: 마지막 안전 위치({0})로 고정",
//                    *LastSafePosition.ToString());
//                DrawDebugSphere(MeshComp->GetWorld(), LastSafePosition, 20.0f, 8, FColor::Red, false, 0.5f);
//            }
//        }
//        else
//        {
//            //@충돌 해제 시 Warping 타겟 제거
//            MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
//
//            if (bEnableDebugDraw)
//            {
//                UE_LOGFMT(LogANS_MW_FixPosition, Log, "충돌 해제 감지: 전방 이동 제한 해제");
//            }
//        }
//    }
//
//    if (!bIsBlocked)
//    {
//        //@충돌이 없는 동안은 계속 안전 위치 업데이트
//        LastSafePosition = OwnerCharacter->GetActorLocation();
//    }
//    else
//    {
//        //@충돌 상태일 때는 매 프레임 같은 위치로 워프 타겟 지속 업데이트
//        FMotionWarpingTarget WarpTarget;
//        WarpTarget.Name = WarpTargetName;
//        WarpTarget.Location = LastSafePosition; // 충돌 시작 시 저장된 위치 사용
//        WarpTarget.Rotation = OwnerCharacter->GetActorRotation();
//
//        MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);
//    }
//}
//
//void UANS_MW_FixPositionWhenCollide::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
//{
//    Super::NotifyEnd(MeshComp, Animation);
//
//    //@종료 시 Warping 타겟 제거
//    if (MotionWarpingComp.IsValid())
//    {
//        MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
//
//        if (bEnableDebugDraw)
//        {
//            UE_LOGFMT(LogANS_MW_FixPosition, Log, "노티파이 종료: Warping 타겟 제거됨");
//        }
//    }
//
//    //@충돌 상태 초기화
//    bIsBlocked = false;
//}
//
//FString UANS_MW_FixPositionWhenCollide::GetNotifyName_Implementation() const
//{
//    return FString::Printf(TEXT("표면 미끄러짐 이동 방지"));
//}
//#pragma endregion
//
////@Property/Info...etc
//#pragma region Property or Subwidgets or Infos...etc
//#pragma endregion
//
////@Delegates
//#pragma region Delegates
//#pragma endregion
//
////@Callbacks
//#pragma region Callbacks
//#pragma endregion
//
////@Utility(Setter, Getter,...etc)
//#pragma region Utility
//bool UANS_MW_FixPositionWhenCollide::DetectForwardCollision(USkeletalMeshComponent* MeshComp)
//{
//    //@캐릭터 유효성 검사
//    if (!OwnerCharacter.IsValid())
//        return false;
//
//    //@캐릭터 위치 및 전방 방향
//    FVector Start = OwnerCharacter->GetActorLocation();
//
//    //@캐릭터 눈높이 정도에서 시작
//    float HalfHeight = OwnerCharacter->GetDefaultHalfHeight();
//    Start.Z += HalfHeight * 0.5f;
//
//    FVector Forward = OwnerCharacter->GetActorForwardVector();
//    FVector End = Start + Forward * ForwardCheckDistance;
//
//    //@충돌 설정
//    FHitResult Hit;
//    FCollisionQueryParams Params;
//    Params.AddIgnoredActor(OwnerCharacter.Get());
//
//    //@충돌 체크 수행
//    bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);
//
//    //@디버그 시각화
//    if (bEnableDebugDraw)
//    {
//        FColor LineColor = bHit ? FColor::Red : FColor::Green;
//        DrawDebugLine(MeshComp->GetWorld(), Start, End, LineColor, false, 0.1f, 0, 2.0f);
//
//        if (bHit)
//        {
//            DrawDebugSphere(MeshComp->GetWorld(), Hit.Location, 10.0f, 8, FColor::Orange, false, 0.1f);
//            UE_LOGFMT(LogANS_MW_FixPosition, Log, "전방 충돌 감지: {0}",
//                Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("Unknown"));
//        }
//    }
//
//    return bHit;
//}
//#pragma endregion

#include "ANS_MW_FixPositionWhenCollide.h"
#include "Logging/StructuredLog.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogANS_DisableRootMotion)

UANS_MW_FixPositionWhenCollide::UANS_MW_FixPositionWhenCollide(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsNativeBranchingPoint = true;
}

void UANS_MW_FixPositionWhenCollide::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
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

void UANS_MW_FixPositionWhenCollide::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
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

void UANS_MW_FixPositionWhenCollide::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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
bool UANS_MW_FixPositionWhenCollide::CanBePlaced(UAnimSequenceBase* Animation) const
{
    return (Animation && Animation->IsA(UAnimMontage::StaticClass()));
}
#endif

FString UANS_MW_FixPositionWhenCollide::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("충돌 시 루트 모션 비활성화"));
}

bool UANS_MW_FixPositionWhenCollide::DetectForwardCollision(USkeletalMeshComponent* MeshComp)
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