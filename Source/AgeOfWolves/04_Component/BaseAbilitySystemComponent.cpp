#include "BaseAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

DEFINE_LOG_CATEGORY(LogASC)
// UE_LOGFMT(LogASC, Warning, "");

//@Defualt Setting
#pragma region Default Setting
UBaseAbilitySystemComponent::UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	bChainWindowActive = false;
	AllowedChainMappings.Empty();
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
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
FGameplayAbilitySpecHandle UBaseAbilitySystemComponent::GiveAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	UE_LOGFMT(LogASC, Log, "GiveAbility 시작 - Ability: {0}",
		IsValid(AbilitySpec.Ability) ? AbilitySpec.Ability->GetName() : TEXT("Invalid"));

	if (!IsValid(AbilitySpec.Ability))
	{
		UE_LOGFMT(LogASC, Warning, "GiveAbility 실패 - 유효하지 않은 어빌리티");
		return FGameplayAbilitySpecHandle();
	}

	if (!IsOwnerActorAuthoritative())
	{
		UE_LOGFMT(LogASC, Warning, "GiveAbility 실패 - 권한 없음");
		return FGameplayAbilitySpecHandle();
	}

	if (AbilityScopeLockCount > 0)
	{
		UE_LOGFMT(LogASC, Log, "어빌리티 보류 중 - Scope Lock 상태");
		AbilityPendingAdds.Add(AbilitySpec);
		return AbilitySpec.Handle;
	}

	ABILITYLIST_SCOPE_LOCK();
	FGameplayAbilitySpec& OwnedSpec = ActivatableAbilities.Items[ActivatableAbilities.Items.Add(AbilitySpec)];

	if (OwnedSpec.Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
	{
		UE_LOGFMT(LogASC, Log, "어빌리티 인스턴스 생성 - Policy: InstancedPerActor");
		CreateNewInstanceOfAbility(OwnedSpec, AbilitySpec.Ability);
	}

	OnGiveAbility(OwnedSpec);
	MarkAbilitySpecDirty(OwnedSpec, true);

	AbilitySpecGiven.Broadcast(AbilitySpec);

	return OwnedSpec.Handle;
}

void UBaseAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//@Held 입력 처리
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
				if (!AbilitySpec->IsActive())
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

void UBaseAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UBaseAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	UE_LOGFMT(LogASC, Log, "입력된 Input Tag: {0}", *InputTag.ToString());

	//@Chain System 활성화 중이 아니면 일반 입력 처리
	if (!bChainWindowActive || bCanChainAction)
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (!AbilitySpec.Ability || !AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				continue;
			}

			UE_LOGFMT(LogASC, Log, "입력 Tag에 매칭된 어빌리티: {0} | 입력 Tag: {1}",
				*AbilitySpec.Ability->GetName(),
				*InputTag.ToString());

			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
		return;
	}

	//@Chain Action 매칭 시도
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.Ability || !AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		if (!AbilitySpec.Ability->AbilityTags.Num())
		{
			continue;
		}

		FGameplayTag AbilityTag = AbilitySpec.Ability->AbilityTags.First();
		UE_LOGFMT(LogASC, Log, "체인 시스템 - 입력된 어빌리티 태그: {0}", *AbilityTag.ToString());

		for (const auto& Mapping : AllowedChainMappings)
		{
			UE_LOGFMT(LogASC, Log, "체인 매핑 체크 중 - 허용된 다음 어빌리티: {0}",
				*Mapping.AbilityTag.ToString());

			if (!Mapping.Find(AbilityTag))
			{
				continue;
			}

			UE_LOGFMT(LogASC, Log, "체인 액션 매칭 성공 - 어빌리티: {0} | 이벤트 태그: {1}",
				*AbilitySpec.Ability->GetName(),
				*Mapping.EventTag.ToString());

			ChainActionEventTag = Mapping.EventTag;
			bCanChainAction = true;

			//@Immediate 모드일 때만 즉시 EndChainWindow 호출
			if (CurrentChainMode == EChainActionMode::ImmediateActivation)
			{
				UE_LOGFMT(LogASC, Log, "즉시 실행 모드 - 체인 윈도우 종료");
				EndChainWindow();
			}
			return;
		}
	}
}

void UBaseAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	UE_LOGFMT(LogASC, Log, "해제된 Input Tag: {0}", InputTag.ToString());

	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				UE_LOGFMT(LogASC, Log, "입력 해제된 어빌리티: {0} | 해제된 Tag: {1}",
					AbilitySpec.Ability->GetName(),
					InputTag.ToString());

				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UBaseAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	// 로그 추가
	UE_LOGFMT(LogASC, Log, "AbilitySpecInputPressed - Ability: {0}",
		*Spec.Ability->GetName());

	// 부모 클래스의 InputPressed 처리 (여기서 Spec.InputPressed = true 설정)
	Super::AbilitySpecInputPressed(Spec);

}

void UBaseAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	// 로그 추가
	UE_LOGFMT(LogASC, Log, "AbilitySpecInputReleased - Ability: {0}",
		*Spec.Ability->GetName());

	// 부모 클래스의 InputReleased 처리 (여기서 Spec.InputPressed = false 설정)
	Super::AbilitySpecInputReleased(Spec);

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
				if (TryActivateAbility(AbilitySpec.Handle)) UE_LOGFMT(LogASC, Warning, "{0}이 재 활성화 되었습니다!", UnblockedAbilityTag.GetTagName());
			}
			else
			{
				UE_LOGFMT(LogASC, Error, "{0}이 재 활성화에 실패했습니다.", UnblockedAbilityTag.GetTagName());
			}
		}
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

int32 UBaseAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	UE_LOGFMT(LogASC, Log, "GameplayEvent 처리 시작 - EventTag: {0}", *EventTag.ToString());

	// 활성화 가능한 어빌리티 체크
	TArray<FGameplayAbilitySpec> MatchingSpecs = GetActivatableAbilities();

	// 이벤트에 반응할 수 있는 어빌리티들 로깅
	for (const FGameplayAbilitySpec& Spec : MatchingSpecs)
	{
		if (!Spec.Ability)
		{
			continue;
		}

		// 어빌리티의 트리거 체크
		auto BaseGA = Cast<UBaseGameplayAbility>(Spec.Ability);
		if (BaseGA)
		{
			const TArray<FAbilityTriggerData>& TriggerData = BaseGA->AbilityTriggers;
			if (TriggerData.Num() > 0)
			{
				for (const FAbilityTriggerData& Trigger : TriggerData)
				{
					UE_LOGFMT(LogASC, Log, "- 트리거 태그: {0}", *Trigger.TriggerTag.ToString());
					if (Trigger.TriggerTag == EventTag)
					{
						UE_LOGFMT(LogASC, Log, "- 이벤트 태그 매칭 성공: {0}", *EventTag.ToString());
					}
				}
			}
		}
	}

	int32 ActivatedAbilities = Super::HandleGameplayEvent(EventTag, Payload);
	UE_LOGFMT(LogASC, Log, "GameplayEvent 처리 완료 - 활성화된 어빌리티 수: {0}", ActivatedAbilities);

	return ActivatedAbilities;
}

bool UBaseAbilitySystemComponent::TriggerDamageEvent(const FGameplayTag& EventTag, const FGameplayEventData* Payload)
{
	UE_LOGFMT(LogASC, Log, "데미지 이벤트 트리거 시작 - EventTag: {0}", EventTag.ToString());

	if (!EventTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "데미지 이벤트 트리거 실패 - 유효하지 않은 이벤트 태그");
		return false;
	}

	if (!IsOwnerActorAuthoritative())
	{
		UE_LOGFMT(LogASC, Warning, "데미지 이벤트 트리거 실패 - 권한 없음");
		return false;
	}

	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	if (!ActorInfo)
	{
		UE_LOGFMT(LogASC, Warning, "데미지 이벤트 트리거 실패 - ActorInfo가 유효하지 않음");
		return false;
	}

	int32 Count = HandleGameplayEvent(EventTag, Payload);

	UE_LOGFMT(LogASC, Log, "데미지 이벤트 트리거 완료: 활성화된 어빌리티 수 {0}", Count);

	return true;
}

void UBaseAbilitySystemComponent::StartChainWindow()
{
	StartChainWindowWithTag(FGameplayTag());
}

void UBaseAbilitySystemComponent::StartChainWindowWithTag(const FGameplayTag& InAbilityToBindTag)
{
	UE_LOGFMT(LogASC, Log, "체인 윈도우 시작 시도");

	if (InAbilityToBindTag.IsValid())
	{
		//@캐싱
		OriginAbilityTag = InAbilityToBindTag;

		const TArray<FGameplayAbilitySpec>& AbilitySpecs = GetActivatableAbilities();
		const FGameplayAbilitySpec* TargetSpec = nullptr;

		for (const FGameplayAbilitySpec& Spec : AbilitySpecs)
		{
			if (!Spec.Ability || !Spec.Ability->AbilityTags.HasTag(InAbilityToBindTag))
			{
				continue;
			}

			TargetSpec = &Spec;
			break;
		}

		if (!TargetSpec || !TargetSpec->Ability)
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 태그에 해당하는 어빌리티를 찾을 수 없음: {0}",
				*InAbilityToBindTag.ToString());
			EndChainWindow();
			return;
		}

		auto BaseGA = Cast<UBaseGameplayAbility>(TargetSpec->Ability);
		if (!BaseGA)
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 찾은 어빌리티가 BaseGameplayAbility가 아님");
			EndChainWindow();
			return;
		}

		if (!BaseGA->bUseChainSystem)
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 체인 시스템 미사용 어빌리티: {0}",
				*BaseGA->GetName());
			EndChainWindow();
			return;
		}

		CurrentChainMode = BaseGA->GetChainActionMode();
		ChainActionActivated.BindUFunction(BaseGA, "OnChainActionActivated");
		bChainWindowActive = true;
		AllowedChainMappings = BaseGA->GetChainActionMappings();
		bCanChainAction = false;

		UE_LOGFMT(LogASC, Log, "체인 윈도우 시작 성공 - 태그로 찾은 어빌리티: {0} | 체인 모드: {1}",
			*BaseGA->GetName(),
			CurrentChainMode == EChainActionMode::DelayedActivation ? TEXT("Delayed") : TEXT("Immediate"));
		return;
	}

	auto BaseGA = Cast<UBaseGameplayAbility>(GetAnimatingAbility());
	if (!BaseGA)
	{
		UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 현재 실행 중인 어빌리티가 없거나 BaseGameplayAbility가 아님");
		EndChainWindow();
		return;
	}

	if (!BaseGA->bUseChainSystem)
	{
		UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 체인 시스템 미사용 어빌리티: {0}",
			*BaseGA->GetName());
		EndChainWindow();
		return;
	}

	CurrentChainMode = BaseGA->GetChainActionMode();
	ChainActionActivated.BindUFunction(BaseGA, "OnChainActionActivated");
	bChainWindowActive = true;
	AllowedChainMappings = BaseGA->GetChainActionMappings();
	bCanChainAction = false;

	UE_LOGFMT(LogASC, Log, "체인 윈도우 시작 성공 - 애니메이팅 어빌리티: {0} | 체인 모드: {1}",
		*BaseGA->GetName(),
		CurrentChainMode == EChainActionMode::DelayedActivation ? TEXT("Delayed") : TEXT("Immediate"));
}

