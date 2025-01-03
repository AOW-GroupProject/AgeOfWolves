#include "BaseGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemGlobals.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/AOWGameplayTags.h"

#include "GameplayEffectCustomApplicationRequirement.h"

DEFINE_LOG_CATEGORY(LogGA)

// UE_LOGFMT(LogGA, Error, "");

//@Defualt Setting
#pragma region Default Setting
UBaseGameplayAbility::UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    //@Editor 상에서 BP 유형의 GA 생성 시 활성화 정책을 설정해줍니다.
    ActivationPolicy = EAbilityActivationPolicy::MAX;
    //@체인 시스템
    bUseChainSystem = false;
}

void UBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    check(ActorInfo);


    auto* ASC = ActorInfo->AbilitySystemComponent.Get();
    if(!ASC)
    {
        return;
    }

    auto BaseASC = Cast<UBaseAbilitySystemComponent>(ASC);
    if (!BaseASC)
    {
        return;
    }

    //@Activation Policy가 On Granted일 경우, 등록 직후 활성화 시도
    if (ActivationPolicy == EAbilityActivationPolicy::OnGranted_Instant
        || ActivationPolicy == EAbilityActivationPolicy::OnGranted_ConditionalPeriodic)
    {
        {
            if (!Spec.IsActive() && IsValid(Spec.Ability))
            {
                BaseASC->TryActivateAbility(Spec.Handle);
            }
        }
    }

}

void UBaseGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnRemoveAbility(ActorInfo, Spec);
}

void UBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnAvatarSet(ActorInfo, Spec);
}

void UBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBaseGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
    Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

}

void UBaseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    check(ActorInfo);

    // @GE 제거(Cost, CoolDown 제외 별도의 GE)
    {
        if (ActiveApplyGameplayEffectHandle.IsValid())
        {
            if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
            {
                ASC->RemoveActiveGameplayEffect(ActiveApplyGameplayEffectHandle);
            }
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
bool UBaseGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        return false;
    }

    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    return true;
}

bool UBaseGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    bool bBlocked = false;
    bool bMissing = false;

    // ToDo : BlockedTag, MissingTag 사용 로직 추가
    // 로직 추가시, Ability 활성화가 실패할 때마다 해당 GameplayTag가 출력 로그 메시지에 포함되거나, showdebug AbilitySystem HUD에서 표시된다."
    UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
    const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
    const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;


    // 1. 이 Ability의 태그들 중 하나라도 "Blocked" 되는지 확인한다.
    if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
    {
        UE_LOGFMT(LogGA, Error, "Ability is Blocked : {0}", GetName());
        bBlocked = true;
    }

    // 2. "Activation Required"와 "Actiation Blocked"에 대한 처리.
    {
        const UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(&AbilitySystemComponent);
        static FGameplayTagContainer AllRequiredTags;
        static FGameplayTagContainer AllBlockedTags;

        AllRequiredTags = ActivationRequiredTags;
        AllBlockedTags = ActivationBlockedTags;

        // @설명: 해당 GA의 Ability Tag를 통해 ASC를 통해 ATRM에 정의된 해당 GA의 "Activation Required"와 "Activation Blocked" Ability Tag를 가져옵니다.
        if (BaseASC)
        {
            BaseASC->GetAbilityRelationshipActivationTags(AbilityTags, &AllRequiredTags, &AllBlockedTags);
        }

        // @설명: 해당 GA의 "Activation Required" 혹은 "Activation Blocked"를 통해 활성화 조건 충족여부 확인
        if (AllRequiredTags.Num() || AllBlockedTags.Num())
        {
            static FGameplayTagContainer AbilitySystemComponentTags;

            AbilitySystemComponentTags.Reset();
            BaseASC->GetOwnedGameplayTags(AbilitySystemComponentTags);

            if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
            {
                bBlocked = true;
                UE_LOGFMT(LogGA, Error, "{0} Activation Block Tag 때문에 활성화 불가", GetName());
            }
            if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
            {
                bMissing = true;
                UE_LOGFMT(LogGA, Error, "{0} Required Tag 때문에 활성화 불가", GetName());

            }
        }
    }

    // @TODO: GA 적용 대상(Target) 혹은 GA 적용(Source) 주체에 따라서, 달라지는 GA의 활성화 조건은 추후에 구현합니다.


    // @"Blocked" and "AR"/"AB"
    if (bBlocked)
    {
        if (OptionalRelevantTags && BlockedTag.IsValid())
        {
            OptionalRelevantTags->AddTag(BlockedTag);
        }
        return false;
    }
    if (bMissing)
    {
        if (OptionalRelevantTags && MissingTag.IsValid())
        {
            OptionalRelevantTags->AddTag(MissingTag);
        }
        return false;
    }

    return true;
}

