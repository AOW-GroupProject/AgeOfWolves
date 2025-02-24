#include "GCS_Slash.h"

#include "Kismet/GameplayStatics.h"

UGCS_Slash::UGCS_Slash()
{
    IsOverride = true; // GC 이벤트를 오버라이드
    EffectScale = 1.0f;
}

void UGCS_Slash::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
    const FGameplayCueParameters& Parameters)
{
    if (EventType != EGameplayCueEvent::Executed)
    {
        return;
    }

    if (!IsValid(MyTarget))
    {
        return;
    }

    // Parameters에서 회전값 추출 (예: RawMagnitude를 Yaw로 사용)
    FRotator SpawnRotation = DefaultRotation;
    SpawnRotation.Yaw = Parameters.RawMagnitude;

    // 월드 가져오기
    UWorld* World = MyTarget->GetWorld();
    if (!World)
    {
        return;
    }

    // 이펙트 스폰 위치 계산
    FVector SpawnLocation = MyTarget->GetActorLocation();

    // 파티클 시스템 스폰
    if (SlashEffect)
    {
        UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(
            World,
            SlashEffect,
            SpawnLocation,
            SpawnRotation,
            FVector(EffectScale),
            true
        );
    }

    // 사운드 재생
    if (SlashSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            World,
            SlashSound,
            SpawnLocation,
            SpawnRotation
        );
    }
}