void UBaseAbilitySystemComponent::EndChainWindow()
{
	if (!bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우가 활성화되지 않은 상태에서 종료 시도");
		return;
	}

	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 시작");

	//@체인 액션 실행
	if (bCanChainAction && ChainActionEventTag.IsValid())
	{
		if (!ChainActionActivated.IsBound())
		{
			UE_LOGFMT(LogASC, Warning, "체인 액션 실행 실패 - 바인딩된 함수 없음");
		}
		else
		{
			auto GA = Cast<UGameplayAbility>(ChainActionActivated.GetUObject());
			if (!GA)
			{
				UE_LOGFMT(LogASC, Warning, "체인 액션 실행 실패 - 유효하지 않은 GameplayAbility");
			}
			else if (!GA->IsActive())
			{
				UE_LOGFMT(LogASC, Warning, "체인 액션 실행 실패 - 비활성화된 {0}", *GA->GetName());
			}
			else if (!GA->GetCurrentActorInfo())
			{
				UE_LOGFMT(LogASC, Warning, "체인 액션 실행 실패 - 유효하지 않은 ActorInfo");
			}
			else
			{
				UE_LOGFMT(LogASC, Log, "체인 액션 실행 - 이벤트 태그: {0} | 체인 모드: {1}",
					*ChainActionEventTag.ToString(),
					TEXT("Delayed"));

				ChainActionActivated.ExecuteIfBound(ChainActionEventTag);
			}
		}
	}

	ChainActionActivated.Clear();
	bChainWindowActive = false;
	bCanChainAction = false;
	AllowedChainMappings.Empty();
	ChainActionEventTag = FGameplayTag();
	CurrentChainMode = EChainActionMode::DelayedActivation;

	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 완료");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
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
		if (ActivatingAbilityTags.IsEmpty() || !ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
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
	//@Ability
	if (!Ability)
	{
		UE_LOGFMT(LogASC, Error, "{0}가 유효하지 않습니다", Ability->GetName());
		return;
	}

	//@Activating Abilities
	if (!ActivatingAbilityTags.IsEmpty() && ActivatingAbilityTags.HasAllExact(Ability->AbilityTags))
	{
		ActivatingAbilityTags.RemoveTags(Ability->AbilityTags);
		UE_LOGFMT(LogASC, Warning, "{0}가 활성화 목록에서 제거되었습니다.", Ability->GetName());
	}

	//@UnBlock
	if (AbilityTagRelationshipMapping)
	{
		FGameplayTagContainer TagsToBlock;
		FGameplayTagContainer DummyContainer;

		AbilityTagRelationshipMapping->GetAbilityTagsToBlockAndCancel(Ability->AbilityTags, &TagsToBlock, &DummyContainer);

		for (auto Blocked : TagsToBlock)
		{
			UE_LOGFMT(LogASC, Error, "{0} : 블록된 태그 목록", Blocked.GetTagName().ToString());
		}

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
			if (!TagsToReactivate.IsEmpty()) ReactivateUnblockedPassiveAbility(TagsToReactivate);
		}
	}

	UE_LOGFMT(LogASC, Warning, "{0}가 종료되었습니다.", Ability->GetName());

	// @TODO: Ability 활성화 종료 시점에 ASC에서 할 일들...

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void UBaseAbilitySystemComponent::GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel)
{
	if (AbilityTagRelationshipMapping.Get())
	{
		AbilityTagRelationshipMapping.Get()->GetAbilityTagsToBlockAndCancel(AbilityTags, &OutAbilityTagsToBlock, &OutAbilityTagsToCancel);
	}
}

void UBaseAbilitySystemComponent::GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{

	if (AbilityTagRelationshipMapping)
	{
		AbilityTagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, OutActivationRequired, OutActivationBlocked);
	}
}
#pragma endregion