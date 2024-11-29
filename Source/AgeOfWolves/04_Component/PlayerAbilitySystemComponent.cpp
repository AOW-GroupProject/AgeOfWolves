// Fill out your copyright notice in the Description page of Project Settings.

#include "04_Component/PlayerAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "GameplayTagContainer.h"
#include "02_AbilitySystem/AOWGameplayTags.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogPlayerASC)


void UPlayerAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	AbilityFailedCallbacks.AddUObject(this, &UPlayerAbilitySystemComponent::SaveBlockedAbility);
}

#pragma region Active GA
UPlayerAbilitySystemComponent::UPlayerAbilitySystemComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UPlayerAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// @Held 입력 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UBaseGameplayAbility* BaseAbilityCDO = Cast<UBaseGameplayAbility>(AbilitySpec->Ability);
				if (BaseAbilityCDO)
				{
					if (BaseAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// @Press입력 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
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
					const UBaseGameplayAbility* BaseAbilityCDO = Cast<UBaseGameplayAbility>(AbilitySpec->Ability);
					if (BaseAbilityCDO && BaseAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
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

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UPlayerAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UPlayerAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
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

void UPlayerAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
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

void UPlayerAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	// 로그 추가
	UE_LOGFMT(LogPlayerASC, Log, "AbilitySpecInputPressed - Ability: {0}",
		*Spec.Ability->GetName());

	// 부모 클래스의 InputPressed 처리 (여기서 Spec.InputPressed = true 설정)
	Super::AbilitySpecInputPressed(Spec);

}

void UPlayerAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	// 로그 추가
	UE_LOGFMT(LogPlayerASC, Log, "AbilitySpecInputReleased - Ability: {0}",
		*Spec.Ability->GetName());

	// 부모 클래스의 InputReleased 처리 (여기서 Spec.InputPressed = false 설정)
	Super::AbilitySpecInputReleased(Spec);

}
#pragma endregion

#pragma region Player Input Buffer
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
#pragma endregion
