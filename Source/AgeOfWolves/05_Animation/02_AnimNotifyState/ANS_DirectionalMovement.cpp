#include "ANS_DirectionalMovement.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogANS_DirectionalMovement)

//@Default Setting
#pragma region Default Setting
void UANS_DirectionalMovement::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    //@MeshComp 유효성 체크
    if (!MeshComp)
    {
        UE_LOGFMT(LogANS_DirectionalMovement, Warning, "NotifyBegin 실패 - MeshComp가 유효하지 않음");
        return;
    }

    //@Character 캐스팅
    auto Character = Cast<ACharacterBase>(MeshComp->GetOwner());
    if (!Character)
    {
        UE_LOGFMT(LogANS_DirectionalMovement, Warning, "NotifyBegin 실패 - Character 캐스팅 실패. Owner: {0}",
            *GetNameSafe(MeshComp->GetOwner()));
        return;
    }

    //@시작 위치 저장
    StartLocation = Character->GetActorLocation();

    //@방향 벡터 계산
    FVector DirectionVector = CalculateDirectionVector(Character);

    //@목표 위치 계산
    TargetLocation = StartLocation + (DirectionVector * MovementDistance);

    //@이동 완료 플래그 초기화
    bMovementCompleted = false;

    UE_LOGFMT(LogANS_DirectionalMovement, Log, "방향 이동 시작 - 방향: {0}, 거리: {1}, 속도: {2}, 시작: {3}, 목표: {4}",
        *UEnum::GetValueAsString(MovementDirection),
        MovementDistance,
        MovementSpeed,
        *StartLocation.ToString(),
        *TargetLocation.ToString());

    //@디버그 정보 표시
    if (bDrawDebug)
    {
        DrawDebugInfo(Character, StartLocation, TotalDuration);
    }
}

void UANS_DirectionalMovement::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

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

    //@이동이 완료되었다면 스킵
    if (bMovementCompleted)
    {
        return;
    }

    //@현재 위치
    FVector CurrentLocation = Character->GetActorLocation();

    //@충돌 감지 (MovementDirection 방향으로)
    if (DetectCollisionInDirection(Character))
    {
        bMovementCompleted = true;
        UE_LOGFMT(LogANS_DirectionalMovement, Log, "이동 방향 충돌 감지로 이동 중단 - 현재 위치: {0}",
            *CurrentLocation.ToString());
        return;
    }

    //@VInterpTo를 사용한 부드러운 이동
    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        CurrentLocation,
        TargetLocation,
        FrameDeltaTime,
        MovementSpeed
    );

    //@Z축 고정 (평면 이동)
    NewLocation.Z = CurrentLocation.Z;

    //@새 위치 적용
    Character->SetActorLocation(NewLocation);

    //@디버그 정보 표시
    if (bDrawDebug)
    {
        DrawDebugInfo(Character, NewLocation, FrameDeltaTime);
    }
}

void UANS_DirectionalMovement::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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

    //@이동 완료 여부 로그
    if (!bMovementCompleted)
    {
        UE_LOGFMT(LogANS_DirectionalMovement, Log, "방향 이동 미완료 상태로 종료 - 최종 위치: {0}",
            *Character->GetActorLocation().ToString());
    }

    UE_LOGFMT(LogANS_DirectionalMovement, Log, "방향 이동 ANS 종료");
}

FString UANS_DirectionalMovement::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("방향 설정 부드러운 이동 (%s, %.0fcm, 속도%.1f)"),
        *UEnum::GetValueAsString(MovementDirection),
        MovementDistance,
        MovementSpeed);
}
#pragma endregion

