// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterASC.h"
#include "Logging/StructuredLog.h"


#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"



DEFINE_LOG_CATEGORY(LogMonsterASC)

UBaseMonsterASC::UBaseMonsterASC(const FObjectInitializer& ObjectInitializer)
{
	
}

void UBaseMonsterASC::InitializeComponent()
{
	Super::InitializeComponent();

	// @Ability ���� �ֱ� �̺�Ʈ�� Ŀ���� �ݹ� �Լ� ���
	{
		AbilityActivatedCallbacks.AddUObject(this, &UBaseMonsterASC::OnAbilityActivated);
		AbilityEndedCallbacks.AddUObject(this, &UBaseMonsterASC::OnAbilityEnded);
	}
}

bool UBaseMonsterASC::TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{

	// @Ability Spec Handle
	if (AbilityToActivate.IsValid() == false)
	{
		//UE_LOGFMT(LogMonsterASC, Warning, "InternalTryActivateAbility ���� ��ȿ���� ���� Ability Spec Handle! ASC: {0}. AvatarActor: {1}", GetPathName(), GetNameSafe(GetAvatarActor_Direct()));
		return false;
	}

	// @Ability Spec
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityToActivate);
	if (!Spec)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "TryActivateAbility�� ��ȿ���� ���� Handle�� ȣ���߽��ϴ�.");
		return false;
	}
	// @����: Ability List�� LocK�Ͽ�, Ȱ��ȭ ���� Ability Spec�� �ı��Ǵ� ���� �����մϴ�.
	ABILITYLIST_SCOPE_LOCK();
	// @Actor Info
	const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	if (ActorInfo == nullptr || !ActorInfo->OwnerActor.IsValid() || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}
	// @Ability
	UBaseGameplayAbility* Ability = Cast<UBaseGameplayAbility>(Spec->Ability);
	if (!Ability)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "TryActivateAbility�� ��ȿ���� ���� GA�� Ȱ���߽��ϴ�.");
		return false;
	}

	// @ATMR: Ability Tag�� ���輺: "Block", "AR", "AB"
	UBaseGameplayAbility* InstancedAbility = Cast<UBaseGameplayAbility>(Spec->GetPrimaryInstance());
	{
		UBaseGameplayAbility* const CanActivateAbilitySource = InstancedAbility ? InstancedAbility : Ability;

		if (AbilityTagRelationshipMapping.IsValid())
		{
			const FGameplayTagContainer* SourceTags = nullptr;
			const FGameplayTagContainer* TargetTags = nullptr;
			FGameplayTagContainer OutActivationRequiredAbilityTags;
			FGameplayTagContainer OutActivationBlockedAbilityTags;

			AbilityTagRelationshipMapping->GetRequiredAndBlockedActivationTags(CanActivateAbilitySource->AbilityTags, &OutActivationRequiredAbilityTags, &OutActivationBlockedAbilityTags);

			if (!CanActivateAbilitySource->CanActivateAbility(AbilityToActivate, ActorInfo, SourceTags, TargetTags, &OutActivationRequiredAbilityTags, &OutActivationBlockedAbilityTags))
			{
				NotifyAbilityFailed(AbilityToActivate, CanActivateAbilitySource, InternalTryActivateAbilityFailureTags);
				return false;
			}
		}
	}
	// @Instancing Policy
	if (Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
	{
		if (Spec->IsActive())
		{
			if (InstancedAbility)
			{
				bool bReplicateEndAbility = true;
				bool bWasCancelled = false;
				InstancedAbility->EndAbility(AbilityToActivate, ActorInfo, Spec->ActivationInfo, bReplicateEndAbility, bWasCancelled);
			}
			else
			{
				//UE_LOGFMT(LogMonsterASC, Error, "Can't activate instanced per actor ability %s when their is already a currently active instance for this actor.", Ability->GetName());
				return false;
			}
		}
	}
	// @Ability Spec
	Spec->ActivationInfo = FGameplayAbilityActivationInfo(ActorInfo->OwnerActor.Get());
	FGameplayAbilityActivationInfo& ActivationInfo = Spec->ActivationInfo;

	// @Instancing Policy
	// @Instancing Policy -> InstancePerExecution
	if (Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerExecution)
	{
		InstancedAbility = Cast<UBaseGameplayAbility>(CreateNewInstanceOfAbility(*Spec, Ability));
		InstancedAbility->CallActivateAbility(AbilityToActivate, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	}
	// @Instancing Policy -> InstancedPerActor
	else if (InstancedAbility)
	{
		InstancedAbility->CallActivateAbility(AbilityToActivate, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	}
	// @Instancing Policy -> None
	else
	{
		Ability->CallActivateAbility(AbilityToActivate, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	}

	if (InstancedAbility)
	{
		if (OutInstancedAbility)
		{
			*OutInstancedAbility = InstancedAbility;
		}
		InstancedAbility->SetCurrentActivationInfo(ActivationInfo);
	}

	return true;
}

void UBaseMonsterASC::CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore)
{
	Super::CancelAbilitySpec(Spec, Ignore);
	UGameplayAbility* GA = Spec.Ability;
	if (!GA)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "Ability Spec�� Ability ������ �����ϴ�!");
		return;
	}
	// @Activating Abilities: Ȱ��ȭ ��Ͽ��� ����
	{
		if (ActivatingAbilityTags.HasAllExact(GA->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(GA->AbilityTags);
			//UE_LOGFMT(LogMonsterASC, Warning, "{0}�� ��ҵǾ����ϴ�.", GA->GetName());
		}
	}
	
}

void UBaseMonsterASC::ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags)
{
	for (const auto UnblockedAbilityTag : UnblockedAbilityTags)
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (!AbilitySpec.IsActive() && IsValid(AbilitySpec.Ability) && AbilitySpec.Handle.IsValid()
				&& AbilitySpec.Ability->AbilityTags.HasTagExact(UnblockedAbilityTag))
			{
				//if (TryActivateAbility(AbilitySpec.Handle)) //UE_LOGFMT(LogMonsterASC, Warning, "{0}�� �� Ȱ��ȭ �Ǿ����ϴ�!", UnblockedAbilityTag.GetTagName());
			}
			else
			{
				//UE_LOGFMT(LogMonsterASC, Error, "{0}�� �� Ȱ��ȭ�� �����߽��ϴ�.", UnblockedAbilityTag.GetTagName());
			}
		}
	}
}

