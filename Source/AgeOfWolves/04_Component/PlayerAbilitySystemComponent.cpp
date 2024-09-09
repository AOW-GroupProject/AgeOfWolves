// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/PlayerAbilitySystemComponent.h"

#include "GameplayTagContainer.h"
#include "02_AbilitySystem/AOWGameplayTags.h"




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
		TryActivateAbility(SavedAbility->GetCurrentAbilitySpecHandle());
		SavedAbility = nullptr;
	}
}

void UPlayerAbilitySystemComponent::SaveBlockedAbility(const UGameplayAbility* BlockedAbility, const FGameplayTagContainer& TagContainer)
{
	if (bCanSaveBlockedAbility == false)
	{
		return;
	}

	// Passive Ability는 선입력과 관계없기 때문에 조기 Return
	const FGameplayTag PassiveTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Passive")));
	if (BlockedAbility->AbilityTags.HasTag(PassiveTag))
	{
		return;
	}
	FGameplayTagContainer CurrentOwnedGameplayTags;
	GetOwnedGameplayTags(CurrentOwnedGameplayTags);

	// 공격 어빌리티 실행 중 닷지 어빌리티 실행 시 저장
	const FGameplayTag NormalAttackascendantTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Active.Attack")));
	if (CurrentOwnedGameplayTags.HasTag(NormalAttackascendantTag))
	{
		if (BlockedAbility->AbilityTags.HasTag(AOWGameplayTags::Ability_Active_Dodge))
		{
			SavedAbility = BlockedAbility;
			return;
		}
	}
	
	// 좌클릭 입력시 다음으로 올 수 있는 능력만 저장 
	if (CurrentOwnedGameplayTags.HasTagExact(AOWGameplayTags::Ability_Ready_Attack_Normal_2))
	{
		if (BlockedAbility->AbilityTags.HasTagExact(AOWGameplayTags::Ability_Active_Attack_Normal_2))
		{
			SavedAbility = BlockedAbility;
		}
		return;
	}
	else if (CurrentOwnedGameplayTags.HasTagExact(AOWGameplayTags::Ability_Ready_Attack_Normal_3))
	{
		if (BlockedAbility->AbilityTags.HasTagExact(AOWGameplayTags::Ability_Active_Attack_Normal_3))
		{
			SavedAbility = BlockedAbility;
		}		
		return;
	}
	else if (CurrentOwnedGameplayTags.HasTagExact(AOWGameplayTags::Ability_Ready_Attack_Normal_4))
	{
		if (BlockedAbility->AbilityTags.HasTagExact(AOWGameplayTags::Ability_Active_Attack_Normal_4))
		{
			SavedAbility = BlockedAbility;
		}
		return;
	}
	else if (CurrentOwnedGameplayTags.HasTagExact(AOWGameplayTags::Ability_Ready_Attack_Normal_4))
	{
		if (BlockedAbility->AbilityTags.HasTagExact(AOWGameplayTags::Ability_Active_Attack_Normal_4))
		{
			SavedAbility = BlockedAbility;
		}
		return;
	}
	else if (CurrentOwnedGameplayTags.HasTagExact(AOWGameplayTags::Ability_Ready_Attack_DodgeAttack))
	{
		if (BlockedAbility->AbilityTags.HasTagExact(AOWGameplayTags::Ability_Active_Attack_DodgeAttack))
		{
			SavedAbility = BlockedAbility;
		}
		return;
	}
	SavedAbility = BlockedAbility;
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
void UPlayerAbilitySystemComponent::SetbCanSaveBlockedAbility(bool bCanSave)
{
	bCanSaveBlockedAbility = bCanSave;

}