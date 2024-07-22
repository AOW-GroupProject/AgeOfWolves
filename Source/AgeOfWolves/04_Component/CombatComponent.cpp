// Fill out your copyright notice in the Description page of Project Settings.
#include "04_Component/CombatComponent.h"


#include "01_Character/PlayerCharacter.h"
#include "04_Component/CombatComponent.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"


#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"

UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	BaseAbilitySystemComponent = GetAbilitysystemComponent();
	// BaseAbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &UCombatComponent::QueueBlockedAbility);
	// BaseAbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &UCombatComponent::CheckQueuedAbility);
}

int UCombatComponent::GetInputCount()
{
	return InputCount;
}

void UCombatComponent::IncrementCombo()
{
	if (bComboWindowOpen)
	{
		ComboIndex = 1;
	}
}

void UCombatComponent::ResetCombo()
{
	ComboIndex = 0;
	InputCount = 0;
}

void UCombatComponent::ActivateComboAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	UBaseAbilitySystemComponent* ASC = GetAbilitysystemComponent();
	if (bComboWindowOpen && ComboIndex > 0) // Combo�� �����Ҷ�
	{

	}

	ASC->TryActivateAbilityByClass(AbilityClass);
}

bool UCombatComponent::CanComboAttack()
{
	if (ComboIndex > 0)
	{
		InputCount++;
		return true;
	}
	return false;
}

UBaseAbilitySystemComponent* UCombatComponent::GetAbilitysystemComponent()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);
	APlayerStateBase* PlayerStateBase = Cast<APlayerStateBase>(PlayerCharacter->GetPlayerState());
	check(PlayerStateBase);
	UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(PlayerStateBase->GetAbilitySystemComponent());
	check(BaseASC);
	return BaseASC;
}

void UCombatComponent::OpenComboWindow()
{
	bComboWindowOpen = true;
}

void UCombatComponent::CloseComboWindow()
{
	bComboWindowOpen = false;
}

bool UCombatComponent::GetbComboWindowOpen()
{
	return bComboWindowOpen;
}





#pragma region Input Queue Management


void UCombatComponent::QueueBlockedAbility(const UGameplayAbility* BlockedAbility, const FGameplayTagContainer& TagContainer)
{
	UE_LOG(LogTemp, Warning, TEXT("QueueBlockedAbility"));
	UE_LOG(LogTemp, Warning, TEXT("BlockedAbility : %s"), *BlockedAbility->GetName());
	// Handle�� ���� AbilitySpec�� �˻��ϰ� QueuedAbility�� �����մϴ�.
	if (BaseAbilitySystemComponent)
	{
		QueuedAbility = BlockedAbility;
	}
}

void UCombatComponent::CheckQueuedAbility(UGameplayAbility* CurrentActivatedAbility)
{

	if (BaseAbilitySystemComponent && QueuedAbility)
	{
		BaseAbilitySystemComponent->TryActivateAbilityByClass(QueuedAbility->GetClass());
	}
	/*
		if (BaseAbilitySystemComponent)
	{
		if (QueuedAbility)
		{
			const FGameplayAbilitySpec* BlockedAbilitySpec = BaseAbilitySystemComponent->FindAbilitySpecFromClass(QueuedAbility->GetClass());
			QueuedSpecHandles = BlockedAbilitySpec->Handle;
			UE_LOG(LogTemp, Warning, TEXT("BlockedAbility : %s"), *QueuedAbility->GetName());
			BaseAbilitySystemComponent->TryActivateAbility(QueuedSpecHandles);
			QueuedAbility = nullptr;
		}

	}
	
	*/

}

void UCombatComponent::UpdateAllowedInputTags(TArray<FGameplayTag> InputTags)
{
	AllowedInputTags = InputTags;

}

void UCombatComponent::TryActivateQueuedAbility()
{

}
void UCombatComponent::ResetAllowedInputTags()
{
	AllowedInputTags.Empty();
}

#pragma endregion


