#include "BaseAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogASC)
// UE_LOGFMT(LogASC, Warning, "");

#pragma region Default Setting
UBaseAbilitySystemComponent::UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	InputTriggeredSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// @Ability 생명 주기 이벤트에 커스텀 콜백 함수 등록
	{
		AbilityActivatedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityActivated);
		AbilityEndedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityEnded);
	}
}

bool UBaseAbilitySystemComponent::TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("베이스 컴포넌트의 TryACtivateAbility"));
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

	// @ATMR: Ability Tag간 관계성: "Block", "AR", "AB"
	UBaseGameplayAbility* InstancedAbility = Cast<UBaseGameplayAbility>(Spec->GetPrimaryInstance());
	UBaseGameplayAbility* const CanActivateAbilitySource = InstancedAbility ? InstancedAbility : Ability;
	{

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

void UBaseAbilitySystemComponent::CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore)
{

	//UGameplayAbility* GA = Spec.Ability; 
	//if (!GA)
	//{
	//	UE_LOGFMT(LogASC, Error, "Ability Spec에 Ability 정보가 없습니다!");
	//	return;
	//}
	//// @Activating Abilities: 활성화 목록에서 제거
	//{
	//	if (ActivatingAbilityTags.HasAllExact(GA->AbilityTags))
	//	{
	//		ActivatingAbilityTags.RemoveTags(GA->AbilityTags);
	//		UE_LOGFMT(LogASC, Warning, "{0}이 취소되었습니다.", GA->GetName());
	//	}
	//}
	//// @GE 제거(Cost, Cooldown 제외)
	//{
	//	if (const auto BaseGA = CastChecked<UBaseGameplayAbility>(GA))
	//	{
	//		if (Spec.Handle.IsValid())
	//		{
	//			FActiveGameplayEffectHandle ActiveGEHandle = FindActiveGameplayEffectHandle(Spec.Handle);
	//			const FActiveGameplayEffect* ActiveGE = GetActiveGameplayEffect(ActiveGEHandle);

	//			if (ActiveGE && ActiveGE->Spec.Def && ActiveGE->Spec.Def->GetClass() == BaseGA->ApplyGameplayEffectClass)
	//			{
	//				RemoveActiveGameplayEffect(ActiveGEHandle);
	//				UE_LOGFMT(LogASC, Warning, "{0} 게임플레이 이펙트가 제거되었습니다.", *BaseGA->ApplyGameplayEffectClass->GetName());
	//			}
	//		}
	//	}
	//}

	UGameplayAbility* GA = Spec.Ability; 
	if (!GA)
	{
		UE_LOGFMT(LogASC, Error, "Ability Spec에 Ability 정보가 없습니다!");
		return;
	}
	// @Activating Abilities: 활성화 목록에서 제거
	{
		if (ActivatingAbilityTags.HasAllExact(GA->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(GA->AbilityTags);
			UE_LOGFMT(LogASC, Warning, "{0}이 취소되었습니다.", GA->GetName());
		}
	}

	Super::CancelAbilitySpec(Spec, Ignore);

}

void UBaseAbilitySystemComponent::ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags)
{
	for (const auto UnblockedAbilityTag : UnblockedAbilityTags)
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (!AbilitySpec.IsActive() && IsValid(AbilitySpec.Ability) && AbilitySpec.Handle.IsValid()
				&& AbilitySpec.Ability->AbilityTags.HasTagExact(UnblockedAbilityTag))
			{
				if(TryActivateAbility(AbilitySpec.Handle)) UE_LOGFMT(LogASC, Warning, "{0}이 재 활성화 되었습니다!", UnblockedAbilityTag.GetTagName());
			}
			else
			{
				 UE_LOGFMT(LogASC, Error, "{0}이 재 활성화에 실패했습니다.", UnblockedAbilityTag.GetTagName());
			}
		}
	}

}
#pragma endregion

#pragma region GA Life Span
void UBaseAbilitySystemComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
	// @Ability
	if (!Ability)
	{
		UE_LOGFMT(LogASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
		return;
	}
	// @"Block" and "Cancel": "Ability Tags To Block"/ "Abilit Tags To Cancel" 적용
	{
		ApplyAbilityBlockAndCancelTags(Ability->AbilityTags, Ability, true, FGameplayTagContainer::EmptyContainer, true, FGameplayTagContainer::EmptyContainer);
	}
	// @Activating Abilities: 활성화 목록에 추가
	{
		if(ActivatingAbilityTags.IsEmpty() || !ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.AppendTags(Ability->AbilityTags);
			UE_LOGFMT(LogASC, Warning, "{0}가 활성화 목록에 추가되었습니다.", Ability->GetName());
		}
	}
	UE_LOGFMT(LogASC, Warning, "{0}가 활성화 되었습니다.", Ability->GetName());

	// @TODO: Ability 활성화 시점에 ASC에서 할 일들...

}

void UBaseAbilitySystemComponent::OnAbilityEnded(UGameplayAbility* Ability)
{

	UE_LOGFMT(LogASC, Warning, "{0}가 종료되었습니다.", Ability->GetName());
	// @Ability
	if (!Ability)
	{
		UE_LOGFMT(LogASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
		return;
	}
	// @Activating Abilities
	{
		if (!ActivatingAbilityTags.IsEmpty() && ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
		{
			ActivatingAbilityTags.RemoveTags(Ability->AbilityTags);
			UE_LOGFMT(LogASC, Warning, "{0}가 활성화 목록에서 제거되었습니다.", Ability->GetName());
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
				UE_LOGFMT(LogASC, Warning, "Block 되었던 {0}가 해제되었습니다.", TagsToBlock.ToString());
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
#pragma endregion

#pragma region Active GA
void UBaseAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// @InputTriggered
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputTriggeredSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
				// @InputPressed + 다중 키 입력
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				// @InputPressed 활성화
				else
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			if (UGameplayAbility* Ability = AbilitySpec->Ability)
			{
				TryActivateAbility(AbilitySpecHandle);
			}
		}
	}

	// @InputReleased
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				// @InputRelased 활성화
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputTriggeredSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::ClearAbilityInput()
{
	InputTriggeredSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::AbilityInputTagTriggered(const FGameplayTag& InputTag)
{
	// @InputPressed+InputHeld
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputTriggeredSpecHandles.AddUnique(AbilitySpec.Handle);
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
			}
		}
	}
}

void UBaseAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	// @Spec의 InputPressed 값 변경(참), GA의 InputPressed 호출
	Super::AbilitySpecInputPressed(Spec);
}

void UBaseAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	// @Spec의 InputPressed 값 변경(거짓), GA의 InputReleased 호출
	Super::AbilitySpecInputReleased(Spec);
}
#pragma endregion

#pragma region Gameplay Tag Relationship Mapping
void UBaseAbilitySystemComponent::GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel)
{
	if (AbilityTagRelationshipMapping.Get())
	{
		AbilityTagRelationshipMapping.Get()->GetAbilityTagsToBlockAndCancel(AbilityTags, &OutAbilityTagsToBlock, &OutAbilityTagsToCancel);
	}
}

void UBaseAbilitySystemComponent::GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	//check(AbilityTagRelationship)

	if (AbilityTagRelationshipMapping->IsValidLowLevel())
	{
		AbilityTagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, OutActivationRequired, OutActivationBlocked);
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
#pragma endregion


