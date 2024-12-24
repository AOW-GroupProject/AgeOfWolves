#include "CombatLibrary.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogCombatLibrary)

EHitReactDirection UCombatLibrary::CalculateHitDirection(const FVector& ImpactLocation, const AActor* HitActor)
{
    if (!IsValid(HitActor)) return EHitReactDirection::Max;

    FVector ActorLocation = HitActor->GetActorLocation();
    FVector ForwardVector = HitActor->GetActorForwardVector();
    FVector RightVector = HitActor->GetActorRightVector();

    FVector ToHit = (ImpactLocation - ActorLocation);
    ToHit.Z = 0;
    ToHit = ToHit.GetSafeNormal();

    float ForwardDot = FVector::DotProduct(ForwardVector, ToHit);
    float RightDot = FVector::DotProduct(RightVector, ToHit);
    const float Threshold = FMath::Cos(FMath::DegreesToRadians(45.f));

    if (ForwardDot >= Threshold && FMath::Abs(RightDot) < Threshold)
    {
        return EHitReactDirection::Forward;
    }
    else if (-ForwardDot > Threshold && FMath::Abs(RightDot) < Threshold)
    {
        return EHitReactDirection::Backward;
    }
    else if (RightDot >= Threshold && FMath::Abs(ForwardDot) < Threshold)
    {
        return EHitReactDirection::Right;
    }
    else if (-RightDot > Threshold && FMath::Abs(ForwardDot) < Threshold)
    {
        return EHitReactDirection::Left;
    }

    return EHitReactDirection::Max;
}