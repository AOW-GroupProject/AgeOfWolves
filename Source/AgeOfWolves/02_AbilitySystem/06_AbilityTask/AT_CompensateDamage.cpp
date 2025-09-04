#include "AT_CompensateDamage.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"
#include "07_BlueprintNode/CombatLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"

DEFINE_LOG_CATEGORY(LogCompensateDamage)

//@Default Setting
#pragma region Default Setting
UAT_CompensateDamage::UAT_CompensateDamage(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //@기본값 설정
    bOnlyTriggerOnce = false;
    bHasTriggered = false;

    //@이벤트 태그 초기화
    DamageEventTag = FGameplayTag::RequestGameplayTag("EventTag.OnDamaged");
    StrongAttackTag = FGameplayTag::RequestGameplayTag("AttackMagnitude.Strong");
    AttackFailedEventTag = FGameplayTag::RequestGameplayTag("EventTag.OnAttackFailed");

    //@ASC 참조 초기화
    BoundASC.Reset();

    UE_LOGFMT(LogCompensateDamage, Log, "AT_CompensateDamage 생성자 완료");
}

void UAT_CompensateDamage::Activate()
{
    Super::Activate();

    UE_LOGFMT(LogCompensateDamage, Log, "AT_CompensateDamage 활성화 시작");

    //@초기화
    InitializeCompensateDamage();

    //@이벤트 바인딩
    BindToGameplayEvent();

    UE_LOGFMT(LogCompensateDamage, Log, "AT_CompensateDamage 활성화 완료");
}

void UAT_CompensateDamage::OnDestroy(bool AbilityEnded)
{
    UE_LOGFMT(LogCompensateDamage, Log, "AT_CompensateDamage 종료 시작 - AbilityEnded: {0}", AbilityEnded);

    //@이벤트 언바인딩
    UnbindFromGameplayEvent();

    //@참조 정리
    BoundASC.Reset();

    Super::OnDestroy(AbilityEnded);

    UE_LOGFMT(LogCompensateDamage, Log, "AT_CompensateDamage 종료 완료");
}

void UAT_CompensateDamage::BindToGameplayEvent()
{
    //@ASC 가져오기
    if (!AbilitySystemComponent.Get())
    {
        UE_LOGFMT(LogCompensateDamage, Error, "이벤트 바인딩 실패: AbilitySystemComponent가 유효하지 않음");
        EndTask();
        return;
    }

    //@BaseAbilitySystemComponent로 캐스팅
    UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(AbilitySystemComponent);
    if (!BaseASC)
    {
        UE_LOGFMT(LogCompensateDamage, Error, "이벤트 바인딩 실패: BaseAbilitySystemComponent 캐스팅 실패");
        EndTask();
        return;
    }

    BoundASC = BaseASC;

    FDamageEventPreProcess& PreProcessDelegate = BaseASC->GetDamageEventPreProcess();
    GameplayEventHandle = PreProcessDelegate.AddUObject(
        this,
        &UAT_CompensateDamage::OnDamageEventPreProcess
    );

    if (GameplayEventHandle.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Log, "DamageEventPreProcess 델리게이트 바인딩 성공 - EventTag: {0}",
            DamageEventTag.ToString());
    }
    else
    {
        UE_LOGFMT(LogCompensateDamage, Error, "DamageEventPreProcess 델리게이트 바인딩 실패");
        EndTask();
        return;
    }
}

void UAT_CompensateDamage::UnbindFromGameplayEvent()
{
    if (!BoundASC.IsValid())
    {
        return;
    }

    //@델리게이트 언바인딩
    if (GameplayEventHandle.IsValid())
    {
        FDamageEventPreProcess& PreProcessDelegate = BoundASC->GetDamageEventPreProcess();
        PreProcessDelegate.Remove(GameplayEventHandle);
        GameplayEventHandle.Reset();

        UE_LOGFMT(LogCompensateDamage, Log, "DamageEventPreProcess 델리게이트 언바인딩 완료");
    }
}

