#include "AttackGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

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

    //@MainEffect 유효성 검사
    auto MainEffectClass = GetApplyGameplayEffectClass();
    if (!MainEffectClass)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 메인 GameplayEffect 클래스가 유효하지 않음");
        return;
    }

    auto MainEffectCDO = MainEffectClass.GetDefaultObject();
    if (!MainEffectCDO)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 메인 GameplayEffect CDO가 유효하지 않음");
        return;
    }

    //@SubEffect는 선택적으로 처리
    UGameplayEffect* SubEffectCDO = nullptr;
    auto SubEffectClass = GetApplySubGameplayEffectClass();
    if (SubEffectClass)
    {
        SubEffectCDO = SubEffectClass.GetDefaultObject();
    }

    bool bSuccess = UCombatLibrary::SendGameplayEventToTarget(
        FGameplayTag::RequestGameplayTag("EventTag.OnDamaged"),
        HitActor,
        SourceActor,
        HitResult,
        0.0f,
        MainEffectCDO,
        SubEffectCDO
    );

    if (!bSuccess)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - Target: {0}, 사유: 이벤트 전송 실패",
            HitActor->GetName());
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "데미지 이벤트 전송 완료 - Target: {0}, Instigator: {1}, Impact Location: {2}",
        HitActor->GetName(), SourceActor->GetName(), HitResult.ImpactPoint.ToString());

    //@히트 스탑 적용
    if (bEnableHitStop)
    {
        ApplyHitStop();
    }
}

void UAttackGameplayAbility::StartWeaponTrace()
{
    // SCOPE_LOCK은 범위를 벗어날 때 자동으로 잠금 해제
    FScopeLock ScopeLock(&TraceStateLock);

    if (bIsTracing)
    {
        UE_LOGFMT(LogAttackGA, Warning, "트레이스 시작 실패 - 사유: 이미 트레이스가 진행 중");
        return;
    }

    bIsTracing = true;
    ActorsToIgnore.Empty();

    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 시작 - 소켓 정보 [시작: {0}, 끝: {1}]",
        WeaponTraceStartSocket.ToString(), WeaponTraceEndSocket.ToString());
}

void UAttackGameplayAbility::ProcessWeaponTrace()
{
    FScopeLock ScopeLock(&TraceStateLock);

    // 1. 트레이스 상태 확인
    if (!bIsTracing)
    {
        UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 처리 중단 - 사유: 트레이스가 활성화되지 않음");
        return;
    }

    // 2. 캐릭터 및 메시 유효성 검사
    ACharacterBase* Character = GetCharacterFromActorInfo();
    if (!Character || !Character->GetMesh())
    {
        UE_LOGFMT(LogAttackGA, Warning, "무기 트레이스 실패 - 사유: 캐릭터 또는 메시가 유효하지 않음");
        return;
    }

    USkeletalMeshComponent* Mesh = Character->GetMesh();

    // 3. 소켓 유효성 검사
    if (!Mesh->DoesSocketExist(WeaponTraceStartSocket) || !Mesh->DoesSocketExist(WeaponTraceEndSocket))
    {
        UE_LOGFMT(LogAttackGA, Warning, "무기 트레이스 실패 - 사유: 소켓이 존재하지 않음 [시작: {0}, 끝: {1}]",
            *WeaponTraceStartSocket.ToString(), *WeaponTraceEndSocket.ToString());
        return;
    }

    // 4. 트레이스 위치 설정
    FVector StartLocation = Mesh->GetSocketLocation(WeaponTraceStartSocket);
    FVector EndLocation = Mesh->GetSocketLocation(WeaponTraceEndSocket);

    if (StartLocation.Equals(EndLocation, 1.0f))
    {
        UE_LOGFMT(LogAttackGA, Warning, "무기 트레이스 실패 - 사유: 시작점과 끝점이 같음");
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "트레이스 위치 정보 - 시작: {0}, 끝: {1}",
        *StartLocation.ToString(), *EndLocation.ToString());

    // 5. 자기 자신을 무시 목록에 추가
    ActorsToIgnore.AddUnique(TWeakObjectPtr<AActor>(Character));

    // 6. 쿼리 파라미터 설정
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnPhysicalMaterial = true;

    for (const TWeakObjectPtr<AActor>& ActorToIgnore : ActorsToIgnore)
    {
        if (ActorToIgnore.IsValid())
        {
            QueryParams.AddIgnoredActor(ActorToIgnore.Get());
        }
    }

    // 7. 트레이스 수행
    TArray<FHitResult> HitResults;
    bool bTraceSuccess = false;

    switch (TraceType)
    {
    case EWeaponTraceType::Line:
    {
        bTraceSuccess = GetWorld()->LineTraceMultiByChannel(
            HitResults,
            StartLocation,
            EndLocation,
            ECC_Visibility,
            QueryParams
        );
        break;
    }
    case EWeaponTraceType::Sphere:
    {
        FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereTraceRadius);
        bTraceSuccess = GetWorld()->SweepMultiByChannel(
            HitResults,
            StartLocation,
            EndLocation,
            FQuat::Identity,
            ECC_Visibility,
            SphereShape,
            QueryParams
        );
        break;
    }
    case EWeaponTraceType::Box:
    {
        FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxTraceHalfSize);
        bTraceSuccess = GetWorld()->SweepMultiByChannel(
            HitResults,
            StartLocation,
            EndLocation,
            FQuat::Identity,
            ECC_Visibility,
            BoxShape,
            QueryParams
        );
        break;
    }
    }

     //8. 디버그 드로잉
