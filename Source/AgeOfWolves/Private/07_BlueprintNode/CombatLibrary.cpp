#include "CombatLibrary.h"
#include "Logging/StructuredLog.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

DEFINE_LOG_CATEGORY(LogCombatLibrary)

//@계산 관련...
#pragma region Calculation
EHitReactDirection UCombatLibrary::CalculateHitDirection(const FVector& ImpactLocation, const AActor* HitActor)
{
    if (!IsValid(HitActor))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 계산 실패 - 사유: HitActor가 유효하지 않음");
        return EHitReactDirection::Max;
    }

    FVector ActorLocation = HitActor->GetActorLocation();
    FVector ForwardVector = HitActor->GetActorForwardVector();
    FVector RightVector = HitActor->GetActorRightVector();

    FVector ToHit = (ImpactLocation - ActorLocation);
    ToHit.Z = 0;
    ToHit = ToHit.GetSafeNormal();

    float ForwardDot = FVector::DotProduct(ForwardVector, ToHit);
    float RightDot = FVector::DotProduct(RightVector, ToHit);

    // 방향 판정을 위한 임계값 설정
    const float ForwardThreshold = FMath::Cos(FMath::DegreesToRadians(30.f));  // 약 0.866 (더 엄격함)
    const float DiagonalThreshold = FMath::Cos(FMath::DegreesToRadians(60.f)); // 약 0.5 (대각선 판정용)
    const float BackwardThreshold = FMath::Cos(FMath::DegreesToRadians(45.f)); // Backward는 기존과 동일하게 유지
    const float SideThreshold = FMath::Cos(FMath::DegreesToRadians(45.f));     // 약 0.707 (좌/우 판정)

    UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 계산 시작 - Target: {0}", *HitActor->GetName());
    UE_LOGFMT(LogCombatLibrary, Log, "벡터 정보 - Impact: {0}, Actor: {1}, ToHit: {2}",
        *ImpactLocation.ToString(), *ActorLocation.ToString(), *ToHit.ToString());
    UE_LOGFMT(LogCombatLibrary, Log, "내적 결과 - Forward: {0}, Right: {1}, ForwardThreshold: {2}, BackwardThreshold: {3}",
        ForwardDot, RightDot, ForwardThreshold, BackwardThreshold);

    EHitReactDirection HitDirection;

    // 전방 (순수 전방 - 더 엄격한 각도)
    if (ForwardDot >= ForwardThreshold && FMath::Abs(RightDot) < DiagonalThreshold)
    {
        HitDirection = EHitReactDirection::Forward;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 전방");
    }
    // 전방 우측 (대각선 판정)
    else if (ForwardDot >= DiagonalThreshold && RightDot >= DiagonalThreshold)
    {
        HitDirection = EHitReactDirection::FR;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 전방 우측");
    }
    // 전방 좌측 (대각선 판정)
    else if (ForwardDot >= DiagonalThreshold && RightDot <= -DiagonalThreshold)
    {
        HitDirection = EHitReactDirection::FL;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 전방 좌측");
    }
    // 후방 (기존 임계값 유지)
    else if (-ForwardDot > BackwardThreshold && FMath::Abs(RightDot) < BackwardThreshold)
    {
        HitDirection = EHitReactDirection::Backward;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 후방");
    }
    // 우측
    else if (RightDot >= SideThreshold && FMath::Abs(ForwardDot) < DiagonalThreshold)
    {
        HitDirection = EHitReactDirection::Right;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 우측");
    }
    // 좌측
    else if (RightDot <= -SideThreshold && FMath::Abs(ForwardDot) < DiagonalThreshold)
    {
        HitDirection = EHitReactDirection::Left;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 좌측");
    }
    else
    {
        HitDirection = EHitReactDirection::Max;
        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 결정: 불확실 - 임계값을 초과하는 방향 없음");
    }

    return HitDirection;
}

