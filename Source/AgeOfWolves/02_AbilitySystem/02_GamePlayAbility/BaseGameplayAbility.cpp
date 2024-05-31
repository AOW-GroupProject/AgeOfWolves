// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemGlobals.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

DEFINE_LOG_CATEGORY(LogGA)

// UE_LOGFMT(LogGA, Error, "");

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
    
	/*
	* @목적: Activation Policy가 OnGranted일 경우, Passive GA이므로 ASC에 등록되는 시점에 활성화해줍니다.
	*/
	if (ActivationPolicy == EAbilityActivationPolicy::OnGranted)
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

bool UBaseGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    bool bBlocked = false;
    bool bMissing = false;

    // 1. "Blocked"와 "Canceled" Ability Tag
    // @TODO: 정상적으로 모두 나오는지 확인
    // @설명: AbilitySystemGlobals를 통해 현재 "Block"되었거나, "Activation Required"가 충족되 않아 취소된 GA의 Ability Tag들을 가져옵니다.
    const UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
    const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
    const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

    // @설명: ASC를 통해 해당 GA가 "Block"되었는지 확인
    if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
    {
         UE_LOGFMT(LogGA, Error, "Yes It is Blocked");

        bBlocked = true;
    }

    // 2. "Activation Required"와 "Actiation Blocked"
    {
        const UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(&AbilitySystemComponent);

        // 1. Activation Required
        static FGameplayTagContainer ActivationRequiredAbilityTags;
        // 2. Activation Blocked 
        static FGameplayTagContainer ActivationBlockedAbilityTags;

        // @설명: 해당 GA의 Ability Tag를 통해 ASC를 통해 ATRM에 정의된 해당 GA의 "Activation Required"와 "Activation Blocked" Ability Tag를 가져옵니다.
        if (BaseASC)
        {
            BaseASC->GetRelationshipActivationTagRequirements(AbilityTags, ActivationRequiredAbilityTags, ActivationBlockedAbilityTags);
        }

        // @설명: 해당 GA의 "Activation Required" 혹은 "Activation Blocked"를 통해 활성화 조건 충족여부 확인
        if (ActivationRequiredAbilityTags.Num() || ActivationBlockedAbilityTags.Num())
        {
            static FGameplayTagContainer AbilitySystemComponentTags;
            AbilitySystemComponentTags.Reset();

            AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

            if (!AbilitySystemComponentTags.HasAll(ActivationRequiredAbilityTags))
            {
                bMissing = true;
                UE_LOGFMT(LogGA, Error, "Yes It is Blocked");

            }
            if (AbilitySystemComponentTags.HasAny(ActivationBlockedAbilityTags))
            {
                bBlocked = true;
                UE_LOGFMT(LogGA, Error, "Yes It is Blocked");

            }
        }
    }

    // @TODO: GA 적용 대상(Target) 혹은 GA 적용(Source) 주체에 따라서, 달라지는 GA의 활성화 조건은 추후에 구현합니다.

    // @설명: 해당 GA가 bMissing(Canceled) 혹은 bBlocked(Blocked) 되었다면, false 반환
    if (bMissing)
    {
        //if (OptionalRelevantTags && MissingTag.IsValid())
        //{
        //    OptionalRelevantTags->AddTag(MissingTag);
        //}
        return false;
    }
    if (bBlocked)
    {
        //if (OptionalRelevantTags && BlockedTag.IsValid())
        //{
        //    OptionalRelevantTags->AddTag(BlockedTag);
        //}
        return false;
    }

    return true;
}