bool UBaseGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    // 기본 비용 체크
    if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
    {
        UE_LOGFMT(LogGA, Warning, "{0} 기본 비용 체크 실패", GetName());
        return false;
    }

    // Cost GE 유효성 체크
    UGameplayEffect* CostGE = GetCostGameplayEffect();
    if (!CostGE || !ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        UE_LOGFMT(LogGA, Warning, "{0} Cost GE 또는 ActorInfo가 유효하지 않음", GetName());
        return false;
    }

    // Spec 생성
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CostGE->GetClass(), GetAbilityLevel(), ContextHandle);
    const FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

    if (!Spec)
    {
        UE_LOGFMT(LogGA, Error, "{0} GameplayEffectSpec 생성 실패", GetName());
        return false;
    }

    // Application Requirements 체크
    for (const TSubclassOf<UGameplayEffectCustomApplicationRequirement>& RequirementClass : CostGE->ApplicationRequirements)
    {
        if (!RequirementClass)
        {
            continue;
        }

        const UGameplayEffectCustomApplicationRequirement* RequirementCDO = RequirementClass->GetDefaultObject<UGameplayEffectCustomApplicationRequirement>();
        if (!RequirementCDO)
        {
            continue;
        }

        if (!RequirementCDO->CanApplyGameplayEffect_Implementation(CostGE, *Spec, ASC))
        {
            if (OptionalRelevantTags)
            {
                OptionalRelevantTags->AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.ActivateFail.CustomRequirement")));
            }
            UE_LOGFMT(LogGA, Warning, "{0} 커스텀 적용 요구사항 체크 실패", GetName());
            return false;
        }
    }

    UE_LOGFMT(LogGA, Log, "{0} 비용 체크 성공", GetName());
    return true;

}

void UBaseGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    // 부모 클래스 호출
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);

    // Blueprint 구현 함수 호출
    K2_InputPressed();

    UE_LOGFMT(LogGA, Log, "Ability Input Pressed - Ability: {0}", *GetName());
}

void UBaseGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    // 부모 클래스 호출
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);

    // Blueprint 구현 함수 호출
    K2_InputReleased();

    UE_LOGFMT(LogGA, Log, "Ability Input Released - Ability: {0}", *GetName());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UBaseGameplayAbility::OnChainActionActivated_Implementation(FGameplayTag ChainActionAbilityTag)
{
    UE_LOGFMT(LogGA, Log, "체인 액션 활성화 이벤트 수신 - 어빌리티: {0} | 체인 액션 태그: {1} | 체인 가능 태그: {2}",
        *GetName(),
        *ChainActionAbilityTag.ToString(),
        *ChainableAbilityTags.ToString());

    if (ChainableAbilityTags.HasTag(ChainActionAbilityTag))
    {
        UE_LOGFMT(LogGA, Log, "체인 액션 매칭 성공 - 어빌리티: {0} | 매칭된 태그: {1}",
            *GetName(),
            *ChainActionAbilityTag.ToString());
    }

    //@블루프린트에서 오버라이딩
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion