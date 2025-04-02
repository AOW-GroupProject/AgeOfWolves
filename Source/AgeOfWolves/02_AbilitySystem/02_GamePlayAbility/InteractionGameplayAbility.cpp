#include "InteractionGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/InteractionComponent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "07_BlueprintNode/CombatLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogInteractionGA)

//@Default Setting
#pragma region Default Setting
UInteractionGameplayAbility::UInteractionGameplayAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //@Tracing
    bIsTracing = false;
    //@Interaction Data
    InteractionData = nullptr;
    //@Target
    TargetActor.Reset();
    //@Interaction 성공 여부
    bInteractionCompleted = false;
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UInteractionGameplayAbility::SendInteractionEvent(const FHitResult& HitResult)
{
    //@Hit Actor
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - 사유: HitActor가 유효하지 않음");
        return;
    }

    //@Source Actor
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    if (!SourceActor)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

    //@이벤트 태그 검사
    if (!InteractionData->GetEventTag().IsValid())
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - 사유: 이벤트 태그가 유효하지 않음");
        return;
    }

    //@GameplayEventData 구성
    FGameplayEventData EventData;
    EventData.Instigator = SourceActor;
    EventData.Target = HitActor;
    EventData.OptionalObject = InteractionData;

    //@이벤트 전송
    bool bSuccess = UCombatLibrary::SendGameplayEventToTarget(
        InteractionData->GetEventTag(),
        HitActor,
        SourceActor,
        HitResult,
        0.0f,
        InteractionData,
        nullptr
    );

    if (!bSuccess)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - Target: {0}, 사유: 이벤트 전송 실패",
            *HitActor->GetName());
        return;
    }

    UE_LOGFMT(LogInteractionGA, Log, "상호작용 이벤트 전송 완료 - Target: {0}, Instigator: {1}, 상호작용 타입: {2}",
        *HitActor->GetName(), *SourceActor->GetName(), static_cast<int32>(InteractionData->GetInteractionType()));
}

void UInteractionGameplayAbility::StartInteractionTrace()
{
    //@타겟 액터 유효성 확인
    if (!TargetActor.IsValid())
    {
        UE_LOGFMT(LogInteractionGA, Warning, "상호작용 트레이스 시작 실패 - 사유: 타겟 액터가 유효하지 않음");
        return;
    }

    //@이미 완료된 상호작용인지 확인
    if (bInteractionCompleted)
    {
        UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 시작 무시 - 사유: 이미 상호작용이 완료됨");
        return;
    }

    //@트레이스 상태 확인
    FScopeLock ScopeLock(&TraceStateLock);

    if (bIsTracing)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "상호작용 트레이스 시작 실패 - 사유: 이미 트레이스가 진행 중");
        return;
    }

    bIsTracing = true;
    ActorsToIgnore.Empty();

    //@자기 자신은 무시 목록에 추가
    ACharacterBase* Character = GetCharacterFromActorInfo();
    if (Character)
    {
        ActorsToIgnore.AddUnique(TWeakObjectPtr<AActor>(Character));
    }

    UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 시작 - 타겟: {0}",
        *TargetActor->GetName());
}