FHitDirectionResult UCombatLibrary::CalculateHitDirectionWithHitResult(const AActor* Instigator, const FHitResult& HitResult)
{
    FHitDirectionResult Result;
    Result.Direction = EHitReactDirection::Max;
    Result.Height = EHitHeight::Max;
    Result.ImpactLocation = EHitImpactLocation::Max;

    // 유효성 검사
    AActor* HitActor = HitResult.GetActor();
    if (!IsValid(HitActor) || !IsValid(Instigator))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 계산 실패 - 사유: HitActor 또는 Instigator가 유효하지 않음");
        return Result;
    }

    ACharacter* Character = Cast<ACharacter>(HitActor);
    if (!Character)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 계산 실패 - 사유: HitActor가 Character가 아님");
        return Result;
    }

    // 1. 공격자 위치 기반 Direction 계산
    Result.Direction = CalculateHitDirection(Instigator->GetActorLocation(), Character);

    // 2. 충돌 지점 기반 타격 위치 계산
    Result.ImpactLocation = CalculateHitImpactLocation(HitResult.ImpactPoint, Character);

    // 3. 수직 높이 계산
    Result.Height = CalculateHitHeight(HitResult.ImpactPoint, Character);

    // 최종 결과 로그
    UE_LOGFMT(LogCombatLibrary, Log, "최종 히트 방향 결과: {0}", *Result.ToString());

    return Result;
}

EHitReactDirection UCombatLibrary::CalculateHitReactDirection(const FVector& InstigatorLocation, const AActor* HitActor)
{
    if (!IsValid(HitActor))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 계산 실패 - 사유: HitActor가 유효하지 않음");
        return EHitReactDirection::Max;
    }

    FVector ActorLocation = HitActor->GetActorLocation();
    FVector ForwardVector = HitActor->GetActorForwardVector();
    FVector RightVector = HitActor->GetActorRightVector();

    FVector ToHit = (InstigatorLocation - ActorLocation);
    ToHit.Z = 0;
    ToHit = ToHit.GetSafeNormal();

    float ForwardDot = FVector::DotProduct(ForwardVector, ToHit);
    float RightDot = FVector::DotProduct(RightVector, ToHit);

    // 방향 판정을 위한 임계값 설정
    const float ForwardThreshold = FMath::Cos(FMath::DegreesToRadians(30.f));  // 약 0.866 (더 엄격함)
    const float DiagonalThreshold = FMath::Cos(FMath::DegreesToRadians(60.f)); // 약 0.5 (대각선 판정용)
    const float BackwardThreshold = FMath::Cos(FMath::DegreesToRadians(45.f)); // Backward는 기존과 동일하게 유지
    const float SideThreshold = FMath::Cos(FMath::DegreesToRadians(45.f));     // 약 0.707 (좌/우 판정)

    UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 계산 - ForwardDot: {0}, RightDot: {1}",
        ForwardDot, RightDot);

    // 전방 (순수 전방 - 더 엄격한 각도)
    if (ForwardDot >= ForwardThreshold && FMath::Abs(RightDot) < DiagonalThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향: 전방");
        return EHitReactDirection::Forward;
    }
    // 전방 우측 (대각선 판정)
    else if (ForwardDot >= DiagonalThreshold && RightDot >= DiagonalThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향: 전방 우측");
        return EHitReactDirection::FR;
    }
    // 전방 좌측 (대각선 판정)
    else if (ForwardDot >= DiagonalThreshold && RightDot <= -DiagonalThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향: 전방 좌측");
        return EHitReactDirection::FL;
    }
    // 후방 (기존 임계값 유지)
    else if (-ForwardDot > BackwardThreshold && FMath::Abs(RightDot) < BackwardThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향: 후방");
        return EHitReactDirection::Backward;
    }
    // 우측
    else if (RightDot >= SideThreshold && FMath::Abs(ForwardDot) < DiagonalThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향: 우측");
        return EHitReactDirection::Right;
    }
    // 좌측
    else if (RightDot <= -SideThreshold && FMath::Abs(ForwardDot) < DiagonalThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향: 좌측");
        return EHitReactDirection::Left;
    }

    UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향: 불확실 - 임계값을 초과하는 방향 없음");
    return EHitReactDirection::Max;
}

