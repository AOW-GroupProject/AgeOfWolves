#include "ANS_DirectionalMotionWarping.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogANS_DirectionalMotionWarping)

//@Default Setting
#pragma region Default Setting
void UANS_DirectionalMotionWarping::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    //@MeshComp 유효성 체크
    if (!MeshComp)
    {
        UE_LOGFMT(LogANS_DirectionalMotionWarping, Warning, "NotifyBegin 실패 - MeshComp가 유효하지 않음");
        return;
    }

    //@Character 캐스팅
    auto Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!Character)
    {
        UE_LOGFMT(LogANS_DirectionalMotionWarping, Warning, "NotifyBegin 실패 - Character 캐스팅 실패. Owner: {0}",
            *GetNameSafe(MeshComp->GetOwner()));
        return;
    }

    //@MotionWarpingComponent 체크
    auto MotionWarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>();
    if (!MotionWarpingComp)
    {
        UE_LOGFMT(LogANS_DirectionalMotionWarping, Warning, "NotifyBegin 실패 - MotionWarpingComponent가 없음");
        return;
    }

    //@기존 타겟이 있다면 제거
    MotionWarpingComp->RemoveWarpTarget(FName("TargetLocation"));

    //@시작 위치와 회전 저장
    FVector StartLocation = Character->GetActorLocation();

    //@방향 벡터 계산
    FVector DirectionVector = CalculateDirectionVector(Character);

    //@목표 위치 계산
    FVector TargetLocation = StartLocation + (DirectionVector * MovementDistance);

    //@워프 타겟 설정 - 한 번만 설정하면 애니메이션 내내 적용됨
    FMotionWarpingTarget WarpTarget;
    WarpTarget.Name = FName("TargetLocation");
    WarpTarget.Location = TargetLocation;

    MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

    UE_LOGFMT(LogANS_DirectionalMotionWarping, Log, "방향성 워프 타겟 설정 - 방향: {0}, 거리: {1}, 위치: {2}",
        *UEnum::GetValueAsString(MovementDirection),
        MovementDistance,
        *TargetLocation.ToString());

    //@디버그 정보 표시
    if (bDrawDebug)
    {
        DrawDebugInfo(Character, StartLocation, TargetLocation, TotalDuration);
    }
}

void UANS_DirectionalMotionWarping::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);

    //@MeshComp 유효성 체크
    if (!MeshComp)
    {
        return;
    }

    //@Character 캐스팅
    auto Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!Character)
    {
        return;
    }

    //@MotionWarpingComponent 체크
    auto MotionWarpingComp = Character->FindComponentByClass<UMotionWarpingComponent>();
    if (!MotionWarpingComp)
    {
        return;
    }

    //@워프 타겟 제거
    MotionWarpingComp->RemoveWarpTarget(FName("TargetLocation"));

    UE_LOGFMT(LogANS_DirectionalMotionWarping, Log, "방향성 모션 워프 종료");
}
FString UANS_DirectionalMotionWarping::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("지정 방향으로 수치 값만큼 이동"));

}
#pragma endregion

//@Utility
#pragma region Utility
FVector UANS_DirectionalMotionWarping::CalculateDirectionVector(ACharacterBase* Character)
{
    if (!Character)
    {
        return FVector::ForwardVector;
    }

    FVector Direction;
    // 카메라 방향 대신 캐릭터의 회전을 사용
    FRotator ActorRotation = Character->GetActorRotation();

    // 방향에 따른 벡터 계산 - 캐릭터 기준
    switch (MovementDirection)
    {
    case EMovementDirection::Fwd:
        Direction = Character->GetActorForwardVector();
        break;
    case EMovementDirection::Bwd:
        Direction = -Character->GetActorForwardVector();
        break;
    case EMovementDirection::Left:
        Direction = -Character->GetActorRightVector();
        break;
    case EMovementDirection::Right:
        Direction = Character->GetActorRightVector();
        break;
    case EMovementDirection::FL:
        Direction = (Character->GetActorForwardVector() - Character->GetActorRightVector()).GetSafeNormal();
        break;
    case EMovementDirection::FR:
        Direction = (Character->GetActorForwardVector() + Character->GetActorRightVector()).GetSafeNormal();
        break;
    case EMovementDirection::BL:
        Direction = (-Character->GetActorForwardVector() - Character->GetActorRightVector()).GetSafeNormal();
        break;
    case EMovementDirection::BR:
        Direction = (-Character->GetActorForwardVector() + Character->GetActorRightVector()).GetSafeNormal();
        break;
    default:
        Direction = Character->GetActorForwardVector();
        break;
    }

    // Z 방향 제거 (평면 이동)
    Direction.Z = 0.0f;
    Direction = Direction.GetSafeNormal();

    return Direction;
}

FRotator UANS_DirectionalMotionWarping::CalculateTargetRotation(ACharacterBase* Character)
{
    if (!Character)
    {
        return FRotator::ZeroRotator;
    }

    FRotator ControlRotation = Character->GetControlRotation();

    // 방향에 따른 회전 계산
    float YawOffset = 0.0f;

    switch (MovementDirection)
    {
    case EMovementDirection::Fwd:
        YawOffset = 0.0f;
        break;
    case EMovementDirection::Bwd:
        YawOffset = 180.0f;
        break;
    case EMovementDirection::Left:
        YawOffset = -90.0f;
        break;
    case EMovementDirection::Right:
        YawOffset = 90.0f;
        break;
    case EMovementDirection::FL:
        YawOffset = -45.0f;
        break;
    case EMovementDirection::FR:
        YawOffset = 45.0f;
        break;
    case EMovementDirection::BL:
        YawOffset = -135.0f;
        break;
    case EMovementDirection::BR:
        YawOffset = 135.0f;
        break;
    default:
        YawOffset = 0.0f;
        break;
    }

    // Yaw 회전만 적용
    FRotator TargetRotation = FRotator(0.0f, ControlRotation.Yaw + YawOffset, 0.0f);

    return TargetRotation;
}

void UANS_DirectionalMotionWarping::DrawDebugInfo(ACharacterBase* Character, const FVector& StartLocation, const FVector& TargetLocation, float Duration)
{
    if (!Character || !Character->GetWorld())
    {
        return;
    }

    UWorld* World = Character->GetWorld();

    // 시작 위치에 녹색 구체 표시
    DrawDebugSphere(World, StartLocation, 20.0f, 12, FColor::Green, false, Duration);

    // 목표 위치에 빨간색 구체 표시
    DrawDebugSphere(World, TargetLocation, 20.0f, 12, FColor::Red, false, Duration);

    // 시작 위치와 목표 위치 사이에 선 표시
    DrawDebugLine(World, StartLocation, TargetLocation, FColor::Yellow, false, Duration);

    // 방향 텍스트 표시
    DrawDebugString(World, TargetLocation + FVector(0, 0, 50),
        UEnum::GetValueAsString(MovementDirection), nullptr, FColor::White, Duration);
}
#pragma endregion