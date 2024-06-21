// Fill out your copyright notice in the Description page of Project Settings.
#include "04_Component/CombatComponent.h"


#include "01_Character/PlayerCharacter.h"
#include "04_Component/CombatComponent.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	// BaseAbilitySystemComponent = GetOwner()->GetComponentByClass<UBaseAbilitySystemComponent>();
	// ...
	
}



// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
	if (bComboWindowOpen && ComboIndex > 0) // Combo가 가능할때
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


void UCombatComponent::InputQueueMasterEvent()
{





}

void UCombatComponent::UpdateAllowedInputTags(TArray<FGameplayTag> InputTags)
{
	AllowedInputTags = InputTags;

}

void UCombatComponent::ResetAllowedInputTags()
{
	AllowedInputTags.Empty();
}













#pragma endregion


