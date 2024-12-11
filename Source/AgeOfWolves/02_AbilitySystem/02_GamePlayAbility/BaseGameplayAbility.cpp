// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "AbilitySystemGlobals.h"


DEFINE_LOG_CATEGORY(LogGA)

// UE_LOGFMT(LogGA, Error, "");

#pragma region Default Setting
UBaseGameplayAbility::UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// @설명: Editor 상에서 BP 유형의 GA 생성 시 활성화 정책을 설정해줍니다.
	ActivationPolicy = EAbilityActivationPolicy::MAX;
}

void UBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	check(ActorInfo);

    //@Activation Policy가 On Granted일 경우, 등록 직후 활성화 시도
	if (ActivationPolicy == EAbilityActivationPolicy::OnGranted_Instant
        || ActivationPolicy == EAbilityActivationPolicy::OnGranted_ConditionalPeriodic)
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
        {
			if (!Spec.IsActive() && IsValid(Spec.Ability))
			{
				ASC->TryActivateAbility(Spec.Handle);
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

#pragma region Tag Requirements

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

    // @TODO: Input Block 작업
    // @Input Blocked
    //if (ASC->IsAbilityInputBlocked(Spec->InputID))
    //{
    //    if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
    //    {
    //        UE_LOGFMT(LogGA, Error, "Ability could not be activated due to blocked input ID {1}: {2}", Spec->InputID, *GetName());
    //    }
    //    return false;
    //}

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