void UAT_CompensateDamage::InitializeCompensateDamage()
{
    //@태그 유효성 검사
    if (!DamageEventTag.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Error, "초기화 실패: DamageEventTag가 유효하지 않음");
        EndTask();
        return;
    }

    if (!StrongAttackTag.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Error, "초기화 실패: StrongAttackTag가 유효하지 않음");
        EndTask();
        return;
    }

    if (!AttackFailedEventTag.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Error, "초기화 실패: AttackFailedEventTag가 유효하지 않음");
        EndTask();
        return;
    }

    //@상태 초기화
    bHasTriggered = false;

    UE_LOGFMT(LogCompensateDamage, Log, "CompensateDamage 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
UAT_CompensateDamage* UAT_CompensateDamage::WaitForStrongAttackCompensation(
    UGameplayAbility* OwningAbility,
    FName TaskInstanceName,
    bool bOnlyTriggerOnce)
{
    UE_LOGFMT(LogCompensateDamage, Log, "WaitForStrongAttackCompensation 생성 시작 - TaskName: {0}, OnlyOnce: {1}",
        TaskInstanceName.ToString(), bOnlyTriggerOnce);

    //@어빌리티 유효성 검사
    if (!OwningAbility)
    {
        UE_LOGFMT(LogCompensateDamage, Error, "태스크 생성 실패: OwningAbility가 유효하지 않음");
        return nullptr;
    }

    //@태스크 생성
    UAT_CompensateDamage* MyTask = NewAbilityTask<UAT_CompensateDamage>(OwningAbility, TaskInstanceName);
    if (!MyTask)
    {
        UE_LOGFMT(LogCompensateDamage, Error, "태스크 생성 실패: NewAbilityTask 실패");
        return nullptr;
    }

    //@설정 적용
    MyTask->bOnlyTriggerOnce = bOnlyTriggerOnce;

    UE_LOGFMT(LogCompensateDamage, Log, "WaitForStrongAttackCompensation 생성 완료");

    return MyTask;
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UAT_CompensateDamage::OnDamageEventPreProcess(
    const FGameplayTag& EventTag,
    const FGameplayEventData& EventData,
    bool& bShouldContinueProcessing)
{
    UE_LOGFMT(LogCompensateDamage, Log, "데미지 이벤트 전처리 - EventTag: {0}", EventTag.ToString());

    if (!EventData.OptionalObject || !IsValid(EventData.OptionalObject))
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "EventData.OptionalObject가 유효하지 않음");
        return;
    }

    //@이미 트리거되었고 한 번만 트리거 설정인 경우 무시
    if (bOnlyTriggerOnce && bHasTriggered)
    {
        UE_LOGFMT(LogCompensateDamage, Log, "이벤트 무시: 이미 트리거됨 (OnlyTriggerOnce=true)");
        return;
    }

    //@이벤트 데이터 로그
    FString EventInfo = GetEventDataInfoString(EventData);
    UE_LOGFMT(LogCompensateDamage, Log, "이벤트 데이터 정보: {0}", EventInfo);

    //@강공격 여부 확인
    bool bIsStrongAttack = IsStrongAttack(EventData);

    if (bIsStrongAttack)
    {
        UE_LOGFMT(LogCompensateDamage, Log, "강공격 감지됨 - 파훼 매커니즘 활성화");

        //@데미지 처리 중단 지시
        bShouldContinueProcessing = false;

        //@공격 실패 이벤트 전달
        bool bEventSentSuccessfully = SendAttackFailedEvent(EventData);

        if (bEventSentSuccessfully)
        {
            //@강공격 파훼 성공 델리게이트 호출
            OnStrongAttackCountered.Broadcast(
                EventData.Instigator.Get(),
                EventData.Target.Get(),
                EventData
            );

            UE_LOGFMT(LogCompensateDamage, Log, "강공격 파훼 성공 - Attacker: {0}, Defender: {1}",
                EventData.Instigator.Get() ? *EventData.Instigator->GetName() : TEXT("Unknown"),
                EventData.Target.Get() ? *EventData.Target->GetName() : TEXT("Unknown"));
        }
        else
        {
            UE_LOGFMT(LogCompensateDamage, Warning, "강공격 파훼 실패: 이벤트 전달 실패");
        }

        //@트리거 상태 업데이트
        bHasTriggered = true;

        //@한 번만 트리거 설정인 경우 태스크 종료
        if (bOnlyTriggerOnce)
        {
            UE_LOGFMT(LogCompensateDamage, Log, "OnlyTriggerOnce 설정으로 태스크 종료");

            //@다음 프레임에 종료
            GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
                {
                    EndTask();
                });
        }
    }
    else
    {
        UE_LOGFMT(LogCompensateDamage, Log, "일반 공격 감지됨 - 정상 데미지 처리 계속");

        //@일반 데미지 처리 델리게이트 호출
        OnNormalDamageProcessed.Broadcast(
            EventData.Instigator.Get(),
            EventData.Target.Get(),
            EventData
        );

        //@처리 계속 진행
        bShouldContinueProcessing = true;
    }
}