EHitHeight UCombatLibrary::CalculateHitHeight(const FVector& ImpactPoint, const ACharacter* Character)
{
    if (!IsValid(Character))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "타격 높이 계산 실패 - 사유: Character가 유효하지 않음");
        return EHitHeight::Max;
    }

    UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
    if (!Capsule)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "타격 높이 계산 실패 - 사유: CapsuleComponent가 유효하지 않음");
        return EHitHeight::Max;
    }

    float CharacterHeight = Capsule->GetScaledCapsuleHalfHeight() * 2.0f;
    // 캐릭터의 중심점(ActorLocation)을 기준으로 상대 높이 계산
    float CharacterCenterZ = Character->GetActorLocation().Z;
    float RelativeZ = ImpactPoint.Z - CharacterCenterZ;

    // -1.0 ~ 1.0 범위로 정규화 (-1이 발밑, 0이 중심, 1이 머리)
    float HeightRatio = RelativeZ / Capsule->GetScaledCapsuleHalfHeight();

    UE_LOGFMT(LogCombatLibrary, Log, "높이 계산 - 전체 높이: {0}, 캐릭터 중심: {1}, Impact 높이: {2}, 상대 높이: {3}, 비율: {4}",
        CharacterHeight, CharacterCenterZ, ImpactPoint.Z, RelativeZ, HeightRatio);

    // 비율에 따른 높이 구분 (-1 ~ 1 범위 기준)
    if (HeightRatio > 0.33f) // 상단: 0.33 ~ 1.0
    {
        UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 상단");
        return EHitHeight::Upper;
    }
    else if (HeightRatio > -0.33f) // 중단: -0.33 ~ 0.33
    {
        UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 중단");
        return EHitHeight::Middle;
    }

    // 하단: -1.0 ~ -0.33
    UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 하단");
    return EHitHeight::Lower;
}

EHitImpactLocation UCombatLibrary::CalculateHitImpactLocation(const FVector& ImpactPoint, const AActor* HitActor)
{
    if (!IsValid(HitActor))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "충돌 위치 계산 실패 - 사유: HitActor가 유효하지 않음");
        return EHitImpactLocation::Max;
    }

    FVector ActorLocation = HitActor->GetActorLocation();
    FVector ForwardVector = HitActor->GetActorForwardVector();
    FVector RightVector = HitActor->GetActorRightVector();

    FVector ToCenterHit = (ImpactPoint - ActorLocation);
    ToCenterHit.Z = 0;
    ToCenterHit = ToCenterHit.GetSafeNormal();

    // (타격 - 중심)벡터와 Forward 벡터의 내적
    float ForwardDot = FVector::DotProduct(ForwardVector, ToCenterHit);
    // (타격 - 중심)벡터와 Right 벡터의 내적
    float RightDot = FVector::DotProduct(RightVector, ToCenterHit);

    // 전방/후방은 더 엄격하게 (30도), 좌우는 기존대로 (45도)
    const float ForwardThreshold = FMath::Cos(FMath::DegreesToRadians(30.f));  // 약 0.866 (엄격)
    const float SideThreshold = FMath::Cos(FMath::DegreesToRadians(45.f));     // 약 0.707 (기존)

    UE_LOGFMT(LogCombatLibrary, Log, "충돌 위치 계산 - Forward Dot: {0}, Right Dot: {1}",
        ForwardDot, RightDot);

    if (ForwardDot >= ForwardThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "충돌 위치: 전방");
        return EHitImpactLocation::Front;
    }
    else if (-ForwardDot > SideThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "충돌 위치: 후방");
        return EHitImpactLocation::Back;
    }
    else if (RightDot >= SideThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "충돌 위치: 우측");
        return EHitImpactLocation::Right;
    }
    else if (-RightDot > SideThreshold)
    {
        UE_LOGFMT(LogCombatLibrary, Log, "충돌 위치: 좌측");
        return EHitImpactLocation::Left;
    }

    UE_LOGFMT(LogCombatLibrary, Log, "충돌 위치: 중앙");
    return EHitImpactLocation::Center;
}