//@Utility
#pragma region Utility
FVector UANS_DirectionalMovement::CalculateDirectionVector(ACharacterBase* Character)
{
    if (!Character)
    {
        UE_LOGFMT(LogANS_DirectionalMovement, Warning, "방향 벡터 계산 실패 - Character가 유효하지 않음");
        return FVector::ForwardVector;
    }

    FVector Direction;

    //@방향에 따른 벡터 계산 - 캐릭터 기준
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
        UE_LOGFMT(LogANS_DirectionalMovement, Warning, "알 수 없는 이동 방향, 전방으로 설정");
        break;
    }

    //@Z 방향 제거 (평면 이동)
    Direction.Z = 0.0f;
    Direction = Direction.GetSafeNormal();

    return Direction;
}

bool UANS_DirectionalMovement::DetectCollisionInDirection(ACharacterBase* Character)
{
    //@캐릭터 유효성 검사
    if (!Character || !Character->GetWorld())
    {
        return false;
    }

    //@캐릭터 위치 및 이동 방향
    FVector Start = Character->GetActorLocation();

    //@캐릭터 눈높이 정도에서 시작
    float HalfHeight = Character->GetDefaultHalfHeight();
    Start.Z += HalfHeight * 0.5f;

    //@MovementDirection에 따른 충돌 감지 방향 계산
    FVector DirectionVector = CalculateDirectionVector(Character);
    FVector End = Start + DirectionVector * CollisionCheckDistance;

    //@충돌 설정
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    //@충돌 체크 수행
    bool bHit = Character->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);

    //@디버그 시각화
    if (bDrawDebug)
    {
        FColor LineColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugLine(Character->GetWorld(), Start, End, LineColor, false, 0.1f, 0, 2.0f);

        if (bHit)
        {
            DrawDebugSphere(Character->GetWorld(), Hit.Location, 10.0f, 8, FColor::Orange, false, 0.1f);
            UE_LOGFMT(LogANS_DirectionalMovement, Log, "이동 방향({0}) 충돌 감지: {1}",
                *UEnum::GetValueAsString(MovementDirection),
                Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("Unknown"));
        }
    }

    return bHit;
}

void UANS_DirectionalMovement::DrawDebugInfo(ACharacterBase* Character, const FVector& CurrentLocation, float Duration)
{
    if (!Character || !Character->GetWorld())
    {
        return;
    }

    UWorld* World = Character->GetWorld();

    //@시작 위치에 녹색 구체 표시
    DrawDebugSphere(World, StartLocation, 15.0f, 12, FColor::Green, false, Duration);

    //@목표 위치에 빨간색 구체 표시
    DrawDebugSphere(World, TargetLocation, 15.0f, 12, FColor::Red, false, Duration);

    //@현재 위치에 파란색 구체 표시
    DrawDebugSphere(World, CurrentLocation, 10.0f, 12, FColor::Blue, false, Duration);

    //@시작 위치와 목표 위치 사이에 선 표시
    DrawDebugLine(World, StartLocation, TargetLocation, FColor::Yellow, false, Duration);

    //@현재 위치에서 목표 위치로 화살표 표시
    DrawDebugDirectionalArrow(World, CurrentLocation, TargetLocation, 20.0f, FColor::Cyan, false, Duration);

    //@방향 텍스트 표시
    FString DirectionText = FString::Printf(TEXT("%s (%.0fcm, 속도%.1f)"),
        *UEnum::GetValueAsString(MovementDirection),
        MovementDistance,
        MovementSpeed);
    DrawDebugString(World, TargetLocation + FVector(0, 0, 50), DirectionText, nullptr, FColor::White, Duration);

    //@진행률 표시
    float Progress = 1.0f - (FVector::Dist(CurrentLocation, TargetLocation) / FVector::Dist(StartLocation, TargetLocation));
    Progress = FMath::Clamp(Progress, 0.0f, 1.0f);
    FString ProgressText = FString::Printf(TEXT("진행률: %.1f%%"), Progress * 100.0f);
    DrawDebugString(World, CurrentLocation + FVector(0, 0, 70), ProgressText, nullptr, FColor::Orange, Duration);
}
#pragma endregion