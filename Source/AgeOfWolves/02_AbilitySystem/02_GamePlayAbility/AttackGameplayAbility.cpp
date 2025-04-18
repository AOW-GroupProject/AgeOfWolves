#include "AttackGameplayAbility.h"
#include "Logging/StructuredLog.h"
#include "UObject/NameTypes.h"

#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "07_BlueprintNode/CombatLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(LogAttackGA)
//@Defualt Setting
#pragma region Default Setting
UAttackGameplayAbility::UAttackGameplayAbility(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UAttackGameplayAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // 몽타주 배열이 변경되었을 때 HitStop 설정 배열 크기를 자동으로 조정
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    // 속성 이름을 문자열로 직접 비교
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UBaseGameplayAbility, AnimMontages))
    {
        if (HitStopSettingMode == EHitStopSettingMode::PerMontage)
        {
            // 몽타주 배열 크기에 맞게 HitStop 설정 배열 크기 조정
            int32 MontageCount = AnimMontages.Num();
            int32 SettingsCount = MontageHitStopSettings.Num();

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
                        MontageHitStopSettings.Add(FMontageHitStopSettings());
                    }
                }
                else
                {
                    // 배열 크기 축소
                    MontageHitStopSettings.SetNum(MontageCount);
                }

                UE_LOG(LogAttackGA, Log, TEXT("몽타주 배열 크기 변경 감지 - HitStop 설정 배열 크기 조정됨 (%d)"), MontageCount);
            }
        }
    }
    // HitStop 설정 모드가 변경되었을 때
    else if (PropertyName == GET_MEMBER_NAME_CHECKED(UAttackGameplayAbility, HitStopSettingMode))
    {
        if (HitStopSettingMode == EHitStopSettingMode::PerMontage)
        {
            // 몽타주 배열 크기에 맞게 HitStop 설정 배열 크기 조정
            int32 MontageCount = AnimMontages.Num();
            MontageHitStopSettings.SetNum(MontageCount);

            UE_LOG(LogAttackGA, Log, TEXT("HitStop 설정 모드 변경 - 몽타주별 설정 모드로 변경됨, 설정 배열 크기: %d"), MontageCount);
        }
    }
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

    //@히트 스탑 적용 - 설정 모드에 따라 다르게 처리
    switch (HitStopSettingMode)
    {
       //@공통 히트 스탑 설정 적용
        case EHitStopSettingMode::Global:
            if (bEnableHitStop)
            {
                ApplyHitStop(HitActor);
            }
            break;
        //@몽타주 별 히트 스탑 설정 적용
        case EHitStopSettingMode::PerMontage:
            ApplyHitStopForCurrentMontage(HitActor);
            break;
    }

    //@이팩트-1 : Impact Effect
    ExecuteImpactGameplayCue(
        HitResult,
        SourceActor);

    //@이팩트-2 : Slash Effect


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
    if (!MontageHitStopSettings.IsValidIndex(TargetMontageIndex))
    {
        UE_LOGFMT(LogAttackGA, Warning, "몽타주별 HitStop 적용 실패 - 사유: MontageIndex({0})가 유효하지 않음", TargetMontageIndex);
        return;
    }

    // 해당 몽타주에 설정된 HitStop 설정 가져오기
    const FMontageHitStopSettings& HitStopSetting = MontageHitStopSettings[TargetMontageIndex];

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

void UAttackGameplayAbility::ExecuteImpactGameplayCue(const FHitResult& HitResult, AActor* SourceActor)
{
    //@ImpactEffectCueTag
    if (!ImpactEffectCueTag.IsValid())
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteImpactGameplayCue 실패 - 사유: 유효하지 않은 ImpactEffectCueTag");
        return;
    }

    //@Gameplay Cue Param
    FGameplayCueParameters CueParams;
    CueParams.Location = HitResult.ImpactPoint;
    CueParams.Normal = HitResult.ImpactNormal;
    CueParams.Instigator = SourceActor;
    CueParams.EffectCauser = SourceActor;
    CueParams.SourceObject = this;

    //@ASC
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        UE_LOGFMT(LogAttackGA, Warning, "ExecuteImpactGameplayCue 실패 - 사유: AbilitySystemComponent가 유효하지 않음");
        return;
    }

    //@Execute Gameplay Cue
    ASC->ExecuteGameplayCue(ImpactEffectCueTag, CueParams);

    UE_LOGFMT(LogAttackGA, Log, "충돌 이펙트 GameplayCue 실행 - 태그: {0}, 위치: {1}",
        *ImpactEffectCueTag.ToString(), *HitResult.ImpactPoint.ToString());
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