FSurfacePointResult UCombatLibrary::GetClosestSurfacePointAndNormalFromSocket(
    const AActor* TargetActor,
    FName SocketName,
    EHitImpactLocation DesiredDirection,
    EHitImpactLocation DesiredNormalDirection)
{
    // 기본 리턴값 초기화
    FSurfacePointResult Result;

    // 유효성 검사
    if (!IsValid(TargetActor))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GetClosestSurfacePointAndNormalFromSocket 실패 - 사유: TargetActor가 유효하지 않음");
        return Result;
    }

    // Character 캐스팅
    const ACharacter* Character = Cast<ACharacter>(TargetActor);
    if (!Character)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GetClosestSurfacePointAndNormalFromSocket 실패 - 사유: Character 캐스팅 실패");
        return Result;
    }

    // SkeletalMeshComponent 가져오기
    USkeletalMeshComponent* SkeletalMesh = Character->GetMesh();
    if (!SkeletalMesh)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GetClosestSurfacePointAndNormalFromSocket 실패 - 사유: SkeletalMesh가 유효하지 않음");
        return Result;
    }

    // Socket 위치 가져오기
    const FVector SocketLocation = SkeletalMesh->GetSocketLocation(SocketName);
    if (SocketLocation.IsZero())
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GetClosestSurfacePointAndNormalFromSocket 실패 - 사유: Socket({0})을 찾을 수 없음", *SocketName.ToString());
        return Result;
    }

    // 방향 벡터 계산
    FVector DirectionVector;

    // 원하는 방향에 따라 적절한 방향 벡터 설정
    switch (DesiredDirection)
    {
    case EHitImpactLocation::Front:
        DirectionVector = Character->GetActorForwardVector();
        break;
    case EHitImpactLocation::Back:
        DirectionVector = -Character->GetActorForwardVector();
        break;
    case EHitImpactLocation::Left:
        DirectionVector = -Character->GetActorRightVector();
        break;
    case EHitImpactLocation::Right:
        DirectionVector = Character->GetActorRightVector();
        break;
    case EHitImpactLocation::Center:
    default:
        // Center의 경우 소켓에서 캐릭터 중심을 향한 방향의 반대 방향 사용
        DirectionVector = (SocketLocation - Character->GetActorLocation()).GetSafeNormal();
        break;
    }

    // 방향 벡터에 적절한 거리 곱하기 (캐릭터 크기에 따라 조정 필요)
    const float SearchDistance = 70.0f;
    const FVector SearchPoint = SocketLocation + (DirectionVector * SearchDistance);

    // Physics Asset에서 가장 가까운 지점 찾기
    FClosestPointOnPhysicsAsset ClosestPointResult;
    SkeletalMesh->GetClosestPointOnPhysicsAsset(
        SearchPoint,
        ClosestPointResult,
        false
    );

    // 결과 검증
    if (ClosestPointResult.ClosestWorldPosition.IsZero())
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GetClosestSurfacePointAndNormalFromSocket - Physics Asset에서 가까운 지점을 찾지 못함. Socket: {0}", *SocketName.ToString());
        Result.SurfacePoint = SocketLocation;
        return Result;
    }

    // 표면 위치 설정
    Result.SurfacePoint = ClosestPointResult.ClosestWorldPosition;

    // 표면 노멀 설정 - Physics Asset의 노멀 또는 요청된 방향을 기준으로
    if (DesiredNormalDirection == DesiredDirection)
    {
        // Physics Asset에서 계산된 노멀 사용
        Result.SurfaceNormal = ClosestPointResult.Normal;
    }
    else
    {
        // 특별히 요청된 노멀 방향 설정
        switch (DesiredNormalDirection)
        {
        case EHitImpactLocation::Front:
            Result.SurfaceNormal = Character->GetActorForwardVector();
            break;
        case EHitImpactLocation::Back:
            Result.SurfaceNormal = -Character->GetActorForwardVector();
            break;
        case EHitImpactLocation::Left:
            Result.SurfaceNormal = -Character->GetActorRightVector();
            break;
        case EHitImpactLocation::Right:
            Result.SurfaceNormal = Character->GetActorRightVector();
            break;
        case EHitImpactLocation::Center:
            Result.SurfaceNormal = (Result.SurfacePoint - Character->GetActorLocation()).GetSafeNormal();
            break;
        default:
            Result.SurfaceNormal = ClosestPointResult.Normal;
            break;
        }
    }

    // 디버깅 로그
    UE_LOGFMT(LogCombatLibrary, Log, "GetClosestSurfacePointAndNormalFromSocket 성공 - Socket: {0}, Direction: {1}, NormalDirection: {2}",
        *SocketName.ToString(),
        *UEnum::GetValueAsString(DesiredDirection),
        *UEnum::GetValueAsString(DesiredNormalDirection)
    );
    UE_LOGFMT(LogCombatLibrary, Log, "결과 - 위치: {0}, 노멀: {1}",
        *Result.SurfacePoint.ToString(),
        *Result.SurfaceNormal.ToString()
    );

    //@디버그 시각화 (필요에 따라 활성화)
