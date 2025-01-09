// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(LogAttackGA)

//@Defualt Setting
#pragma region Default Setting
UAttackGameplayAbility::UAttackGameplayAbility(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAttackGameplayAbility::SendDamageEvent(const FHitResult& HitResult)
{
    //@Hit Actor
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: HitActor가 유효하지 않음");
        return;
    }

    //@Source Actor
    AActor* SourceActor = GetAvatarActorFromActorInfo();

    //@Source ASC
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceActor || !SourceASC)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: Source Actor 또는 ASC가 유효하지 않음");
        return;
    }

    //@Event Data
    FGameplayEventData Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag("EventTag.OnDamaged");
    Payload.Instigator = SourceActor;
    Payload.Target = HitActor;
    Payload.OptionalObject = GetApplyGameplayEffectClass().GetDefaultObject();

    //@Context Handle
    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.AddHitResult(HitResult);
    Payload.ContextHandle = EffectContext;

    //@Target ASC
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
    if (!TargetASC)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - Target: {0}, 사유: 타겟의 ASC가 유효하지 않음",
            HitActor->GetName());
        return;
    }

    //@Handle Gameplay Event
    if (!TargetASC->HandleGameplayEvent(Payload.EventTag, &Payload))
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - Target: {0}, 사유: HandleGameplayEvent 호출 실패",
            HitActor->GetName());
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "데미지 이벤트 전송 완료 - Target: {0}, Instigator: {1}, Impact Location: {2}",
        HitActor->GetName(), SourceActor->GetName(), HitResult.ImpactPoint.ToString());
}

void UAttackGameplayAbility::StartWeaponTrace()
{
    bIsTracing = true;

    // ActorsToIgnore 초기화
    ActorsToIgnore.Empty();


    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 시작 - 소켓 정보 [시작: {0}, 끝: {1}]",
        WeaponTraceStartSocket.ToString(), WeaponTraceEndSocket.ToString());
}

void UAttackGameplayAbility::ProcessWeaponTrace()
{
    if (!bIsTracing)
    {
        UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 처리 중단 - 사유: 트레이스가 활성화되지 않음");
        return;
    }

    ACharacterBase* Character = GetCharacterFromActorInfo();
    if (!Character || !Character->GetMesh())
    {
        UE_LOGFMT(LogAttackGA, Warning, "무기 트레이스 실패 - 사유: 캐릭터 또는 메시가 유효하지 않음");
        return;
    }

    // 소켓 위치 가져오기
    FVector StartLocation = Character->GetMesh()->GetSocketLocation(WeaponTraceStartSocket);
    FVector EndLocation = Character->GetMesh()->GetSocketLocation(WeaponTraceEndSocket);

    UE_LOGFMT(LogAttackGA, Log, "트레이스 위치 정보 - 시작: {0}, 끝: {1}, 반경: {2}",
        StartLocation.ToString(), EndLocation.ToString(), TraceRadius);

    // 트레이스 파라미터 설정
    FCollisionQueryParams QueryParams;
    for (AActor* ActorToIgnore : ActorsToIgnore)
    {
        if (!IsValid(ActorToIgnore)) continue;
        QueryParams.AddIgnoredActor(ActorToIgnore);
    }

    // 스피어 트레이스 수행
    TArray<FHitResult> HitResults;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(TraceRadius);

    bool bHit = GetWorld()->LineTraceMultiByChannel(
        HitResults,
        StartLocation,
        EndLocation,
        ECC_Visibility
    );

    // 디버그 드로우
#if ENABLE_DRAW_DEBUG
    //DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.0f, 0, 2.0f);
#endif

    if (!bHit)
    {
        UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 실행 완료 - 히트 없음");
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 히트 발생 - 총 {0}개의 대상 감지", HitResults.Num());

    // 각 히트 결과에 대해 데미지 이벤트 전송
    for (const FHitResult& HitResult : HitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        if (!HitActor) continue;

        UE_LOGFMT(LogAttackGA, Log, "트레이스 히트 상세 정보 - 대상: {0}, 충돌 지점: {1}, 충돌 본: {2}",
            HitActor->GetName(), HitResult.ImpactPoint.ToString(), HitResult.BoneName.ToString());

        //@무시 대상 추가
        ActorsToIgnore.AddUnique(HitActor);

        //@Damage Event 전달
        SendDamageEvent(HitResult);
    }
}

void UAttackGameplayAbility::EndWeaponTrace()
{
    bIsTracing = false;

    // ActorsToIgnore 배열 비우기
    if (ActorsToIgnore.Num() > 0)
    {
        UE_LOGFMT(LogAttackGA, Log, "트레이스 무시 목록 초기화 - 제거된 액터 수: {0}", ActorsToIgnore.Num());
        ActorsToIgnore.Empty();
    }

    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 종료");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
ACharacterBase* UAttackGameplayAbility::GetCharacterFromActorInfo() const
{
    return Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
}
#pragma endregion