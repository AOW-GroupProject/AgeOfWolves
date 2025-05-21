#include "ANS_MW_FixPositionWhenCollide.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "MotionWarpingComponent.h"

DEFINE_LOG_CATEGORY(LogANS_MW_FixPosition)

//@Defualt Setting
#pragma region Default Setting
void UANS_MW_FixPositionWhenCollide::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    //@소유 캐릭터 찾기
    if (!MeshComp || !MeshComp->GetOwner())
        return;

    OwnerCharacter = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!OwnerCharacter.IsValid())
        return;

    //@Motion Warping 컴포넌트 가져오기
    MotionWarpingComp = OwnerCharacter->FindComponentByClass<UMotionWarpingComponent>();
    if (!MotionWarpingComp.IsValid())
        return;

    //@초기화 - 충돌 감지 플래그 리셋
    bIsBlocked = false;

    //@초기 안전 위치 저장
    LastSafePosition = OwnerCharacter->GetActorLocation();

    if (bEnableDebugDraw)
    {
        UE_LOGFMT(LogANS_MW_FixPosition, Log, "전방 충돌 감지 시작: {0}",
            *Animation->GetName());
    }
}

void UANS_MW_FixPositionWhenCollide::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    //@필수 객체 유효성 검사
    if (!OwnerCharacter.IsValid() || !MotionWarpingComp.IsValid())
        return;

    //@현재 프레임의 충돌 상태 확인
    bool bCurrentlyBlocked = DetectForwardCollision(MeshComp);

    //@충돌 상태 변화 감지
    if (bCurrentlyBlocked != bIsBlocked)
    {
        //@상태 전환
        bIsBlocked = bCurrentlyBlocked;

        if (bIsBlocked)
        {
            //@충돌 감지 시 Motion Warping 타겟 설정
            FMotionWarpingTarget WarpTarget;
            WarpTarget.Name = WarpTargetName;
            WarpTarget.Location = LastSafePosition;
            WarpTarget.Rotation = OwnerCharacter->GetActorRotation();

            MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

            if (bEnableDebugDraw)
            {
                UE_LOGFMT(LogANS_MW_FixPosition, Log, "전방 충돌 감지: 전방 이동 제한 시작");
                DrawDebugSphere(MeshComp->GetWorld(), LastSafePosition, 20.0f, 8, FColor::Red, false, 0.5f);
            }
        }
        else
        {
            //@충돌 해제 시 Warping 타겟 제거
            MotionWarpingComp->RemoveWarpTarget(WarpTargetName);

            if (bEnableDebugDraw)
            {
                UE_LOGFMT(LogANS_MW_FixPosition, Log, "충돌 해제 감지: 전방 이동 제한 해제");
                DrawDebugSphere(MeshComp->GetWorld(), OwnerCharacter->GetActorLocation(), 20.0f, 8, FColor::Green, false, 0.5f);
            }
        }
    }

    //@안전 위치 업데이트 (충돌이 없을 때만)
    if (!bIsBlocked)
    {
        LastSafePosition = OwnerCharacter->GetActorLocation();
    }
    else
    {
        //@충돌 상태에서 전방 이동 성분 제거
        FVector CurrentLocation = OwnerCharacter->GetActorLocation();
        FVector Forward = OwnerCharacter->GetActorForwardVector();

        //@전방 이동 성분 추출
        float ForwardDelta = FVector::DotProduct(CurrentLocation - LastSafePosition, Forward);

        //@전방으로 이동했으면 위치 보정
        if (ForwardDelta > 0.5f) // 작은 값은 무시하여 미세한 움직임 허용
        {
            //@전방 이동분만 제거한 새 위치 계산
            FVector AdjustedPosition = CurrentLocation - (Forward * ForwardDelta);
            OwnerCharacter->SetActorLocation(AdjustedPosition, false);

            if (bEnableDebugDraw)
            {
                UE_LOGFMT(LogANS_MW_FixPosition, Log, "위치 보정: 전방 이동 {0} 제거", ForwardDelta);
            }
        }
    }
}

void UANS_MW_FixPositionWhenCollide::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@종료 시 Warping 타겟 제거
    if (MotionWarpingComp.IsValid())
    {
        MotionWarpingComp->RemoveWarpTarget(WarpTargetName);

        if (bEnableDebugDraw)
        {
            UE_LOGFMT(LogANS_MW_FixPosition, Log, "노티파이 종료: Warping 타겟 제거됨");
        }
    }

    //@충돌 상태 초기화
    bIsBlocked = false;
}

FString UANS_MW_FixPositionWhenCollide::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("표면 미끄러짐 이동 방지"));
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
            UE_LOGFMT(LogANS_MW_FixPosition, Log, "전방 충돌 감지: {0}",
                Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("Unknown"));
        }
    }

    return bHit;
}
#pragma endregion