//#if ENABLE_DRAW_DEBUG
//    if (GEngine && TargetActor->GetWorld())
//    {
//        // 소켓 위치 (빨간색)
//        DrawDebugSphere(
//            TargetActor->GetWorld(),
//            SocketLocation,
//            5.0f,
//            12,
//            FColor::Red,
//            false,
//            3.0f,
//            0,
//            1.0f
//        );
//
//        // 검색 지점 (노란색)
//        DrawDebugSphere(
//            TargetActor->GetWorld(),
//            SearchPoint,
//            5.0f,
//            12,
//            FColor::Yellow,
//            false,
//            3.0f,
//            0,
//            1.0f
//        );
//
//        // 결과 지점 (초록색)
//        DrawDebugSphere(
//            TargetActor->GetWorld(),
//            Result.SurfacePoint,
//            5.0f,
//            12,
//            FColor::Green,
//            false,
//            3.0f,
//            0,
//            1.0f
//        );
//
//    //    // 방향 표시 (파란색)
//    //    DrawDebugDirectionalArrow(
//    //        TargetActor->GetWorld(),
//    //        SocketLocation,
//    //        SearchPoint,
//    //        10.0f,
//    //        FColor::Blue,
//    //        false,
//    //        3.0f,
//    //        0,
//    //        1.0f
//    //    );
//
//    //    // 계산된 노멀 벡터 표시 (하늘색)
//    //    DrawDebugDirectionalArrow(
//    //        TargetActor->GetWorld(),
//    //        Result.SurfacePoint,
//    //        Result.SurfacePoint + Result.SurfaceNormal * 30.0f,
//    //        10.0f,
//    //        FColor::Cyan,
//    //        false,
//    //        3.0f,
//    //        0,
//    //        1.0f
//    //    );
//
//    //    // Physics Asset의 원본 노멀 표시 (보라색)
//    //    if (DesiredNormalDirection != DesiredDirection)
//    //    {
//    //        DrawDebugDirectionalArrow(
//    //            TargetActor->GetWorld(),
//    //            Result.SurfacePoint,
//    //            Result.SurfacePoint + ClosestPointResult.Normal * 30.0f,
//    //            10.0f,
//    //            FColor::Purple,
//    //            false,
//    //            3.0f,
//    //            0,
//    //            1.0f
//    //        );
//    //    }
//    }
//#endif

    return Result;
}

