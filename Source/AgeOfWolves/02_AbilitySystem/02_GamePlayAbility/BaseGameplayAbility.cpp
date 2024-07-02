// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "AbilitySystemGlobals.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

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

	/*
	* @목적: Activation Policy가 OnGranted일 경우, Passive GA이므로 ASC에 등록되는 시점에 활성화해줍니다.
	*/
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

void UBaseGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    check(ActorInfo);

    Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UBaseGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    check(ActorInfo);

    Super::InputReleased(Handle, ActorInfo, ActivationInfo);

    // @Cancel: Activation Policy가 WhileInputActive 일 경우.
    {
        if (ActivationPolicy == EAbilityActivationPolicy::WhileInputActive && ActiveApplyGameplayEffectHandle.IsValid())
        {
            if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
            {
                if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(Handle))
                {
                    if (AbilitySpec->IsActive() && IsValid(AbilitySpec->Ability))
                    {
                        CancelAbility(Handle, ActorInfo, ActivationInfo, false);
                    }
                }
            }
        }
    }
}
#pragma endregion

#pragma region Tag Requirements
bool UBaseGameplayAbility::CanActivateAbility(
    const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
    OUT FGameplayTagContainer* ARTags, OUT FGameplayTagContainer* ABTags) const
{
    // @Actor Info 체크
    AActor* const AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
    if (AvatarActor == nullptr || !ShouldActivateAbility(AvatarActor->GetLocalRole()))
    {
        return false;
    }

   // @AR, AB Tags 
    static FGameplayTagContainer DummyContainer1;
    static FGameplayTagContainer DummyContainer2;
    DummyContainer1.Reset();
    DummyContainer2.Reset();

    FGameplayTagContainer& OutActivationRquiredTags = ARTags ? *ARTags : DummyContainer1;
    FGameplayTagContainer& OutActivationBlockedTags = ABTags ? *ABTags : DummyContainer2;

    // @ASC
    UAbilitySystemComponent* const ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!ASC)
    {
        return false;
    }
    
    // @Input Inhibition
    if (ASC->GetUserAbilityActivationInhibited())
    {
        return false;
    }

    // @TODO: Ability System Globals 계속 활용할 것인지??
    // @ASC Globals
    UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();

    // @Cool Down
    if (!AbilitySystemGlobals.ShouldIgnoreCooldowns() && !CheckCooldown(Handle, ActorInfo))
    {
        UE_LOGFMT(LogGA, Error, "Ability could not be activated due to Cooldown: {0}", GetName());
        return false;
    }
    // @Cost
    if (!AbilitySystemGlobals.ShouldIgnoreCosts() && !CheckCost(Handle, ActorInfo))
    {
        UE_LOGFMT(LogGA, Error, "Ability could not be activated due to Cost: {0}", GetName());
        return false;
    }
    /*
    *   @목적: 해당 GA의 Tag Requirements를 확인합니다.
    *   @설명: "Block", "AR", "AB"를 통해 해당 GA 활성화를 위한 Tag 요구 조건을 확인합니다.
    *   @참조: "DoesAbilitySatisfyTagRequirements()" 확인
    */
    if (!DoesAbilitySatisfyTagRequirements(*ASC, SourceTags, TargetTags, &OutActivationRquiredTags, &OutActivationBlockedTags))
    { 
        UE_LOGFMT(LogGA, Error, "Ability could not be activated due to Blocking Tags or Missing Required Tags: {0}", GetName());
        return false;
    }

    // @AbilitySpecHandle
    FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
    if (!Spec)
    {
        UE_LOGFMT(LogGA, Error, "CanActivateAbility {0} failed, called with invalid Handle", GetName());
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

bool UBaseGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, 
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
    FGameplayTagContainer* ARTags, FGameplayTagContainer* ABTags) const
{
    bool bBlocked = false;
    bool bMissing = false;

    // 1. "Blocked" 체크
    if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
    {
        UE_LOGFMT(LogGA, Error, "Ability is Blocked : {0}", GetName());
        bBlocked = true;
    }

    // 2. "Activation Required"와 "Actiation Blocked"에 대한 처리.
    {
        const UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(&AbilitySystemComponent);

        FGameplayTagContainer DummyContainer1 = FGameplayTagContainer::EmptyContainer;
        FGameplayTagContainer DummyContainer2 = FGameplayTagContainer::EmptyContainer;

        // @AR Tags(Activation Required), AB Tags(Activation Blocked)
        FGameplayTagContainer& ActivationRequiredAbilityTags = ARTags ? *ARTags : DummyContainer1;
        static FGameplayTagContainer& ActivationBlockedAbilityTags = ABTags ? *ABTags : DummyContainer2;

        // @설명: 해당 GA의 Ability Tag를 통해 ASC를 통해 ATRM에 정의된 해당 GA의 "Activation Required"와 "Activation Blocked" Ability Tag를 가져옵니다.
        if (BaseASC)
        {
            BaseASC->GetAbilityRelationshipActivationTags(AbilityTags, &ActivationRequiredAbilityTags, &ActivationBlockedAbilityTags);
        }

        // @설명: 해당 GA의 "Activation Required" 혹은 "Activation Blocked"를 통해 활성화 조건 충족여부 확인
        if (ActivationRequiredAbilityTags.Num() || ActivationBlockedAbilityTags.Num())
        {
            static FGameplayTagContainer AbilitySystemComponentTags;
            AbilitySystemComponentTags.Reset();

            BaseASC->GetActivatingAbilityTags(AbilitySystemComponentTags);

            if (!AbilitySystemComponentTags.HasAll(ActivationRequiredAbilityTags))
            {
                bMissing = true;
                UE_LOGFMT(LogGA, Error, "{0} AR 때메 활성화 불가", GetName());

            }
            if (AbilitySystemComponentTags.HasAny(ActivationBlockedAbilityTags))
            {
                bBlocked = true;
                UE_LOGFMT(LogGA, Error, "{0} AB 때메 활성화 불가", GetName());
            }
        }
    }

    // @TODO: GA 적용 대상(Target) 혹은 GA 적용(Source) 주체에 따라서, 달라지는 GA의 활성화 조건은 추후에 구현합니다.


    // @"Blocked" and "AR"/"AB"
    if (bBlocked)
    {
        return false;
    }
    if (bMissing)
    {
        return false;
    }

    return true;
}
#pragma endregion