void UAT_CompensateDamage::OnDamageEventReceived(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    UE_LOGFMT(LogCompensateDamage, Log, "데미지 이벤트 수신 - EventTag: {0}", EventTag.ToString());

    //@이미 트리거되었고 한 번만 트리거 설정인 경우 무시
    if (bOnlyTriggerOnce && bHasTriggered)
    {
        UE_LOGFMT(LogCompensateDamage, Log, "이벤트 무시: 이미 트리거됨 (OnlyTriggerOnce=true)");
        return;
    }

    //@이벤트 태그 확인
    if (!EventTag.MatchesTagExact(DamageEventTag))
    {
        UE_LOGFMT(LogCompensateDamage, Log, "이벤트 무시: 매칭되지 않는 태그 - 수신: {0}, 예상: {1}",
            EventTag.ToString(), DamageEventTag.ToString());
        return;
    }

    //@이벤트 데이터 유효성 검사
    if (!EventData.EventTag.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "이벤트 데이터가 유효하지 않음");
        return;
    }

    //@이벤트 데이터 로그
    FString EventInfo = GetEventDataInfoString(EventData);
    UE_LOGFMT(LogCompensateDamage, Log, "이벤트 데이터 정보: {0}", EventInfo);

    //@강공격 여부 확인
    bool bIsStrongAttack = IsStrongAttack(EventData);

    if (bIsStrongAttack)
    {
        UE_LOGFMT(LogCompensateDamage, Log, "강공격 감지됨 - 파훼 매커니즘 활성화");

        //@공격 실패 이벤트 전달
        bool bEventSentSuccessfully = SendAttackFailedEvent(EventData);

        if (bEventSentSuccessfully)
        {
            //@강공격 파훼 성공 델리게이트 호출
            OnStrongAttackCountered.Broadcast(
                EventData.Instigator.Get(),
                EventData.Target.Get(),
                EventData
            );

            UE_LOGFMT(LogCompensateDamage, Log, "강공격 파훼 성공 - Attacker: {0}, Defender: {1}",
                EventData.Instigator.Get() ? *EventData.Instigator->GetName() : TEXT("Unknown"),
                EventData.Target.Get() ? *EventData.Target->GetName() : TEXT("Unknown"));
        }
        else
        {
            UE_LOGFMT(LogCompensateDamage, Warning, "강공격 파훼 실패: 이벤트 전달 실패");
        }

        //@트리거 상태 업데이트
        bHasTriggered = true;

        //@한 번만 트리거 설정인 경우 태스크 종료
        if (bOnlyTriggerOnce)
        {
            UE_LOGFMT(LogCompensateDamage, Log, "OnlyTriggerOnce 설정으로 태스크 종료");
            EndTask();
        }
    }
    else
    {
        UE_LOGFMT(LogCompensateDamage, Log, "일반 공격 감지됨 - 정상 데미지 처리");

        //@일반 데미지 처리 델리게이트 호출
        OnNormalDamageProcessed.Broadcast(
            EventData.Instigator.Get(),
            EventData.Target.Get(),
            EventData
        );
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UAT_CompensateDamage::IsStrongAttack(const FGameplayEventData& EventData) const
{
    if (!EventData.EventTag.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "강공격 확인 실패: EventData가 유효하지 않음");
        return false;
    }

    //@OptionalObject1에서 AssetTags 추출
    FGameplayTagContainer AssetTags = ExtractAssetTagsFromEventData(EventData);

    if (AssetTags.IsEmpty())
    {
        UE_LOGFMT(LogCompensateDamage, Log, "AssetTags가 비어있음 - 일반 공격으로 처리");
        return false;
    }

    //@StrongAttackTag 확인
    bool bHasStrongTag = AssetTags.HasTagExact(StrongAttackTag);

    UE_LOGFMT(LogCompensateDamage, Log, "강공격 태그 확인 - AssetTags: {0}, StrongTag 존재: {1}",
        AssetTags.ToString(), bHasStrongTag);

    return bHasStrongTag;
}