bool UCombatLibrary::IsActorBackExposed(const AActor* ObserverActor, const AActor* TargetActor, float ExposureAngleThreshold)
{
    // 유효성 검사
    if (!IsValid(ObserverActor) || !IsValid(TargetActor))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "등 노출 확인 실패 - 사유: ObserverActor 또는 TargetActor가 유효하지 않음");
        return false;
    }

    // 위치 벡터 계산
    FVector ObserverLocation = ObserverActor->GetActorLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();

    // 높이(Z) 차이 무시를 위해 XY 평면에서만 계산
    ObserverLocation.Z = TargetLocation.Z;

    // 대상 -> 관찰자 방향 벡터
    FVector DirectionToObserver = (ObserverLocation - TargetLocation).GetSafeNormal();

    // 대상의 전방 벡터
    FVector TargetForward = TargetActor->GetActorForwardVector();
    TargetForward.Z = 0.0f;
    TargetForward.Normalize();

    // 대상의 오른쪽 벡터
    FVector TargetRight = TargetActor->GetActorRightVector();
    TargetRight.Z = 0.0f;
    TargetRight.Normalize();

    // 내적 계산 (전방 벡터와의 각도)
    float ForwardDot = FVector::DotProduct(TargetForward, DirectionToObserver);

    // 오른쪽 벡터와의 내적 계산 (왼쪽/오른쪽 방향 확인용)
    float RightDot = FVector::DotProduct(TargetRight, DirectionToObserver);

    // 각도 계산 (0-180도)
    float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(ForwardDot, -1.0f, 1.0f)));

    // 후면 노출 임계값 설정 (기본값 120도)
    float BackExposureAngle = (ExposureAngleThreshold > 0.0f) ? ExposureAngleThreshold : 160.f;

    // 각도가 임계값보다 크면 후면으로 간주
    bool bIsBackExposed = AngleDegrees > BackExposureAngle;

    // 방향 정보 (디버깅용)
    FString Direction = RightDot > 0 ? TEXT("오른쪽") : TEXT("왼쪽");
    if (ForwardDot > 0.7) Direction = TEXT("전방");
    else if (ForwardDot < -0.7) Direction = TEXT("후방");
    else Direction = Direction + (ForwardDot > 0 ? TEXT(" 전방") : TEXT(" 후방"));

    // 추가 로깅
    UE_LOGFMT(LogCombatLibrary, Log,
        "등 노출 계산 - 대상: {0}, 관찰자: {1}, 각도: {2}°, 방향: {3}, 임계값: {4}°, 결과: {5}",
        *TargetActor->GetName(),
        *ObserverActor->GetName(),
        AngleDegrees,
        *Direction,
        BackExposureAngle,
        bIsBackExposed ? TEXT("노출됨") : TEXT("노출되지 않음"));

    return bIsBackExposed;
}
#pragma endregion

//@이벤트 전달 관련...
#pragma region Send Event
bool UCombatLibrary::SendGameplayEventToTarget(
    FGameplayTag EventTag,
    AActor* TargetActor,
    AActor* InstigatorActor,
    const FHitResult& HitResult,
    float Magnitude,
    UObject* OptionalObject,
    UObject* OptionalObject2)
{
    //@Target
    if (!IsValid(TargetActor))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GameplayEvent 전송 실패 - 사유: TargetActor가 유효하지 않음");
        return false;
    }

    //@Target ASC 가져오기
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (!TargetASC)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GameplayEvent 전송 실패 - Target: {0}, 사유: 타겟의 ASC가 유효하지 않음",
            *TargetActor->GetName());
        return false;
    }

    //@Instigator ASC 가져오기 (Context 생성용)
    UAbilitySystemComponent* InstigatorASC = nullptr;
    if (IsValid(InstigatorActor))
    {
        InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);
    }

    //@Context Handle 생성
    FGameplayEffectContextHandle ContextHandle;
    if (InstigatorASC)
    {
        ContextHandle = InstigatorASC->MakeEffectContext();
    }
    else
    {
        ContextHandle = TargetASC->MakeEffectContext();
    }

    //@HitResult 추가
    ContextHandle.AddHitResult(HitResult);

    //@Event Data 구성
    FGameplayEventData Payload;
    Payload.EventTag = EventTag;
    Payload.Target = TargetActor;
    Payload.Instigator = InstigatorActor;
    Payload.OptionalObject = OptionalObject;
    Payload.OptionalObject2 = OptionalObject2;
    Payload.ContextHandle = ContextHandle;
    Payload.EventMagnitude = Magnitude;

    //@Event 전송
    if (!TargetASC->HandleGameplayEvent(EventTag, &Payload))
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "GameplayEvent 전송 실패 - Target: {0}, Event: {1}, 사유: HandleGameplayEvent 호출 실패",
            *TargetActor->GetName(), *EventTag.ToString());
        return false;
    }

    UE_LOGFMT(LogCombatLibrary, Log, "GameplayEvent 전송 완료 - Target: {0}, Event: {1}, Instigator: {2}",
        *TargetActor->GetName(),
        *EventTag.ToString(),
        IsValid(InstigatorActor) ? *InstigatorActor->GetName() : TEXT("None"));

    return true;
}
#pragma endregion