void UInteractionGameplayAbility::ProcessInteractionTrace()
{
    FScopeLock ScopeLock(&TraceStateLock);

    // 트레이스 상태 확인
    if (!bIsTracing)
    {
        UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 처리 중단 - 사유: 트레이스가 활성화되지 않음");
        return;
    }

    // 타겟 액터 유효성 확인
    if (!TargetActor.IsValid())
    {
        UE_LOGFMT(LogInteractionGA, Warning, "상호작용 트레이스 처리 실패 - 사유: 타겟 액터가 유효하지 않음");
        EndInteractionTrace(false);
        return;
    }

    // 이미 완료된 상호작용인지 확인
    if (bInteractionCompleted)
    {
        UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 처리 무시 - 사유: 이미 상호작용이 완료됨");
        EndInteractionTrace(false);
        return;
    }

    // 캐릭터 및 메시 유효성 검사
    ACharacterBase* Character = GetCharacterFromActorInfo();
    if (!Character || !Character->GetMesh())
    {
        UE_LOGFMT(LogInteractionGA, Warning, "상호작용 트레이스 실패 - 사유: 캐릭터 또는 메시가 유효하지 않음");
        EndInteractionTrace(false);
        return;
    }

    USkeletalMeshComponent* Mesh = Character->GetMesh();

    // 시작 위치 설정
    FVector StartLocation;
    FRotator LookRotation;

    if (WeaponTraceStartSocket != NAME_None && Mesh->DoesSocketExist(WeaponTraceStartSocket))
    {
        // 시작 소켓 기반 시작점
        StartLocation = Mesh->GetSocketLocation(WeaponTraceStartSocket);
        LookRotation = Mesh->GetSocketRotation(WeaponTraceStartSocket);
    }
    else
    {
        // 카메라 또는 캐릭터 기반 시작점
        StartLocation = Character->GetActorLocation();
        LookRotation = Character->GetActorRotation();

        // 눈 높이 조정
        StartLocation.Z += Character->BaseEyeHeight;
    }

    // 종료 위치 설정
    FVector EndLocation;
    if (WeaponTraceEndSocket != NAME_None && Mesh->DoesSocketExist(WeaponTraceEndSocket))
    {
        // 종료 소켓이 있는 경우 직접 사용
        EndLocation = Mesh->GetSocketLocation(WeaponTraceEndSocket);
    }
    else
    {
        // 방향으로 계산
        FVector ForwardVector = LookRotation.Vector();
        EndLocation = StartLocation + (ForwardVector * 200.0f);
    }

    UE_LOGFMT(LogInteractionGA, Log, "트레이스 위치 정보 - 시작: {0}, 끝: {1}",
        *StartLocation.ToString(), *EndLocation.ToString());

    // 쿼리 파라미터 설정
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

    // 트레이스 수행
    TArray<FHitResult> HitResults;
    bool bTraceSuccess = false;

    switch (TraceType)
    {
    case EInteractionTraceType::Line:
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
    case EInteractionTraceType::Sphere:
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
    case EInteractionTraceType::Box:
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

    //@트레이스 결과 처리
    if (HitResults.Num() <= 0)
    {
        UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 실행 완료 - 히트 없음");
        return;
    }

    UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 히트 발생 - 총 {0}개의 대상 감지", HitResults.Num());

    // 각 히트 결과에 대한 처리
    for (const FHitResult& HitResult : HitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        if (!HitActor)
        {
            continue;
        }

        // 타겟 액터와 일치하는지 확인
        if (HitActor != TargetActor.Get())
        {
            UE_LOGFMT(LogInteractionGA, Log, "트레이스 히트 무시 - 대상: {0}, 사유: 타겟 액터({1})와 일치하지 않음",
                *HitActor->GetName(), *TargetActor->GetName());
            continue;
        }

        UE_LOGFMT(LogInteractionGA, Log, "타겟 액터 히트 - 대상: {0}, 충돌 지점: {1}",
            *HitActor->GetName(), *HitResult.ImpactPoint.ToString());

        // 히트 결과와 함께 트레이스 종료 호출
        EndInteractionTrace(true, HitResult);
        return;
    }
}

void UInteractionGameplayAbility::EndInteractionTrace(bool bSuccess, const FHitResult& HitResult)
{
    FScopeLock ScopeLock(&TraceStateLock);

    if (!bIsTracing)
    {
        UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 종료 무시 - 사유: 이미 종료된 상태");
        return;
    }

    bIsTracing = false;

    UE_LOGFMT(LogInteractionGA, Log, "상호작용 트레이스 종료 - 성공: {0}", bSuccess ? TEXT("true") : TEXT("false"));

    //@성공
    if (bSuccess && HitResult.GetActor())
    {
        UE_LOGFMT(LogInteractionGA, Log, "상호작용 완료 처리 - 타겟: {0}", *HitResult.GetActor()->GetName());

        // 상호작용 이벤트 전송
        SendInteractionEvent(HitResult);

        // 상호작용 완료 처리
        bInteractionCompleted = true;
    }

    int32 ClearedCount = ActorsToIgnore.Num();
    ActorsToIgnore.Empty();
}

void UInteractionGameplayAbility::PerformLineTrace(const FVector& Start, const FVector& End,
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
void UInteractionGameplayAbility::OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag)
{
    Super::OnChainActionActivated_Implementation(ChainActionEventTag);

    UE_LOGFMT(LogInteractionGA, Log, "체인 액션 활성화 이벤트 호출 - Ability: {0} | Event Tag: {1}",
        *GetName(),
        *ChainActionEventTag.ToString());
}

void UInteractionGameplayAbility::OnChainActionFinished_Implementation(FGameplayTag ChainActionEventTag)
{
    Super::OnChainActionFinished_Implementation(ChainActionEventTag);

    UE_LOGFMT(LogInteractionGA, Log, "체인 액션 종료 이벤트 호출 - Ability: {0} | Event Tag: {1}",
        *GetName(),
        *ChainActionEventTag.ToString());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
ACharacterBase* UInteractionGameplayAbility::GetCharacterFromActorInfo() const
{
    return Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
}

bool UInteractionGameplayAbility::CanInteractWith(AActor* Target) const
{
    if (!Target)
        return false;

    // 기본적인 예시: 오브젝트 태그가 지정된 경우 태그 확인
    if (InteractionData->GetObjectTag().IsValid())
    {
        // 대상 액터가 태그를 갖고 있는지 확인
        ACharacterBase* Character = Cast<ACharacterBase>(TargetActor);
        if (Character)
        {
            FGameplayTag CharacterTag = Character->GetCharacterTag();
            if (!CharacterTag.IsValid() || !CharacterTag.MatchesTag(InteractionData->GetObjectTag()))
                return false;
        }
    }

    return true;
}
#pragma endregion