bool UAT_CompensateDamage::SendAttackFailedEvent(const FGameplayEventData& OriginalEventData) const
{
    if (!OriginalEventData.EventTag.IsValid())
    {
        UE_LOGFMT(LogCompensateDamage, Error, "공격 실패 이벤트 전달 실패: OriginalEventData가 유효하지 않음");
        return false;
    }

    //@공격자 확인
    AActor* OriginalAttacker = const_cast<AActor*>(OriginalEventData.Instigator.Get());
    if (!IsValid(OriginalAttacker))
    {
        UE_LOGFMT(LogCompensateDamage, Error, "공격 실패 이벤트 전달 실패: 원본 공격자가 유효하지 않음");
        return false;
    }

    //@수비자(현재 Avatar) 확인
    AActor* Defender = AbilitySystemComponent->GetAvatarActor();
    if (!IsValid(Defender))
    {
        UE_LOGFMT(LogCompensateDamage, Error, "공격 실패 이벤트 전달 실패: 수비자가 유효하지 않음");
        return false;
    }

    //@HitResult 생성 (원본 이벤트에서 추출 시도)
    FHitResult HitResult;
    if (OriginalEventData.ContextHandle.IsValid())
    {
        if (const FHitResult* ContextHitResult = OriginalEventData.ContextHandle.GetHitResult())
        {
            HitResult = *ContextHitResult;
        }
        else
        {
            //@기본 HitResult 생성
            HitResult.Location = Defender->GetActorLocation();
            HitResult.ImpactPoint = Defender->GetActorLocation();
            HitResult.bBlockingHit = true;
        }
    }
    else
    {
        //@기본 HitResult 생성
        HitResult.Location = Defender->GetActorLocation();
        HitResult.ImpactPoint = Defender->GetActorLocation();
        HitResult.bBlockingHit = true;
    }

    //@소유 어빌리티에서 Sub GameplayEffect 가져오기
    UGameplayEffect* SubEffectCDO = nullptr;
    if (UBaseGameplayAbility* BaseAbility = Cast<UBaseGameplayAbility>(Ability))
    {
        auto SubEffectClass = BaseAbility->GetApplySubGameplayEffectClass();
        if (SubEffectClass)
        {
            SubEffectCDO = SubEffectClass.GetDefaultObject();
        }
    }

    UE_LOGFMT(LogCompensateDamage, Log, "공격 실패 이벤트 전달 시작 - Target: {0}, Instigator: {1}, Event: {2}, SubEffect: {3}",
        *OriginalAttacker->GetName(),
        *Defender->GetName(),
        AttackFailedEventTag.ToString(),
        SubEffectCDO ? *SubEffectCDO->GetName() : TEXT("없음"));

    //@CombatLibrary를 통해 이벤트 전달 (Sub Effect 포함)
    bool bSuccess = UCombatLibrary::SendGameplayEventToTarget(
        AttackFailedEventTag,           // EventTag
        OriginalAttacker,               // TargetActor (원래 공격자)
        Defender,                       // InstigatorActor (수비자)
        HitResult,                      // HitResult
        0.0f,                          // Magnitude (기본값)
        SubEffectCDO,                  // OptionalObject: Sub Effect (Groggy)
        nullptr                        // OptionalObject2
    );

    if (bSuccess)
    {
        UE_LOGFMT(LogCompensateDamage, Log, "공격 실패 이벤트 전달 성공 - Target: {0}",
            *OriginalAttacker->GetName());
    }
    else
    {
        UE_LOGFMT(LogCompensateDamage, Error, "공격 실패 이벤트 전달 실패 - Target: {0}",
            *OriginalAttacker->GetName());
    }

    return bSuccess;
}

