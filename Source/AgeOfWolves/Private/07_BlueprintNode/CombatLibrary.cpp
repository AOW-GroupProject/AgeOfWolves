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

//FHitDirectionResult UCombatLibrary::CalculateHitDirectionWithHitResult(const AActor* Instigator, const FHitResult& HitResult)
//{
//    FHitDirectionResult Result;
//    Result.Direction = EHitReactDirection::Max;
//    Result.Height = EHitHeight::Max;
//    Result.ImpactLocation = EHitImpactLocation::Max;
//
//    // 유효성 검사
//    AActor* HitActor = HitResult.GetActor();
//    if (!IsValid(HitActor) || !IsValid(Instigator))
//    {
//        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 계산 실패 - 사유: HitActor 또는 Instigator가 유효하지 않음");
//        return Result;
//    }
//
//    ACharacter* Character = Cast<ACharacter>(HitActor);
//    if (!Character)
//    {
//        UE_LOGFMT(LogCombatLibrary, Warning, "히트 방향 계산 실패 - 사유: HitActor가 Character가 아님");
//        return Result;
//    }
//
//    // 1. 공격자 위치 기반 Direction 계산 - CalculateHitDirection 함수 재사용
//    Result.Direction = CalculateHitDirection(Instigator->GetActorLocation(), Character);
//
//    // 2. Impact Point 기반 타격 위치 계산
//    {
//        FVector HitLocation = HitResult.ImpactPoint;
//        FVector ActorLocation = Character->GetActorLocation();
//        FVector ForwardVector = Character->GetActorForwardVector();
//        FVector RightVector = Character->GetActorRightVector();
//
//        FVector ToCenterHit = (HitLocation - ActorLocation);
//        ToCenterHit.Z = 0;
//        ToCenterHit = ToCenterHit.GetSafeNormal();
//
//        //@(타격 - 중심)벡터와 Forward 벡터의 내적
//        float ForwardDot = FVector::DotProduct(ForwardVector, ToCenterHit);
//        //@(타격 - 중심)벡터와 Right 벡터의 내적
//        float RightDot = FVector::DotProduct(RightVector, ToCenterHit);
//
//        //@전방/후방은 더 엄격하게 (30도), 좌우는 기존대로 (45도)
//        const float ForwardThreshold = FMath::Cos(FMath::DegreesToRadians(30.f));  // 약 0.866 (엄격)
//        const float SideThreshold = FMath::Cos(FMath::DegreesToRadians(45.f));     // 약 0.707 (기존)
//
//        UE_LOGFMT(LogCombatLibrary, Log, "Impact Location 계산 - Forward Dot: {0}, Right Dot: {1}",
//            ForwardDot, RightDot);
//
//        if (ForwardDot >= ForwardThreshold)
//        {
//            Result.ImpactLocation = EHitImpactLocation::Front;
//            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 전방");
//        }
//        else if (-ForwardDot > SideThreshold)
//        {
//            Result.ImpactLocation = EHitImpactLocation::Back;
//            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 후방");
//        }
//        else if (RightDot >= SideThreshold)
//        {
//            Result.ImpactLocation = EHitImpactLocation::Right;
//            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 우측");
//        }
//        else if (-RightDot > SideThreshold)
//        {
//            Result.ImpactLocation = EHitImpactLocation::Left;
//            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 좌측");
//        }
//        else
//        {
//            Result.ImpactLocation = EHitImpactLocation::Center;
//            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 중앙");
//        }
//    }
//
//    // 3. 수직 높이 계산
//    {
//        UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
//        if (Capsule)
//        {
//            float CharacterHeight = Capsule->GetScaledCapsuleHalfHeight() * 2.0f;
//            // 캐릭터의 중심점(ActorLocation)을 기준으로 상대 높이 계산
//            float CharacterCenterZ = Character->GetActorLocation().Z;
//            float RelativeZ = HitResult.ImpactPoint.Z - CharacterCenterZ;
//
//            // -1.0 ~ 1.0 범위로 정규화 (-1이 발밑, 0이 중심, 1이 머리)
//            float HeightRatio = RelativeZ / Capsule->GetScaledCapsuleHalfHeight();
//
//            UE_LOGFMT(LogCombatLibrary, Log, "높이 계산 - 전체 높이: {0}, 캐릭터 중심: {1}, Impact 높이: {2}, 상대 높이: {3}, 비율: {4}",
//                CharacterHeight, CharacterCenterZ, HitResult.ImpactPoint.Z, RelativeZ, HeightRatio);
//
//            // 비율에 따른 높이 구분 (-1 ~ 1 범위 기준)
//            if (HeightRatio > 0.33f) // 상단: 0.33 ~ 1.0
//            {
//                Result.Height = EHitHeight::Upper;
//                UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 상단");
//            }
//            else if (HeightRatio > -0.33f) // 중단: -0.33 ~ 0.33
//            {
//                Result.Height = EHitHeight::Middle;
//                UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 중단");
//            }
//            else // 하단: -1.0 ~ -0.33
//            {
//                Result.Height = EHitHeight::Lower;
//                UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 하단");
//            }
//        }
//    }
//
//    // 최종 결과 로그
//    UE_LOGFMT(LogCombatLibrary, Log, "최종 히트 방향 결과: {0}", *Result.ToString());
//
//    return Result;
//}

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