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

	// @Ability 생명 주기 이벤트에 커스텀 콜백 함수 등록
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
		//UE_LOGFMT(LogMonsterASC, Warning, "InternalTryActivateAbility 에서 유효하지 않은 Ability Spec Handle! ASC: {0}. AvatarActor: {1}", GetPathName(), GetNameSafe(GetAvatarActor_Direct()));
		return false;
	}

	// @Ability Spec
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityToActivate);
	if (!Spec)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "TryActivateAbility는 유효하지 않은 Handle을 호출했습니다.");
		return false;
	}
	// @설명: Ability List를 LocK하여, 활성화 도중 Ability Spec이 파괴되는 것을 방지합니다.
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
		//UE_LOGFMT(LogMonsterASC, Error, "TryActivateAbility는 유효하지 않은 GA를 활용했습니다.");
		return false;
	}

	// @ATMR: Ability Tag간 관계성: "Block", "AR", "AB"
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
		//UE_LOGFMT(LogMonsterASC, Error, "Ability Spec에 Ability 정보가 없습니다!");
		return;
	}
	// @Activating Abilities: 활성화 목록에서 제거
	{
		if (ActivatingAbilityTags.HasAllExact(GA->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(GA->AbilityTags);
			//UE_LOGFMT(LogMonsterASC, Warning, "{0}이 취소되었습니다.", GA->GetName());
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
				//if (TryActivateAbility(AbilitySpec.Handle)) //UE_LOGFMT(LogMonsterASC, Warning, "{0}이 재 활성화 되었습니다!", UnblockedAbilityTag.GetTagName());
			}
			else
			{
				//UE_LOGFMT(LogMonsterASC, Error, "{0}이 재 활성화에 실패했습니다.", UnblockedAbilityTag.GetTagName());
			}
		}
	}
}

void UBaseMonsterASC::OnAbilityActivated(UGameplayAbility* Ability)
{
	// @Ability
	if (!Ability)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
		return;
	}
	// @"Block" and "Cancel": "Ability Tags To Block"/ "Abilit Tags To Cancel" 적용
	{
		ApplyAbilityBlockAndCancelTags(Ability->AbilityTags, Ability, true, FGameplayTagContainer::EmptyContainer, true, FGameplayTagContainer::EmptyContainer);
	}
	// @Activating Abilities: 활성화 목록에 추가
	{
		if (ActivatingAbilityTags.IsEmpty() || !ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.AppendTags(Ability->AbilityTags);
			//UE_LOGFMT(LogMonsterASC, Warning, "{0}가 활성화 목록에 추가되었습니다.", Ability->GetName());
		}
	}
	//UE_LOGFMT(LogMonsterASC, Warning, "{0}가 활성화 되었습니다.", Ability->GetName());

	// @TODO: Ability 활성화 시점에 ASC에서 할 일들...
}

void UBaseMonsterASC::OnAbilityEnded(UGameplayAbility* Ability)
{
	//UE_LOGFMT(LogMonsterASC, Warning, "{0}가 종료되었습니다.", Ability->GetName());
	// @Ability
	if (!Ability)
	{
		//UE_LOGFMT(LogMonsterASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
		return;
	}
	// @Activating Abilities
	{
		if (!ActivatingAbilityTags.IsEmpty() && ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(Ability->AbilityTags);
			//UE_LOGFMT(LogMonsterASC, Warning, "{0}가 활성화 목록에서 제거되었습니다.", Ability->GetName());
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
				//UE_LOGFMT(LogMonsterASC, Warning, "Block 되었던 {0}가 해제되었습니다.", TagsToBlock.ToString());
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

	// @TODO: Ability 활성화 종료 시점에 ASC에서 할 일들...
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