FGameplayTagContainer UAT_CompensateDamage::ExtractAssetTagsFromEventData(const FGameplayEventData& EventData) const
{
    UE_LOGFMT(LogCompensateDamage, Log, "ExtractAssetTagsFromEventData 시작");

    // 1단계: OptionalObject 유효성 검사
    if (!EventData.OptionalObject)
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "EventData.OptionalObject가 nullptr");
        return FGameplayTagContainer::EmptyContainer;
    }

    if (!IsValid(EventData.OptionalObject))
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "EventData.OptionalObject가 유효하지 않은 객체");
        return FGameplayTagContainer::EmptyContainer;
    }

    // 2단계: GameplayEffect 캐스팅
    UGameplayEffect* MainEffect = Cast<UGameplayEffect>(EventData.OptionalObject);
    if (!MainEffect)
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "OptionalObject를 UGameplayEffect로 캐스팅 실패 - 실제 타입: {0}",
            *EventData.OptionalObject->GetClass()->GetName());
        return FGameplayTagContainer::EmptyContainer;
    }

    // 3단계: MainEffect 추가 검증
    if (!IsValid(MainEffect))
    {
        UE_LOGFMT(LogCompensateDamage, Warning, "캐스팅된 MainEffect가 유효하지 않음");
        return FGameplayTagContainer::EmptyContainer;
    }

    // 4단계: CDO 확인 (에디터에서 가끔 문제 발생)
    if (MainEffect->HasAnyFlags(RF_ClassDefaultObject))
    {
        UE_LOGFMT(LogCompensateDamage, Log, "MainEffect가 CDO임 - 안전하게 처리");
    }

    // 5단계: 태그 컨테이너 접근
    FGameplayTagContainer ResultTags;

    // 구조체 접근 전 추가 안전성 검사
    try
    {
        if (MainEffect->InheritableGameplayEffectTags.Added.IsValid())
        {
            ResultTags = MainEffect->InheritableGameplayEffectTags.Added;
            UE_LOGFMT(LogCompensateDamage, Log, "추출된 AssetTags: {0} (개수: {1})",
                *ResultTags.ToString(), ResultTags.Num());
        }
        else
        {
            UE_LOGFMT(LogCompensateDamage, Warning, "InheritableGameplayEffectTags.Added가 유효하지 않음");
        }
    }
    catch (...)
    {
        UE_LOGFMT(LogCompensateDamage, Error, "태그 추출 중 예외 발생");
        return FGameplayTagContainer::EmptyContainer;
    }

    return ResultTags;
}

FString UAT_CompensateDamage::GetEventDataInfoString(const FGameplayEventData& EventData) const
{
    if (!EventData.EventTag.IsValid())
    {
        return TEXT("EventData: null");
    }

    FString Info = FString::Printf(
        TEXT("EventTag: %s, Target: %s, Instigator: %s, Magnitude: %.2f, HasContext: %s, HasOptionalObject: %s"),
        *EventData.EventTag.ToString(),
        EventData.Target.Get() ? *EventData.Target->GetName() : TEXT("None"),
        EventData.Instigator.Get() ? *EventData.Instigator->GetName() : TEXT("None"),
        EventData.EventMagnitude,
        EventData.ContextHandle.IsValid() ? TEXT("Yes") : TEXT("No"),
        EventData.OptionalObject ? TEXT("Yes") : TEXT("No")
    );

    return Info;
}
#pragma endregion