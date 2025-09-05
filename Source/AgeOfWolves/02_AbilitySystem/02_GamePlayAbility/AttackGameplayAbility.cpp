#include "AttackGameplayAbility.h"
#include "Logging/StructuredLog.h"
#include "UObject/NameTypes.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "07_BlueprintNode/CombatLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "02_AbilitySystem/06_AbilityTask/AT_CompensateDamage.h"

DEFINE_LOG_CATEGORY(LogAttackGA)

//@Defualt Setting
#pragma region Default Setting
UAttackGameplayAbility::UAttackGameplayAbility(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    //@파훼 태스크 초기화
    CurrentCompensationTask = nullptr;
}

void UAttackGameplayAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // 몽타주 배열이 변경되었을 때 HitStop 설정 배열 크기를 자동으로 조정
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    // 속성 이름을 문자열로 직접 비교
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UBaseGameplayAbility, AnimMontages))
    {
        if (HitStopSettingMode == EFXApplyRange::PerMontage)
        {
            // 몽타주 배열 크기에 맞게 HitStop 설정 배열 크기 조정
            int32 MontageCount = AnimMontages.Num();
            int32 SettingsCount = MontageTimeFXSettings.Num();

            // 크기가 다른 경우 조정
            if (MontageCount != SettingsCount)
            {
                // 배열 크기 조정 (기존 값은 유지)
                if (MontageCount > SettingsCount)
                {
                    // 새 항목 추가
                    int32 ItemsToAdd = MontageCount - SettingsCount;
                    for (int32 i = 0; i < ItemsToAdd; i++)
                    {
                        MontageTimeFXSettings.Add(FTimeFXSetting());
                    }
                }
                else
                {
                    // 배열 크기 축소
                    MontageTimeFXSettings.SetNum(MontageCount);
                }

                UE_LOG(LogAttackGA, Log, TEXT("몽타주 배열 크기 변경 감지 - HitStop 설정 배열 크기 조정됨 (%d)"), MontageCount);
            }
        }
    }
    // HitStop 설정 모드가 변경되었을 때
    else if (PropertyName == GET_MEMBER_NAME_CHECKED(UAttackGameplayAbility, HitStopSettingMode))
    {
        if (HitStopSettingMode == EFXApplyRange::PerMontage)
        {
            // 몽타주 배열 크기에 맞게 HitStop 설정 배열 크기 조정
            int32 MontageCount = AnimMontages.Num();
            MontageTimeFXSettings.SetNum(MontageCount);

            UE_LOG(LogAttackGA, Log, TEXT("HitStop 설정 모드 변경 - 몽타주별 설정 모드로 변경됨, 설정 배열 크기: %d"), MontageCount);
        }
    }
}

void UAttackGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    //@파훼 태스크 정리 (부모 클래스 호출 전에 수행)
    DeactivateCompensationTask();

    UE_LOGFMT(LogAttackGA, Log, "공격 어빌리티 종료 시 파훼 태스크 정리 완료 - 어빌리티: {0}, 취소 여부: {1}",
        *GetName(), bWasCancelled ? TEXT("취소됨") : TEXT("정상 종료"));

    //@부모 클래스의 EndAbility 호출
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
UAbilityTask_PlayMontageAndWait* UAttackGameplayAbility::PlayMontageWithCallbackAndIndex(
    UAnimMontage* MontageToPlay,
    int32 MontageIndex,
    float Rate,
    FName StartSection,
    bool bStopWhenAbilityEnds)
{
    //@현재 몽타주 인덱스 설정
    CurrentMontageIndex = MontageIndex;

    UE_LOGFMT(LogAttackGA, Log, "몽타주 재생 시작 - 몽타주: {0}, 인덱스: {1}, 재생률: {2}",
        *MontageToPlay->GetName(), MontageIndex, Rate);

    return PlayMontageWithCallback(MontageToPlay, Rate, StartSection, bStopWhenAbilityEnds);
}

