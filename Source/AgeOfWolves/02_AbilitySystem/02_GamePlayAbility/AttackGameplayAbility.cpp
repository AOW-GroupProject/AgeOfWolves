#include "AttackGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "07_BlueprintNode/CombatLibrary.h"

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
    if (!SourceActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

    bool bSuccess = UCombatLibrary::SendGameplayEventToTarget(
        FGameplayTag::RequestGameplayTag("EventTag.OnDamaged"),
        HitActor,
        SourceActor,
        HitResult,
        0.0f,
        GetApplyGameplayEffectClass().GetDefaultObject(),
        GetApplySubGameplayEffectClass().GetDefaultObject()
    );

    if (!bSuccess)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - Target: {0}, 사유: 이벤트 전송 실패",
            HitActor->GetName());
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "데미지 이벤트 전송 완료 - Target: {0}, Instigator: {1}, Impact Location: {2}",
        HitActor->GetName(), SourceActor->GetName(), HitResult.ImpactPoint.ToString());
}

void UAttackGameplayAbility::StartWeaponTrace()
{
    bIsTracing = true;

    //@ActorsToIgnore 초기화
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

    //@자기 자신을 무시 목록에 추가
    ActorsToIgnore.AddUnique(Character);

    //@Socket Location
    FVector StartLocation = Character->GetMesh()->GetSocketLocation(WeaponTraceStartSocket);
    FVector EndLocation = Character->GetMesh()->GetSocketLocation(WeaponTraceEndSocket);

    UE_LOGFMT(LogAttackGA, Log, "트레이스 위치 정보 - 시작: {0}, 끝: {1}",
        StartLocation.ToString(), EndLocation.ToString());

    //@Collision Query Params
    FCollisionQueryParams QueryParams;
    for (AActor* ActorToIgnore : ActorsToIgnore)
    {
        if (!IsValid(ActorToIgnore)) continue;
        QueryParams.AddIgnoredActor(ActorToIgnore);
    }

    //@트레이스 수행
    TArray<FHitResult> HitResults;
    switch (TraceType)
    {
    case EWeaponTraceType::Line:
    {
        PerformLineTrace(StartLocation, EndLocation, QueryParams, HitResults);
        break;
    }
    case EWeaponTraceType::Sphere:
    {
        FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereTraceRadius);
        PerformSweepTrace(StartLocation, EndLocation, SphereShape, QueryParams, HitResults);
        break;
    }
    case EWeaponTraceType::Box:
    {
        FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxTraceHalfSize);
        PerformSweepTrace(StartLocation, EndLocation, BoxShape, QueryParams, HitResults);
        break;
    }
    }

//#if ENABLE_DRAW_DEBUG
//    const float DrawDuration = 2.0f;  // 디버그 라인이 표시될 시간
//    const FColor TraceColor = FColor::Red;  // 트레이스 색상
//
//    switch (TraceType)
//    {
//    case EWeaponTraceType::Line:
//    {
//        // 라인 트레이스의 경우 시작점과 끝점을 잇는 선만 그립니다
//        DrawDebugLine(
//            GetWorld(),
//            StartLocation,
//            EndLocation,
//            TraceColor,
//            false,
//            DrawDuration,
//            0,
//            2.0f
//        );
//        break;
//    }
//    case EWeaponTraceType::Sphere:
//    {
//        // 구체 트레이스의 경우 시작점과 끝점에 구체를, 그 사이를 잇는 선을 그립니다
//        DrawDebugSphere(
//            GetWorld(),
//            StartLocation,
//            SphereTraceRadius,
//            12,  // 구체의 세그먼트 수
//            TraceColor,
//            false,
//            DrawDuration
//        );
//        DrawDebugSphere(
//            GetWorld(),
//            EndLocation,
//            SphereTraceRadius,
//            12,
//            TraceColor,
//            false,
//            DrawDuration
//        );
//        DrawDebugLine(
//            GetWorld(),
//            StartLocation,
//            EndLocation,
//            TraceColor,
//            false,
//            DrawDuration,
//            0,
//            2.0f
//        );
//        break;
//    }
//    case EWeaponTraceType::Box:
//    {
//        // 박스 트레이스의 경우 시작점과 끝점에 박스를, 그 사이를 잇는 선을 그립니다
//        FQuat Rotation = FRotationMatrix::MakeFromZ(EndLocation - StartLocation).ToQuat();
//        DrawDebugBox(
//            GetWorld(),
//            StartLocation,
//            BoxTraceHalfSize,
//            Rotation,
//            TraceColor,
//            false,
//            DrawDuration,
//            0,
//            2.0f
//        );
//        DrawDebugBox(
//            GetWorld(),
//            EndLocation,
//            BoxTraceHalfSize,
//            Rotation,
//            TraceColor,
//            false,
//            DrawDuration,
//            0,
//            2.0f
//        );
//        DrawDebugLine(
//            GetWorld(),
//            StartLocation,
//            EndLocation,
//            TraceColor,
//            false,
//            DrawDuration,
//            0,
//            2.0f
//        );
//        break;
//    }
//    }
//
//    // Hit 발생 시 Hit 위치에 점 표시
//    for (const FHitResult& Hit : HitResults)
//    {
//        DrawDebugPoint(
//            GetWorld(),
//            Hit.ImpactPoint,
//            10.0f,  // 점 크기
//            FColor::Green,  // Hit 위치는 녹색으로 표시
//            false,
//            DrawDuration,
//            0
//        );
//    }
//#endif

    if (HitResults.Num() <= 0)
    {
        UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 실행 완료 - 히트 없음");
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 히트 발생 - 총 {0}개의 대상 감지", HitResults.Num());

    //@각 히트 결과에 대해 데미지 이벤트 전송
    for (const FHitResult& HitResult : HitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        if (!HitActor) continue;

        //@이미 무시 목록에 있는 액터는 스킵
        if (ActorsToIgnore.Contains(HitActor))
        {
            UE_LOGFMT(LogAttackGA, Log, "트레이스 히트 무시 - 대상: {0}, 사유: 이미 데미지가 적용된 대상",
                HitActor->GetName());
            continue;
        }

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

void UAttackGameplayAbility::PerformLineTrace(const FVector& Start, const FVector& End,
    FCollisionQueryParams& QueryParams, TArray<FHitResult>& OutHitResults)
{

    GetWorld()->LineTraceMultiByChannel(
        OutHitResults,
        Start,
        End,
        ECC_Visibility,
        QueryParams
    );
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UAttackGameplayAbility::OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag)
{
    Super::OnChainActionActivated_Implementation(ChainActionEventTag);

    UE_LOGFMT(LogAttackGA, Log, "체인 액션 활성화 이벤트 호출 - Ability: {0} | Event Tag: {1}",
        *GetName(),
        *ChainActionEventTag.ToString());
}

void UAttackGameplayAbility::OnChainActionFinished_Implementation(FGameplayTag ChainActionEventTag)
{
    Super::OnChainActionFinished_Implementation(ChainActionEventTag);

    UE_LOGFMT(LogAttackGA, Log, "체인 액션 종료 이벤트 호출 - Ability: {0} | Event Tag: {1}",
        *GetName(),
        *ChainActionEventTag.ToString());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
ACharacterBase* UAttackGameplayAbility::GetCharacterFromActorInfo() const
{
    return Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
}
#pragma endregion