//#if ENABLE_DRAW_DEBUG
//    const float DrawDuration = 2.0f;
//    const FColor TraceColor = FColor::Red;
//    const FColor HitColor = FColor::Green;
//
//    switch (TraceType)
//    {
//    case EWeaponTraceType::Line:
//    {
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
//        DrawDebugSphere(GetWorld(), StartLocation, SphereTraceRadius, 12, TraceColor, false, DrawDuration);
//        DrawDebugSphere(GetWorld(), EndLocation, SphereTraceRadius, 12, TraceColor, false, DrawDuration);
//        DrawDebugLine(GetWorld(), StartLocation, EndLocation, TraceColor, false, DrawDuration);
//        break;
//    }
//    case EWeaponTraceType::Box:
//    {
//        FQuat Rotation = FRotationMatrix::MakeFromZ(EndLocation - StartLocation).ToQuat();
//        DrawDebugBox(GetWorld(), StartLocation, BoxTraceHalfSize, Rotation, TraceColor, false, DrawDuration);
//        DrawDebugBox(GetWorld(), EndLocation, BoxTraceHalfSize, Rotation, TraceColor, false, DrawDuration);
//        DrawDebugLine(GetWorld(), StartLocation, EndLocation, TraceColor, false, DrawDuration);
//        break;
//    }
//    }
//
//    // Hit 지점 표시
//    for (const FHitResult& Hit : HitResults)
//    {
//        DrawDebugPoint(
//            GetWorld(),
//            Hit.ImpactPoint,
//            10.0f,
//            HitColor,
//            false,
//            DrawDuration,
//            0
//        );
//    }
//#endif

    // 9. 트레이스 결과 처리
    if (HitResults.Num() <= 0)
    {
        UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 실행 완료 - 히트 없음");
        return;
    }

    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 히트 발생 - 총 {0}개의 대상 감지", HitResults.Num());

    // 10. 각 히트 결과에 대한 데미지 처리
    for (const FHitResult& HitResult : HitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        if (!HitActor)
        {
            continue;
        }

        // 이미 무시 목록에 있는지 확인
        bool bAlreadyHit = false;
        for (const TWeakObjectPtr<AActor>& IgnoredActor : ActorsToIgnore)
        {
            if (IgnoredActor.IsValid() && IgnoredActor.Get() == HitActor)
            {
                bAlreadyHit = true;
                break;
            }
        }

        if (bAlreadyHit)
        {
            UE_LOGFMT(LogAttackGA, Log, "트레이스 히트 무시 - 대상: {0}, 사유: 이미 데미지가 적용된 대상",
                *HitActor->GetName());
            continue;
        }

        UE_LOGFMT(LogAttackGA, Log, "트레이스 히트 상세 정보 - 대상: {0}, 충돌 지점: {1}, 충돌 본: {2}",
            *HitActor->GetName(), *HitResult.ImpactPoint.ToString(), *HitResult.BoneName.ToString());

        // 무시 목록에 추가
        ActorsToIgnore.Add(TWeakObjectPtr<AActor>(HitActor));

        // 데미지 이벤트 전송
        SendDamageEvent(HitResult);
    }
}

void UAttackGameplayAbility::EndWeaponTrace()
{
    FScopeLock ScopeLock(&TraceStateLock);

    if (!bIsTracing)
    {
        UE_LOGFMT(LogAttackGA, Log, "트레이스 종료 실패 - 사유: 이미 종료된 상태");
        return;
    }

    bIsTracing = false;

    int32 ClearedCount = ActorsToIgnore.Num();
    ActorsToIgnore.Empty();

    UE_LOGFMT(LogAttackGA, Log, "무기 트레이스 종료 - 제거된 무시 대상 수: {0}", ClearedCount);
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

void UAttackGameplayAbility::ApplyHitStop()
{
    //@Avatar
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    if (!SourceActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "HitStop 적용 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

    //@GameInstance
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(SourceActor);
    if (!GameInstance)
    {
        UE_LOGFMT(LogAttackGA, Warning, "HitStop 적용 실패 - 사유: GameInstance가 유효하지 않음");
        return;
    }

    //@TimeManipulationSubsystem
    UTimeManipulationSubsystem* TimeSystem = GameInstance->GetSubsystem<UTimeManipulationSubsystem>();
    if (!TimeSystem)
    {
        UE_LOGFMT(LogAttackGA, Warning, "HitStop 적용 실패 - 사유: TimeManipulationSubsystem을 찾을 수 없음");
        return;
    }

    //@FTimeDilationSettings
    FTimeDilationSettings HitStopSettings;
    HitStopSettings.DilationMode = HitStopMode;
    HitStopSettings.DilationIntensity = HitStopIntensity;

    //@Is Already Applied?
    if (TimeSystem->IsActorTimeDilated(SourceActor))
    {
        UE_LOGFMT(LogAttackGA, Log, "HitStop 적용 스킵 - 사유: 이미 타임 딜레이션이 적용 중");
        return;
    }

    //@Time Dilation
    TimeSystem->ApplyHitStop(SourceActor, HitStopSettings, bGlobalHitStop);

    UE_LOGFMT(LogAttackGA, Log, "HitStop 적용 완료 - 액터: {0}, 모드: {1}, 강도: {2}",
        *SourceActor->GetName(),
        "히트 스톱",
        static_cast<int32>(HitStopIntensity));
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