// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/PlayerAbilitySystemComponent.h"

#include "GameplayTagContainer.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"




void UPlayerAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	AbilityFailedCallbacks.AddUObject(this, &UPlayerAbilitySystemComponent::SaveBlockedAbility);
}

void UPlayerAbilitySystemComponent::OnAbilityEnded(UGameplayAbility* Ability)
{
	Super::OnAbilityEnded(Ability);

	// Passive Ability는 선입력과 관계없기 때문에 조기 Retrun
	const FGameplayTag PassiveTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Passive")));
	if (Ability->AbilityTags.HasTag(PassiveTag))
	{
		return;
	}
	// 저장된 어빌리티가 있다면 실행한다.
	if (SavedAbility)
	{
		const FGameplayTagContainer RequiredTags = Cast<UBaseGameplayAbility>(SavedAbility)->GetRequiredTags();
		AddLooseGameplayTags(RequiredTags);

		TryActivateAbility(SavedAbility->GetCurrentAbilitySpecHandle());

		RemoveLooseGameplayTags(RequiredTags);
		SavedAbility = nullptr;
	}
}

void UPlayerAbilitySystemComponent::SaveBlockedAbility(const UGameplayAbility* BlockedAbility, const FGameplayTagContainer& TagContainer)
{
	// 선입력을 할 수 없거나, Passive Ability는 선입력과 관계없기 때문에 조기 return
	const FGameplayTag PassiveTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Passive")));
	if (bCanSaveBlockedAbility == false)
	{
		return;
	}
	if  (BlockedAbility->AbilityTags.HasTag(PassiveTag))
	{
		return;
	}

	FGameplayTagContainer CurrentOwnedGameplayTags;
	GetOwnedGameplayTags(CurrentOwnedGameplayTags);

	// 엑티브 어빌리티 실행 중 공격 어빌리티 선입력에 대한 처리 (예 : 공격 -> 공격, 닷지 -> 공격 등)
	const FGameplayTag ActiveAttackTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Active.Attack")));
	const FGameplayTag ActiveTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Active")));
	if (BlockedAbility->AbilityTags.HasTag(ActiveAttackTag))
	{
		if (DoesAvatarActorHasActivationRequiredTagsForAbility(BlockedAbility))
		{
			SavedAbility = BlockedAbility;
		}
		return;
	}
	else if (BlockedAbility->AbilityTags.HasTag(ActiveTag))
	{
		SavedAbility = BlockedAbility;
		return;
	}
}

void UPlayerAbilitySystemComponent::EarlyCancelAbility()
{
	// 저장된 어빌리티가 있으면, 현재 실행중인 어빌리티를 찾아 종료함.
	if (SavedAbility)
	{
		FGameplayTagContainer CurrentOwnedGameplayTags;
		GetOwnedGameplayTags(CurrentOwnedGameplayTags);
		CancelAbilities(&CurrentOwnedGameplayTags);
	}
}



bool UPlayerAbilitySystemComponent::DoesAvatarActorHasActivationRequiredTagsForAbility(const UGameplayAbility* BlockedAbility)
{
	const FGameplayTagContainer RequiredTags = Cast<UBaseGameplayAbility>(BlockedAbility)->GetRequiredTags();
	FGameplayTagContainer CurrentOwnedGameplayTags;
	GetOwnedGameplayTags(CurrentOwnedGameplayTags);
	return CurrentOwnedGameplayTags.HasAllExact(RequiredTags);
}


void UPlayerAbilitySystemComponent::SetbCanSaveBlockedAbility(bool bCanSave)
{
	bCanSaveBlockedAbility = bCanSave;

}

