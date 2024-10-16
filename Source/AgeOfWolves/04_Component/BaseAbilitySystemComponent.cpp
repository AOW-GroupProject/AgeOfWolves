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
				// if (!TagsToReactivate.IsEmpty()) ReactivateUnblockedPassiveAbility(TagsToReactivate);
			}
		}
	}

	// @TODO: Ability 활성화 종료 시점에 ASC에서 할 일들...

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

	if (AbilityTagRelationshipMapping)
	{
		AbilityTagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, OutActivationRequired, OutActivationBlocked);
	}
}

void UBaseAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer AbilityTagsToBlock = BlockTags;
	FGameplayTagContainer AbilityTagsToCancel = CancelTags;

	if (AbilityTagRelationshipMapping)
	{
		AbilityTagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &AbilityTagsToBlock, &AbilityTagsToCancel);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, AbilityTagsToBlock, bExecuteCancelTags, AbilityTagsToCancel);
}
#pragma endregion