//@이팩트 관련...
#pragma region Gameplay Cue
FSlashGameplayCueParams UCombatLibrary::PrepareSlashGameplayCueParameters(AActor* InActor, const FHitResult& HitResult)
{
    FSlashGameplayCueParams Params;

    // 유효성 검사
    if (!InActor)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "PrepareSlashGameplayCueParameters 실패 - 사유: Actor가 유효하지 않음");
        return Params;
    }

    // 캐릭터 캐스팅
    ACharacter* Character = Cast<ACharacter>(InActor);
    if (!Character)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "PrepareSlashGameplayCueParameters 실패 - 사유: Character 캐스팅 실패");
        return Params;
    }

    // 스켈레탈 메시 컴포넌트 가져오기
    USkeletalMeshComponent* SkeletalMesh = Character->GetMesh();
    if (!SkeletalMesh)
    {
        UE_LOGFMT(LogCombatLibrary, Warning, "PrepareSlashGameplayCueParameters 실패 - 사유: SkeletalMesh가 유효하지 않음");
        return Params;
    }

    // 충돌 지점 정보 추출
    const FVector ImpactPoint = HitResult.ImpactPoint;

    // 스켈레탈 메시 표면의 가장 가까운 지점 찾기
    FClosestPointOnPhysicsAsset ClosestPointResult;
    SkeletalMesh->GetClosestPointOnPhysicsAsset(
        ImpactPoint,
        ClosestPointResult,
        false
    );

    // ClosestPoint와 Normal 정보 추출
    const FVector ClosestPoint = ClosestPointResult.ClosestWorldPosition;
    const FVector SurfaceNormal = ClosestPointResult.Normal;

    // 회전 계산
    FRotator SlashRotation = SurfaceNormal.Rotation();

    // 디버깅을 위한 시각화
//#if ENABLE_DRAW_DEBUG
//    // ClosestPoint 표시 (녹색)
//    DrawDebugSphere(
//        Character->GetWorld(),
//        ClosestPoint,
//        5.0f,
//        12,
//        FColor::Green,
//        false,
//        3.0f,
//        0,
//        1.0f
//    );
//
//    // 원래 ImpactPoint 표시 (빨간색)
//    DrawDebugSphere(
//        Character->GetWorld(),
//        ImpactPoint,
//        5.0f,
//        12,
//        FColor::Red,
//        false,
//        3.0f,
//        0,
//        1.0f
//    );
//
//    // ClosestPointResult의 Normal 방향 표시 (파란색)
//    DrawDebugDirectionalArrow(
//        Character->GetWorld(),
//        ClosestPoint,
//        ClosestPoint + SurfaceNormal * 30.0f,
//        10.0f,
//        FColor::Blue,
//        false,
//        3.0f,
//        0,
//        1.0f
//    );
//#endif

    // 결과값 설정
    Params.SlashRotation = SlashRotation;
    Params.SpawnLocation = ClosestPoint;
    Params.ImpactNormal = SurfaceNormal;

    UE_LOGFMT(LogCombatLibrary, Log, "슬래시 GameplayCue 파라미터 준비 완료 - Target: {0}, Location: {1}, Rotation: {2}, Normal: {3}",
        *InActor->GetName(),
        *ClosestPoint.ToString(),
        *SlashRotation.ToString(),
        *SurfaceNormal.ToString()
    );

    return Params;
}
#pragma endregion