void UBaseMonsterASC::OnAbilityActivated(UGameplayAbility* Ability)
{
	// @Ability
	if (!Ability)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "{0}�� ��ȿ���� �ʽ��ϴ�", Ability->GetName());
		return;
	}
	// @"Block" and "Cancel": "Ability Tags To Block"/ "Abilit Tags To Cancel" ����
	{
		ApplyAbilityBlockAndCancelTags(Ability->AbilityTags, Ability, true, FGameplayTagContainer::EmptyContainer, true, FGameplayTagContainer::EmptyContainer);
	}
	// @Activating Abilities: Ȱ��ȭ ��Ͽ� �߰�
	{
		if (ActivatingAbilityTags.IsEmpty() || !ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.AppendTags(Ability->AbilityTags);
			//UE_LOGFMT(LogMonsterASC, Warning, "{0}�� Ȱ��ȭ ��Ͽ� �߰��Ǿ����ϴ�.", Ability->GetName());
		}
	}
	//UE_LOGFMT(LogMonsterASC, Warning, "{0}�� Ȱ��ȭ �Ǿ����ϴ�.", Ability->GetName());

	// @TODO: Ability Ȱ��ȭ ������ ASC���� �� �ϵ�...
}

void UBaseMonsterASC::OnAbilityEnded(UGameplayAbility* Ability)
{
	//UE_LOGFMT(LogMonsterASC, Warning, "{0}�� ����Ǿ����ϴ�.", Ability->GetName());
	// @Ability
	if (!Ability)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "{0}�� ��ȿ���� �ʽ��ϴ�", Ability->GetName());
		return;
	}
	// @Activating Abilities
	{
		if (!ActivatingAbilityTags.IsEmpty() && ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(Ability->AbilityTags);
			//UE_LOGFMT(LogMonsterASC, Warning, "{0}�� Ȱ��ȭ ��Ͽ��� ���ŵǾ����ϴ�.", Ability->GetName());
		}
	}
	// @UnBlock
	{
		if (AbilityTagRelationshipMapping)
		{
			FGameplayTagContainer TagsToBlock;
			FGameplayTagContainer DummyContainer;

			AbilityTagRelationshipMapping->GetAbilityTagsToBlockAndCancel(Ability->AbilityTags, &TagsToBlock, &DummyContainer);

			if (!TagsToBlock.IsEmpty())
			{
				UnBlockAbilitiesWithTags(TagsToBlock);
				//UE_LOGFMT(LogMonsterASC, Warning, "Block �Ǿ��� {0}�� �����Ǿ����ϴ�.", TagsToBlock.ToString());
				// @Reactivate
				FGameplayTagContainer TagsToReactivate;
				for (const auto Tag : TagsToBlock)
				{
					if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("Ability.Passive")))
					{
						TagsToReactivate.AddTag(Tag);
					}
				}
				ReactivateUnblockedPassiveAbility(TagsToReactivate);
			}
		}
	}

	// @TODO: Ability Ȱ��ȭ ���� ������ ASC���� �� �ϵ�...
}

void UBaseMonsterASC::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{

}

void UBaseMonsterASC::ClearAbilityInput()
{
}

void UBaseMonsterASC::GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel)
{
	if (AbilityTagRelationshipMapping.Get())
	{
		AbilityTagRelationshipMapping.Get()->GetAbilityTagsToBlockAndCancel(AbilityTags, &OutAbilityTagsToBlock, &OutAbilityTagsToCancel);
	}
}

void UBaseMonsterASC::GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const
{
	if (AbilityTagRelationshipMapping->IsValidLowLevel())
	{
		AbilityTagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, OutActivationRequired, OutActivationBlocked);
	}
}

void UBaseMonsterASC::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer AbilityTagsToBlock;
	FGameplayTagContainer AbilityTagsToCancel;

	if (AbilityTagRelationshipMapping)
	{
		AbilityTagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &AbilityTagsToBlock, &AbilityTagsToCancel);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, AbilityTagsToBlock, bExecuteCancelTags, AbilityTagsToCancel);
}

UAbilityTagRelationshipMapping* UBaseMonsterASC::GetTagRelationship()
{
	return TagRelationship;
}
