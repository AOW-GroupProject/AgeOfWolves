#include "CombatLibrary.h"
#include "Logging/StructuredLog.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogCombatLibrary)

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
    const float Threshold = FMath::Cos(FMath::DegreesToRadians(45.f));

    UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 계산 시작 - Target: {0}", *HitActor->GetName());
    UE_LOGFMT(LogCombatLibrary, Log, "벡터 정보 - Impact: {0}, Actor: {1}, ToHit: {2}",
        *ImpactLocation.ToString(), *ActorLocation.ToString(), *ToHit.ToString());
    UE_LOGFMT(LogCombatLibrary, Log, "내적 결과 - Forward: {0}, Right: {1}, Threshold: {2}",
        ForwardDot, RightDot, Threshold);

    EHitReactDirection HitDirection;
    if (ForwardDot >= Threshold && FMath::Abs(RightDot) < Threshold)
    {
        HitDirection = EHitReactDirection::Forward;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 전방");
    }
    else if (-ForwardDot > Threshold && FMath::Abs(RightDot) < Threshold)
    {
        HitDirection = EHitReactDirection::Backward;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 후방");
    }
    else if (RightDot >= Threshold && FMath::Abs(ForwardDot) < Threshold)
    {
        HitDirection = EHitReactDirection::Right;
        UE_LOGFMT(LogCombatLibrary, Log, "히트 방향 결정: 우측");
    }
    else if (-RightDot > Threshold && FMath::Abs(ForwardDot) < Threshold)
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

    // 공통으로 사용할 변수들
    FVector ActorLocation = Character->GetActorLocation();
    FVector ForwardVector = Character->GetActorForwardVector();
    FVector RightVector = Character->GetActorRightVector();
    const float Threshold = FMath::Cos(FMath::DegreesToRadians(45.f));

    // 1. 공격자 위치 기반 Direction 계산
    {
        FVector InstigatorLocation = Instigator->GetActorLocation();
        FVector ToInstigator = (InstigatorLocation - ActorLocation);
        ToInstigator.Z = 0;
        ToInstigator = ToInstigator.GetSafeNormal();

        float ForwardDot = FVector::DotProduct(ForwardVector, ToInstigator);
        float RightDot = FVector::DotProduct(RightVector, ToInstigator);

        UE_LOGFMT(LogCombatLibrary, Log, "공격자 방향 계산 - Target: {0}, Instigator: {1}",
            *Character->GetName(), *Instigator->GetName());
        UE_LOGFMT(LogCombatLibrary, Log, "공격자 방향 내적 - Forward: {0}, Right: {1}, Threshold: {2}",
            ForwardDot, RightDot, Threshold);

        if (ForwardDot >= Threshold && FMath::Abs(RightDot) < Threshold)
        {
            Result.Direction = EHitReactDirection::Forward;
            UE_LOGFMT(LogCombatLibrary, Log, "공격자 위치: 전방");
        }
        else if (-ForwardDot > Threshold && FMath::Abs(RightDot) < Threshold)
        {
            Result.Direction = EHitReactDirection::Backward;
            UE_LOGFMT(LogCombatLibrary, Log, "공격자 위치: 후방");
        }
        else if (RightDot >= Threshold && FMath::Abs(ForwardDot) < Threshold)
        {
            Result.Direction = EHitReactDirection::Right;
            UE_LOGFMT(LogCombatLibrary, Log, "공격자 위치: 우측");
        }
        else if (-RightDot > Threshold && FMath::Abs(ForwardDot) < Threshold)
        {
            Result.Direction = EHitReactDirection::Left;
            UE_LOGFMT(LogCombatLibrary, Log, "공격자 위치: 좌측");
        }
    }

    // 2. Impact Point 기반 타격 위치 계산
    {
        FVector HitLocation = HitResult.ImpactPoint;

        //@Debug Sphere 그리기
#if ENABLE_DRAW_DEBUG
        DrawDebugSphere(
            Character->GetWorld(),
            HitLocation,
            10.0f,
            12,
            FColor::Red,
            false,
            3.0f,
            0,
            2.0f
        );
#endif

        FVector ToCenterHit = (HitLocation - ActorLocation);
        ToCenterHit.Z = 0;
        ToCenterHit = ToCenterHit.GetSafeNormal();

        //@(타격 - 중심)벡터와 Forward 벡터의 내적
        float ForwardDot = FVector::DotProduct(ForwardVector, ToCenterHit);
        //@(타격 - 중심)벡터와 Right 벡터의 내적
        float RightDot = FVector::DotProduct(RightVector, ToCenterHit);

        //@전방/후방은 더 엄격하게 (30도), 좌우는 기존대로 (45도)
        const float ForwardThreshold = FMath::Cos(FMath::DegreesToRadians(30.f));  // 약 0.866 (엄격)
        const float SideThreshold = FMath::Cos(FMath::DegreesToRadians(45.f));     // 약 0.707 (기존)

        UE_LOGFMT(LogCombatLibrary, Log, "Impact Location 계산 - Forward Dot: {0}, Right Dot: {1}",
            ForwardDot, RightDot);

        if (ForwardDot >= ForwardThreshold)
        {
            Result.ImpactLocation = EHitImpactLocation::Front;
            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 전방");
        }
        else if (-ForwardDot > ForwardThreshold)
        {
            Result.ImpactLocation = EHitImpactLocation::Back;
            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 후방");
        }
        else if (RightDot >= SideThreshold)
        {
            Result.ImpactLocation = EHitImpactLocation::Right;
            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 우측");
        }
        else if (-RightDot > SideThreshold)
        {
            Result.ImpactLocation = EHitImpactLocation::Left;
            UE_LOGFMT(LogCombatLibrary, Log, "타격 위치: 좌측");
        }
    }

    // 3. 수직 높이 계산
    {
        UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
        if (Capsule)
        {
            float CharacterHeight = Capsule->GetScaledCapsuleHalfHeight() * 2.0f;
            // 캐릭터의 중심점(ActorLocation)을 기준으로 상대 높이 계산
            float CharacterCenterZ = ActorLocation.Z;
            float RelativeZ = HitResult.ImpactPoint.Z - CharacterCenterZ;

            // -1.0 ~ 1.0 범위로 정규화 (-1이 발밑, 0이 중심, 1이 머리)
            float HeightRatio = RelativeZ / Capsule->GetScaledCapsuleHalfHeight();

            UE_LOGFMT(LogCombatLibrary, Log, "높이 계산 - 전체 높이: {0}, 캐릭터 중심: {1}, Impact 높이: {2}, 상대 높이: {3}, 비율: {4}",
                CharacterHeight, CharacterCenterZ, HitResult.ImpactPoint.Z, RelativeZ, HeightRatio);

            // 비율에 따른 높이 구분 (-1 ~ 1 범위 기준)
            if (HeightRatio > 0.33f) // 상단: 0.33 ~ 1.0
            {
                Result.Height = EHitHeight::Upper;
                UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 상단");
            }
            else if (HeightRatio > -0.33f) // 중단: -0.33 ~ 0.33
            {
                Result.Height = EHitHeight::Middle;
                UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 중단");
            }
            else // 하단: -1.0 ~ -0.33
            {
                Result.Height = EHitHeight::Lower;
                UE_LOGFMT(LogCombatLibrary, Log, "타격 높이: 하단");
            }
        }
    }

    // 최종 결과 로그
    UE_LOGFMT(LogCombatLibrary, Log, "최종 히트 방향 결과: {0}", *Result.ToString());

    return Result;
}

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