void UAttackGameplayAbility::SendDamageEvent(const FHitResult& HitResult)
{
    // === 기존 유효성 검사 (그대로) ===
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: HitActor가 유효하지 않음");
        return;
    }

    AActor* SourceActor = GetAvatarActorFromActorInfo();
    if (!SourceActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "SendDamageEvent 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

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

    UGameplayEffect* SubEffectCDO = nullptr;
    auto SubEffectClass = GetApplySubGameplayEffectClass();
    if (SubEffectClass)
    {
        SubEffectCDO = SubEffectClass.GetDefaultObject();
    }

    // === 기존 데미지 이벤트 전송 (Target에게) ===
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

    UE_LOGFMT(LogAttackGA, Log, "데미지 이벤트 전송 완료 - Target: {0}, Instigator: {1}",
        HitActor->GetName(), SourceActor->GetName());

    // === 새로운 부분: Source ASC의 데미지 전달 델리게이트 호출 ===
    if (UBaseAbilitySystemComponent* SourceASC = Cast<UBaseAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
    {
        // EventData 구성
        FGameplayEventData DamageDealtEventData;
        DamageDealtEventData.Instigator = SourceActor;
        DamageDealtEventData.Target = HitActor;
        DamageDealtEventData.EventTag = FGameplayTag::RequestGameplayTag("EventTag.OnDamageDealt");
        DamageDealtEventData.EventMagnitude = 0.0f; // 실제 데미지량은 GE에서 계산됨

        // HitResult를 Context에 추가
        FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
        if (FGameplayEffectContext* Context = ContextHandle.Get())
        {
            Context->AddHitResult(HitResult);
        }
        DamageDealtEventData.ContextHandle = ContextHandle;

        // ASC의 데미지 전달 델리게이트 호출
        SourceASC->DamageDealtByActor.Broadcast(SourceActor, HitActor, DamageDealtEventData);

        UE_LOGFMT(LogAttackGA, Log, "데미지 전달 델리게이트 호출 완료 - Source: {0}, Target: {1}",
            *SourceActor->GetName(), *HitActor->GetName());
    }
    else
    {
        UE_LOGFMT(LogAttackGA, Warning, "데미지 전달 델리게이트 호출 실패 - Source ASC를 찾을 수 없음");
    }

    // === 기존 효과 처리 (그대로) ===
    ExecuteTimeFX(HitResult, SourceActor);
    ExecuteCollisionFX(HitResult, SourceActor);
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
    case EWeaponTraceType::Cylinder:
    {
        FCollisionShape CylinderShape = FCollisionShape::MakeCapsule(CylinderRadius, CylinderHalfHeight);
        // Z축을 기준으로 회전된 캡슐은 방향 벡터에 맞게 회전시켜야 함
        FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
        FQuat Rotation = FQuat::FindBetweenNormals(FVector(0, 0, 1), Direction);

        bTraceSuccess = GetWorld()->SweepMultiByChannel(
            HitResults,
            StartLocation,
            EndLocation,
            Rotation,
            ECC_Visibility,
            CylinderShape,
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
//        case EWeaponTraceType::Line:
//        {
//            DrawDebugLine(
//                GetWorld(),
//                StartLocation,
//                EndLocation,
//                TraceColor,
//                false,
//                DrawDuration,
//                0,
//                2.0f
//            );
//            break;
//        }
//        case EWeaponTraceType::Sphere:
//        {
//            DrawDebugSphere(GetWorld(), StartLocation, SphereTraceRadius, 12, TraceColor, false, DrawDuration);
//            DrawDebugSphere(GetWorld(), EndLocation, SphereTraceRadius, 12, TraceColor, false, DrawDuration);
//            DrawDebugLine(GetWorld(), StartLocation, EndLocation, TraceColor, false, DrawDuration);
//            break;
//        }
//        case EWeaponTraceType::Box:
//        {
//            FQuat Rotation = FRotationMatrix::MakeFromZ(EndLocation - StartLocation).ToQuat();
//            DrawDebugBox(GetWorld(), StartLocation, BoxTraceHalfSize, Rotation, TraceColor, false, DrawDuration);
//            DrawDebugBox(GetWorld(), EndLocation, BoxTraceHalfSize, Rotation, TraceColor, false, DrawDuration);
//            DrawDebugLine(GetWorld(), StartLocation, EndLocation, TraceColor, false, DrawDuration);
//            break;
//        }
//        case EWeaponTraceType::Cylinder:
//        {
//            FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
//            FQuat Rotation = FQuat::FindBetweenNormals(FVector(0, 0, 1), Direction);
//
//            // 실린더 디버그 드로잉 - 시작점과 끝점에 캡슐 표시
//            DrawDebugCapsule(
//                GetWorld(),
//                StartLocation,
//                CylinderHalfHeight,
//                CylinderRadius,
//                Rotation,
//                TraceColor,
//                false,
//                DrawDuration
//            );
//            DrawDebugCapsule(
//                GetWorld(),
//                EndLocation,
//                CylinderHalfHeight,
//                CylinderRadius,
//                Rotation,
//                TraceColor,
//                false,
//                DrawDuration
//            );
//            DrawDebugLine(GetWorld(), StartLocation, EndLocation, TraceColor, false, DrawDuration);
//            break;
//        }
//    }
//
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

        //@이미 무시 목록에 있는지 확인
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

        //@무시 목록에 추가
        ActorsToIgnore.Add(TWeakObjectPtr<AActor>(HitActor));

        //@데미지 이벤트 전송
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

void UAttackGameplayAbility::ExecuteTimeFX(const FHitResult& HitResult, AActor* SourceActor)
{
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor || !SourceActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteTimeFX 실패 - 사유: Target 또는 Source가 유효하지 않음");
        return;
    }

    //@히트 스탑 적용 - 설정 모드에 따라 다르게 처리
    switch (HitStopSettingMode)
    {
        //@공통 히트 스탑 설정 적용
    case EFXApplyRange::Global:
        if (bEnableHitStop)
        {
            ApplyHitStop(HitActor);
        }
        break;
        //@몽타주 별 히트 스탑 설정 적용
    case EFXApplyRange::PerMontage:
        ApplyHitStopForCurrentMontage(HitActor);
        break;
    }
}

void UAttackGameplayAbility::ApplyHitStop(AActor* Target)
{
    //@Avatar
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    if (!SourceActor || !Target)
    {
        UE_LOGFMT(LogAttackGA, Warning, "HitStop 적용 실패 - 사유: Source Actor 또는 Target이 유효하지 않음");
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
    TimeSystem->ApplyHitStop(SourceActor, Target, HitStopSettings, bGlobalHitStop);

    UE_LOGFMT(LogAttackGA, Log, "기본 HitStop 적용 완료 - 액터: {0}, 모드: {1}, 강도: {2}",
        *SourceActor->GetName(),
        "히트 스톱",
        static_cast<int32>(HitStopIntensity));
}

void UAttackGameplayAbility::ApplyHitStopForCurrentMontage(AActor* Target, int32 MontageIndex)
{
    //@Avatar
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    if (!SourceActor || !Target)
    {
        UE_LOGFMT(LogAttackGA, Warning, "몽타주별 HitStop 적용 실패 - 사유: Source Actor 또는 Target이 유효하지 않음");
        return;
    }

    //@GameInstance
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(SourceActor);
    if (!GameInstance)
    {
        UE_LOGFMT(LogAttackGA, Warning, "몽타주별 HitStop 적용 실패 - 사유: GameInstance가 유효하지 않음");
        return;
    }

    //@TimeManipulationSubsystem
    UTimeManipulationSubsystem* TimeSystem = GameInstance->GetSubsystem<UTimeManipulationSubsystem>();
    if (!TimeSystem)
    {
        UE_LOGFMT(LogAttackGA, Warning, "몽타주별 HitStop 적용 실패 - 사유: TimeManipulationSubsystem을 찾을 수 없음");
        return;
    }

    // 적용할 몽타주 인덱스 결정
    int32 TargetMontageIndex = (MontageIndex >= 0) ? MontageIndex : CurrentMontageIndex;

    // 인덱스 유효성 검사
    if (!MontageTimeFXSettings.IsValidIndex(TargetMontageIndex))
    {
        UE_LOGFMT(LogAttackGA, Warning, "몽타주별 HitStop 적용 실패 - 사유: MontageIndex({0})가 유효하지 않음", TargetMontageIndex);
        return;
    }

    // 해당 몽타주에 설정된 HitStop 설정 가져오기
    const auto& HitStopSetting = MontageTimeFXSettings[TargetMontageIndex];

    // HitStop이 비활성화된 경우 스킵
    if (!HitStopSetting.bEnableHitStop)
    {
        UE_LOGFMT(LogAttackGA, Log, "몽타주별 HitStop 적용 스킵 - 사유: 몽타주({0})에 HitStop이 비활성화됨", TargetMontageIndex);
        return;
    }

    //@Is Already Applied?
    if (TimeSystem->IsActorTimeDilated(SourceActor))
    {
        UE_LOGFMT(LogAttackGA, Log, "몽타주별 HitStop 적용 스킵 - 사유: 이미 타임 딜레이션이 적용 중");
        return;
    }

    //@FTimeDilationSettings
    FTimeDilationSettings HitStopSettings;
    HitStopSettings.DilationMode = HitStopSetting.HitStopMode;
    HitStopSettings.DilationIntensity = HitStopSetting.HitStopIntensity;

    //@Time Dilation
    TimeSystem->ApplyHitStop(SourceActor, Target, HitStopSettings, HitStopSetting.bGlobalHitStop);

    FString MontageName = "Unknown";
    if (AnimMontages.IsValidIndex(TargetMontageIndex) && AnimMontages[TargetMontageIndex])
    {
        MontageName = AnimMontages[TargetMontageIndex]->GetName();
    }

    UE_LOGFMT(LogAttackGA, Log, "몽타주별 HitStop 적용 완료 - 액터: {0}, 몽타주: {1}, 모드: {2}, 강도: {3}",
        *SourceActor->GetName(),
        *MontageName,
        "히트 스톱",
        static_cast<int32>(HitStopSetting.HitStopIntensity));
}

void UAttackGameplayAbility::ExecuteCollisionFX(const FHitResult& HitResult, AActor* SourceActor)
{
    //@매개변수 유효성 검사
    if (!SourceActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteCollisionFX 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

    //@설정 모드에 따라 다르게 처리
    switch (FXSettingMode)
    {
        //@공통 FX 설정 적용
    case EFXApplyRange::Global:
    {
        if (!GlobalFXSetting.IsEnabled() || !GlobalFXSetting.GetEffectCueTag().IsValid())
        {
            UE_LOGFMT(LogAttackGA, Warning, "ExecuteCollisionFX 실패 - 사유: 전역 FX 설정이 비활성화되었거나 CueTag가 유효하지 않음");
            return;
        }

        //@위치 결정: 충돌 위치, 소켓, 또는 커스텀 위치
        FTransform SpawnTransform = FTransform::Identity;

        // 충돌 위치 우선 사용 여부 확인
        if (GlobalFXSetting.UseImpactLocation())
        {
            // 충돌 위치 직접 사용
            SpawnTransform.SetLocation(HitResult.ImpactPoint);
            SpawnTransform.SetRotation(FQuat(FRotator(0.f, 0.f, 0.f)));

            UE_LOGFMT(LogAttackGA, Log, "FX 위치 설정 - 충돌 위치 사용: {0}",
                *HitResult.ImpactPoint.ToString());
        }
        else if (GlobalFXSetting.UseSocket())
        {
            //@소켓 위치 가져오기
            if (!GetSocketTransform(GlobalFXSetting.GetSocketName(), SpawnTransform))
            {
                //@소켓 없으면 충돌 위치 사용
                SpawnTransform.SetLocation(HitResult.ImpactPoint);
                SpawnTransform.SetRotation(FQuat(FRotator(0.f, 0.f, 0.f)));

                UE_LOGFMT(LogAttackGA, Warning, "FX 위치 설정 - 소켓({0})을 찾을 수 없어 충돌 위치 대체 사용: {1}",
                    *GlobalFXSetting.GetSocketName().ToString(), *HitResult.ImpactPoint.ToString());
            }
            else
            {
                UE_LOGFMT(LogAttackGA, Log, "FX 위치 설정 - 소켓({0}) 위치 사용: {1}",
                    *GlobalFXSetting.GetSocketName().ToString(), *SpawnTransform.GetLocation().ToString());
            }
        }
        else
        {
            //@커스텀 위치 사용
            SpawnTransform.SetLocation(GlobalFXSetting.GetCustomLocation());
            SpawnTransform.SetRotation(FQuat(GlobalFXSetting.GetEffectRotation()));

            UE_LOGFMT(LogAttackGA, Log, "FX 위치 설정 - 커스텀 위치 사용: {0}",
                *GlobalFXSetting.GetCustomLocation().ToString());
        }

        // 회전 설정 - 충돌 위치를 사용하지 않을 때만 커스텀 회전 적용
        if (!GlobalFXSetting.UseImpactLocation() && !GlobalFXSetting.UseSocket())
        {
            SpawnTransform.SetRotation(FQuat(GlobalFXSetting.GetEffectRotation()));
        }

        //@스케일 설정
        SpawnTransform.SetScale3D(GlobalFXSetting.GetEffectScale());

        //@이펙트 실행
        ExecuteGameplayCueAtLocation(GlobalFXSetting.GetEffectCueTag(), SpawnTransform, SourceActor);
    }
    break;

    //@몽타주별 FX 설정 적용
    case EFXApplyRange::PerMontage:
        ExecuteCollisionFXForCurrentMontage(HitResult, SourceActor);
        break;
    }
}

void UAttackGameplayAbility::ExecuteGameplayCueAtLocation(const FGameplayTag& CueTag, const FTransform& SpawnTransform, AActor* SourceActor)
{
    //@태그 유효성 검사
    if (!CueTag.IsValid())
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteGameplayCueAtLocation 실패 - 사유: CueTag가 유효하지 않음");
        return;
    }

    //@ASC 유효성 검사
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteGameplayCueAtLocation 실패 - 사유: AbilitySystemComponent가 유효하지 않음");
        return;
    }

    //@Gameplay Cue Param
    FGameplayCueParameters CueParams;
    CueParams.Location = SpawnTransform.GetLocation();
    CueParams.Normal = SpawnTransform.GetRotation().GetForwardVector();
    CueParams.Instigator = SourceActor;
    CueParams.EffectCauser = SourceActor;
    CueParams.SourceObject = this;

    //@Execute Gameplay Cue
    ASC->ExecuteGameplayCue(CueTag, CueParams);

    UE_LOGFMT(LogAttackGA, Log, "GameplayCue 실행 완료 - 태그: {0}, 위치: {1}, 회전: {2}, 스케일: {3}",
        *CueTag.ToString(), *SpawnTransform.GetLocation().ToString(),
        *SpawnTransform.GetRotation().Rotator().ToString(), *SpawnTransform.GetScale3D().ToString());
}

void UAttackGameplayAbility::ExecuteCollisionFXForCurrentMontage(const FHitResult& HitResult, AActor* SourceActor, int32 MontageIndex)
{
    //@매개변수 유효성 검사
    if (!SourceActor)
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteCollisionFXForCurrentMontage 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

    //@몽타주 인덱스 결정
    int32 TargetMontageIndex = (MontageIndex >= 0) ? MontageIndex : CurrentMontageIndex;

    //@인덱스 유효성 검사
    if (!MontageFXSettings.IsValidIndex(TargetMontageIndex))
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteCollisionFXForCurrentMontage 실패 - 사유: 몽타주 인덱스({0})가 유효하지 않음", TargetMontageIndex);
        return;
    }

    //@해당 몽타주의 FX 설정 가져오기
    const FCollisionFXSetting& FXSetting = MontageFXSettings[TargetMontageIndex];

    //@설정 유효성 검사
    if (!FXSetting.IsEnabled() || !FXSetting.GetEffectCueTag().IsValid())
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteCollisionFXForCurrentMontage 실패 - 사유: 몽타주({0})의 FX 설정이 비활성화되었거나 CueTag가 유효하지 않음", TargetMontageIndex);
        return;
    }

    //@위치 결정: 소켓 또는 커스텀 위치
    FTransform SpawnTransform;
    if (FXSetting.UseSocket())
    {
        //@소켓 위치 가져오기
        if (!GetSocketTransform(FXSetting.GetSocketName(), SpawnTransform))
        {
            //@소켓 없으면 충돌 위치 사용
            SpawnTransform.SetLocation(HitResult.ImpactPoint);
            SpawnTransform.SetRotation(FQuat(HitResult.ImpactNormal.Rotation()));
        }
    }
    else
    {
        //@커스텀 위치 사용
        SpawnTransform.SetLocation(FXSetting.GetCustomLocation());
        SpawnTransform.SetRotation(FQuat(FXSetting.GetEffectRotation()));
    }

    //@스케일 설정
    SpawnTransform.SetScale3D(FXSetting.GetEffectScale());

    //@이펙트 실행
    ExecuteGameplayCueAtLocation(FXSetting.GetEffectCueTag(), SpawnTransform, SourceActor);
}

void UAttackGameplayAbility::ActivateCompensationTask(bool bOnlyTriggerOnce)
{
    //@기존 태스크 정리
    if (CurrentCompensationTask && CurrentCompensationTask->IsActive())
    {
        UE_LOGFMT(LogAttackGA, Warning, "파훼 태스크 활성화 실패 - 사유: 이미 활성화된 태스크 존재");
        return;
    }

    //@새로운 파훼 태스크 생성
    CurrentCompensationTask = UAT_CompensateDamage::WaitForStrongAttackCompensation(
        this,
        FName("CompensationTask"),
        bOnlyTriggerOnce
    );

    if (!CurrentCompensationTask)
    {
        UE_LOGFMT(LogAttackGA, Warning, "파훼 태스크 생성 실패");
        return;
    }

    //@강공격 파훼 델리게이트 바인딩
    StrongAttackCounteredHandle = CurrentCompensationTask->OnStrongAttackCountered.AddUObject(
        this,
        &UAttackGameplayAbility::OnStrongAttackCountered
    );

    if (StrongAttackCounteredHandle.IsValid())
    {
        UE_LOGFMT(LogAttackGA, Log, "강공격 파훼 델리게이트 바인딩 성공");
    }
    else
    {
        UE_LOGFMT(LogAttackGA, Warning, "강공격 파훼 델리게이트 바인딩 실패");
    }

    //@태스크 활성화
    CurrentCompensationTask->ReadyForActivation();

    UE_LOGFMT(LogAttackGA, Log, "파훼 태스크 활성화 완료 - OnlyTriggerOnce: {0}", bOnlyTriggerOnce);
}

void UAttackGameplayAbility::DeactivateCompensationTask()
{
    if (CurrentCompensationTask)
    {
        //@델리게이트 언바인딩
        if (StrongAttackCounteredHandle.IsValid())
        {
            CurrentCompensationTask->OnStrongAttackCountered.Remove(StrongAttackCounteredHandle);
            StrongAttackCounteredHandle.Reset();
            UE_LOGFMT(LogAttackGA, Log, "강공격 파훼 델리게이트 언바인딩 완료");
        }

        //@태스크 종료
        if (CurrentCompensationTask->IsActive())
        {
            CurrentCompensationTask->EndTask();
        }
        CurrentCompensationTask = nullptr;

        UE_LOGFMT(LogAttackGA, Log, "파훼 태스크 비활성화 완료");
    }
    else
    {
        UE_LOGFMT(LogAttackGA, Log, "파훼 태스크 비활성화 스킵 - 사유: 활성화된 태스크가 없음");
    }
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

void UAttackGameplayAbility::OnStrongAttackCountered_Implementation(const AActor* Attacker, const AActor* Defender, const FGameplayEventData& EventData)
{
    UE_LOGFMT(LogAttackGA, Log, "강공격 파훼 성공 콜백 호출 - 공격자: {0}, 수비자: {1}",
        Attacker ? *Attacker->GetName() : TEXT("Unknown"),
        Defender ? *Defender->GetName() : TEXT("Unknown"));

    // 블루프린트에서 추가 로직을 구현할 수 있도록 기본 구현은 비워둠
    // 필요시 여기에 기본 C++ 로직 추가 가능
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
ACharacterBase* UAttackGameplayAbility::GetCharacterFromActorInfo() const
{
    return Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
}

bool UAttackGameplayAbility::GetSocketTransform(FName SocketName, FTransform& OutTransform) const
{
    //@소켓 이름 유효성 검사
    if (SocketName.IsNone())
    {
        UE_LOGFMT(LogAttackGA, Warning, "GetSocketTransform 실패 - 사유: 소켓 이름이 유효하지 않음");
        return false;
    }

    //@캐릭터 및 메시 유효성 검사
    ACharacterBase* Character = GetCharacterFromActorInfo();
    if (!Character || !Character->GetMesh())
    {
        UE_LOGFMT(LogAttackGA, Warning, "GetSocketTransform 실패 - 사유: 캐릭터 또는 메시가 유효하지 않음");
        return false;
    }

    USkeletalMeshComponent* Mesh = Character->GetMesh();

    //@소켓 존재 여부 확인
    if (!Mesh->DoesSocketExist(SocketName))
    {
        UE_LOGFMT(LogAttackGA, Warning, "GetSocketTransform 실패 - 사유: 소켓({0})이 존재하지 않음", *SocketName.ToString());
        return false;
    }

    //@소켓 트랜스폼 가져오기
    OutTransform = Mesh->GetSocketTransform(SocketName);
    return true;
}

UAT_CompensateDamage* UAttackGameplayAbility::GetCompensationTask() const
{
    return CurrentCompensationTask;
}

//@파훼 태스크 활성화 상태 확인
bool UAttackGameplayAbility::IsCompensationTaskActive() const
{
    return CurrentCompensationTask && CurrentCompensationTask->IsActive();
}
#pragma endregion