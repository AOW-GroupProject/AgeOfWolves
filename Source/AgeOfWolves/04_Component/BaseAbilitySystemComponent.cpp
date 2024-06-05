#include "BaseAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogASC)
// UE_LOGFMT(LogASC, Warning, "");

UBaseAbilitySystemComponent::UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AbilityActivatedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityActivated);
	AbilityEndedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityEnded);

}

bool UBaseAbilitySystemComponent::TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	// @Ability Spec Handle
	if (AbilityToActivate.IsValid() == false)
	{
		UE_LOGFMT(LogASC, Warning, "InternalTryActivateAbility 에서 유효하지 않은 Ability Spec Handle! ASC: {0}. AvatarActor: {1}", GetPathName(), GetNameSafe(GetAvatarActor_Direct()));
		return false;
	}

	// @Ability Spec
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityToActivate);
	if (!Spec)
	{
		UE_LOGFMT(LogASC, Error, "TryActivateAbility는 유효하지 않은 Handle을 호출했습니다.");
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
		UE_LOGFMT(LogASC, Error, "TryActivateAbility는 유효하지 않은 GA를 활용했습니다.");
		return false;
	}

	UBaseGameplayAbility* InstancedAbility = Cast<UBaseGameplayAbility>(Spec->GetPrimaryInstance());

	// @ATMR: Ability Tag간 관계성: "Block", "AR", "AB"
	{
		UBaseGameplayAbility* const CanActivateAbilitySource = InstancedAbility ? InstancedAbility : Ability;

		if (AbilityTagRelationshipMapping.Get())
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
				UE_LOGFMT(LogASC, Error, "Can't activate instanced per actor ability %s when their is already a currently active instance for this actor.", Ability->GetName());
				return false;
			}
		}
	}

	// @Ability Spec
	Spec->ActivationInfo = FGameplayAbilityActivationInfo(ActorInfo->OwnerActor.Get());
	FGameplayAbilityActivationInfo& ActivationInfo = Spec->ActivationInfo;

	// @Call Activate Ability
	if (Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerExecution)
	{
		InstancedAbility = Cast<UBaseGameplayAbility>(CreateNewInstanceOfAbility(*Spec, Ability));
		InstancedAbility->CallActivateAbility(AbilityToActivate, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	}

	if (InstancedAbility)
	{
		if (OutInstancedAbility)
		{
			*OutInstancedAbility = InstancedAbility;
		}
		InstancedAbility->SetCurrentActivationInfo(ActivationInfo);	
	}

	return false;
}

void UBaseAbilitySystemComponent::CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore)
{
	UGameplayAbility* GA = Spec.Ability; 
	if (!GA)
	{
		UE_LOGFMT(LogASC, Error, "Ability Spec에 Ability 정보가 없습니다!");
		return;
	}

	// @Activating Abilities: 활성화 목록에서 제거
	if (ActivatingAbilityTags.HasAllExact(GA->AbilityTags))
	{
		ActivatingAbilityTags.RemoveTags(GA->AbilityTags);
		UE_LOGFMT(LogASC, Warning, "{0}이 취소되었습니다.", GA->GetName());
	}

	Super::CancelAbilitySpec(Spec, Ignore);
}

void UBaseAbilitySystemComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
	// @Ability
	if (!Ability)
	{
		UE_LOGFMT(LogASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
		return;
	}

	// @"Block" and "Cancel": "Ability Tags To Block"/ "Abilit Tags To Cancel" 적용
	ApplyAbilityBlockAndCancelTags(Ability->AbilityTags, Ability, true, FGameplayTagContainer::EmptyContainer, true, FGameplayTagContainer::EmptyContainer);

	// @Activating Abilities: 활성화 목록에 추가
	if(ActivatingAbilityTags.IsEmpty() || !ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
	{
		ActivatingAbilityTags.AppendTags(Ability->AbilityTags);
		UE_LOGFMT(LogASC, Warning, "{0}가 활성화 목록에 추가되었습니다.", Ability->GetName());
	}

	UE_LOGFMT(LogASC, Warning, "{0}가 활성화 되었습니다.", Ability->GetName());

	// @TODO: Ability 활성화 시점에 ASC에서 할 일들...

}

void UBaseAbilitySystemComponent::OnAbilityEnded(UGameplayAbility* Ability)
{
	// @Ability
	{
		if (!Ability)
		{
			UE_LOGFMT(LogASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
			return;
		}
	}
	// @UnBlock: "Block" 했던 태그들 삭제
	{
		if (AbilityTagRelationshipMapping)
		{
			FGameplayTagContainer TagsToBlock;
			FGameplayTagContainer DummyContainer = FGameplayTagContainer::EmptyContainer;

			AbilityTagRelationshipMapping->GetAbilityTagsToBlockAndCancel(Ability->AbilityTags, &TagsToBlock, &DummyContainer);

			if (!TagsToBlock.IsEmpty())
			{
				UnBlockAbilitiesWithTags(TagsToBlock);
			}
			
			UE_LOGFMT(LogASC, Warning, "Block 되었던 {0}가 해제되었습니다.", TagsToBlock.ToString());
		}
	}
	// @Activating Abilities
	{
		if (!ActivatingAbilityTags.IsEmpty() && ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(Ability->AbilityTags);
			UE_LOGFMT(LogASC, Warning, "{0}가 활성화 목록에서 제거되었습니다.", Ability->GetName());
		}
	}

	UE_LOGFMT(LogASC, Warning, "{0}가 종료되었습니다.", Ability->GetName());

	// @TODO: Ability 활성화 종료 시점에 ASC에서 할 일들...

}

void UBaseAbilitySystemComponent::GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel)
{
	if (AbilityTagRelationshipMapping.Get())
	{
		AbilityTagRelationshipMapping.Get()->GetAbilityTagsToBlockAndCancel(AbilityTags, &OutAbilityTagsToBlock, &OutAbilityTagsToCancel);
	}
}

void UBaseAbilitySystemComponent::GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutActivationRequired, OUT FGameplayTagContainer& OutActivationBlocked) const
{
	//check(AbilityTagRelationship)

	if (AbilityTagRelationshipMapping.Get())
	{
		AbilityTagRelationshipMapping.Get()->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UBaseAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer AbilityTagsToBlock;
	FGameplayTagContainer AbilityTagsToCancel;

	if (AbilityTagRelationshipMapping)
	{
		AbilityTagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &AbilityTagsToBlock, &AbilityTagsToCancel);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, AbilityTagsToBlock, bExecuteCancelTags, AbilityTagsToCancel);
}

void UBaseAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//
	// Process all abilities that activate when the input is held.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UBaseGameplayAbility* LyraAbilityCDO = CastChecked<UBaseGameplayAbility>(AbilitySpec->Ability);

				if (LyraAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//
	// Process all abilities that had their input pressed this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UBaseGameplayAbility* LyraAbilityCDO = CastChecked<UBaseGameplayAbility>(AbilitySpec->Ability);

					if (LyraAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	/*
	*/
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			if (UGameplayAbility* Ability = AbilitySpec->Ability)
			{
				// @설명: 활성화 시도
				TryActivateAbility(AbilitySpecHandle);
			}
		}
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UBaseAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

