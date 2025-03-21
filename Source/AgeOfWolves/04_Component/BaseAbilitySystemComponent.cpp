#include "BaseAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/AIAbilitySequencerComponent.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogASC)

//@Defualt Setting
#pragma region Default Setting
UBaseAbilitySystemComponent::UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//@Input Handles...
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	//@Chain Window 활성화
	bChainWindowActive = false;
	//@Chain Mapping 목록
	AllowedChainMappings.Empty();

	//@상호작용 시스템 초기화
	bInteractionAvailable = false;
	InteractionTargetActor = nullptr;
}

void UBaseAbilitySystemComponent::ExternalBindToAIAbilitySequencer(ABaseAIController* BaseAIC)
{
	//@AIC
	if (!BaseAIC)
	{
		UE_LOGFMT(LogASC, Warning, "바인딩 실패: AI 컨트롤러가 유효하지 않음");
		return;
	}

	//@AI Sequencer
	auto AISequencer = BaseAIC->FindComponentByClass<UAIAbilitySequencerComponent>();
	if (!AISequencer)
	{
		UE_LOGFMT(LogASC, Warning, "바인딩 실패: AI Ability Sequencer 컴포넌트를 찾을 수 없음");
		return;
	}

	//@어빌리티 활성화 요청 이벤트 바인딩
	AISequencer->RequestActivateAbilityBlockUnit.BindUFunction(this, "OnRequestActivateAbilityBlockUnitByAI");

	UE_LOGFMT(LogASC, Log, "AI Ability Sequencer 컴포넌트와 바인딩 완료");
}

void UBaseAbilitySystemComponent::ExternalBindToInteractionComp(AController* Controller)
{
	auto PC = Cast<ABasePlayerController>(Controller);
	if (!PC)
	{
		UE_LOGFMT(LogASC, Warning, "{0}: 상호작용 컴포넌트 바인딩 실패: 컨트롤러가 BasePlayerController가 아님", __FUNCDNAME__);
		return;
	}

	auto InteractionComp = PC->GetInteractionComponent();
	if (!InteractionComp)
	{
		UE_LOGFMT(LogASC, Warning, "{0}: 상호작용 컴포넌트 바인딩 실패: 유효한 InteractionComponent가 없음", __FUNCDNAME__);
		return;
	}

	//@외부 바인딩...
	InteractionComp->PotentialInteractionChanged.AddDynamic(this, &UBaseAbilitySystemComponent::OnPotentialInteractionChanged);

	UE_LOGFMT(LogASC, Log, "{0}: InteractionComponent와 바인딩 성공 - PC: {1}",
		__FUNCDNAME__, *PC->GetName());
}

void UBaseAbilitySystemComponent::InitializeComponent()
{

	// @Ability 생명 주기 이벤트에 커스텀 콜백 함수 등록
	//@어빌리티 활성화 이벤트
	AbilityActivatedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityActivated);
	//@어빌리티 활성화 종료 이베느
	AbilityEndedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityEnded);
	//@GameplayEffect 적용 이벤트
	OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		this,
		&UBaseAbilitySystemComponent::OnGameplayEffectApplied
	);

	Super::InitializeComponent();
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
	//@Input Tag
	if (!InputTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "입력 Tag가 유효하지 않음");
		return;
	}

	UE_LOGFMT(LogASC, Log, "입력된 Input Tag: {0}", *InputTag.ToString());

	//@상호작용
	if (bInteractionAvailable && InteractionTargetActor.IsValid())
	{
		if (CurrentPotentialInteraction.InputTag == InputTag)
		{
			UE_LOGFMT(LogASC, Log, "상호작용 입력 태그 매칭 성공 - 입력: {0}",
				*InputTag.ToString());

			//@상호 작용 성공
			EndInteractionWindow(true);
			return;
		}
	}

	//@Chain System 활성화 중이 아니면 일반 입력 처리
	if (!bChainWindowActive || bCanChainAction)
	{
		UE_LOGFMT(LogASC, Log, "일반 입력 처리 모드 - Chain Window Active: {0}, Can Chain Action: {1}",
			bChainWindowActive, bCanChainAction);

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

	UE_LOGFMT(LogASC, Log, "체인 액션 매칭 시도 시작 - Input Tag: {0}", *InputTag.ToString());

	//@Chain Action 매칭 시도
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.Ability || !AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		if (!AbilitySpec.Ability->AbilityTags.Num())
		{
			UE_LOGFMT(LogASC, Log, "어빌리티 태그가 없음 - Ability: {0}", *AbilitySpec.Ability->GetName());
			continue;
		}

		FGameplayTag AbilityTag = AbilitySpec.Ability->AbilityTags.First();
		UE_LOGFMT(LogASC, Log, "체인 시스템 - 입력된 어빌리티 태그: {0}", *AbilityTag.ToString());

		for (const auto& Mapping : AllowedChainMappings)
		{
			if (!Mapping.Find(AbilityTag))
			{
				UE_LOGFMT(LogASC, Error, "체인 매핑에서 매칭 실패 - Ability Tag: {0}", *AbilityTag.ToString());
				continue;
			}

			UE_LOGFMT(LogASC, Log, "체인 매핑 찾음 - Ability Tag: {0}, Event Tag: {1}",
				*AbilityTag.ToString(), *Mapping.EventTag.ToString());

			// OriginAbilityTag에 해당하는 활성화된 어빌리티 찾기
			for (const FGameplayAbilitySpec& OriginSpec : GetActivatableAbilities())
			{
				if (OriginSpec.Ability && OriginSpec.IsActive() &&
					OriginSpec.Ability->AbilityTags.HasTag(OriginAbilityTag))
				{
					UE_LOGFMT(LogASC, Log, "원본 어빌리티 찾음 - Ability: {0}", *OriginSpec.Ability->GetName());

					if (auto OriginInstance = OriginSpec.GetPrimaryInstance())
					{
						ChainActionActivated.BindUFunction(OriginInstance, "OnChainActionActivated");
						ChainActionFinished.BindUFunction(OriginInstance, "OnChainActionFinished");
						UE_LOGFMT(LogASC, Log, "체인 액션 델리게이트 바인딩 완료");
					}
					break;
				}
			}

			ChainActionEventTag = Mapping.EventTag;
			bCanChainAction = true;

			if (CurrentChainMode == EChainActionMode::ImmediateActivation)
			{
				UE_LOGFMT(LogASC, Log, "즉시 활성화 모드로 체인 윈도우 종료");
				EndChainWindow();
			}
			return;
		}
	}

	UE_LOGFMT(LogASC, Log, "매칭되는 체인 액션을 찾지 못함 - Input Tag: {0}", *InputTag.ToString());
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

			//@취소 이벤트 발생
			AbilityCancelled.Broadcast(GA);
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

	//@Event Tag
	if (!FGameplayTag::RequestGameplayTag(EventTag.GetTagName()).IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "유효하지 않은 Event Tag: {0}", *EventTag.ToString());
		return 0;
	}

	//@체인 액션 종료 이벤트 처리
	if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("EventTag.OnChainActionFinished")))
	{
		UE_LOGFMT(LogASC, Log, "체인 액션 종료 이벤트 처리");
		ChainActionFinished.ExecuteIfBound(ChainActionEventTag);

		//@콜백 제거
		ChainActionFinished.Clear();

		return 0;
	}

	//@Chain Window 활성화 시
	if (bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우 활성화 상태에서 이벤트 처리 시작 - EventTag: {0}",
			*EventTag.ToString());

		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (!Spec.Ability || !Spec.IsActive() || !Spec.Ability->AbilityTags.HasTag(OriginAbilityTag))
				continue;

			auto BaseGA = Cast<UBaseGameplayAbility>(Spec.Ability);
			if (!BaseGA || BaseGA->GetChainSystemType() != EChainSystemType::Passive)
				continue;

			UE_LOGFMT(LogASC, Log, "패시브 체인 시스템 검사 중 - Ability: {0}", *BaseGA->GetName());

			for (const auto& Mapping : BaseGA->GetChainEventMappings())
			{
				if (Mapping.RequiredEventTag == EventTag)
				{
					UE_LOGFMT(LogASC, Log, "체인 이벤트 매핑 매치 발견");
					bCanChainAction = true;
					ChainActionEventTag = Mapping.EventTagToSend;

					if (auto OriginInstance = Spec.GetPrimaryInstance())
					{
						ChainActionActivated.BindUFunction(OriginInstance, "OnChainActionActivated");
						ChainActionFinished.BindUFunction(OriginInstance, "OnChainActionFinished");

						UE_LOGFMT(LogASC, Log, "체인 액션 이벤트 바인딩 완료 - 어빌리티: {0}",
							*OriginInstance->GetName());
					}

					UE_LOGFMT(LogASC, Log, "Chain Event 매칭 성공 - RequiredTag: {0}, EventToSend: {1}",
						*Mapping.RequiredEventTag.ToString(),
						*Mapping.EventTagToSend.ToString());

					if (CurrentChainMode == EChainActionMode::ImmediateActivation)
					{
						UE_LOGFMT(LogASC, Log, "체인 윈도우 즉시 종료 시작");
						EndChainWindow(Payload);
					}
					return 0;
				}
			}
		}
	}

	//@체인 시스템의 실행 모드가 "즉시 실행"일 경우.
	if (CurrentChainMode == EChainActionMode::ImmediateActivation &&
		EventTag == ChainActionEventTag)
	{
		// Passive Chain인지 먼저 확인
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (!Spec.Ability || !Spec.IsActive() || !Spec.Ability->AbilityTags.HasTag(OriginAbilityTag))
				continue;

			auto BaseGA = Cast<UBaseGameplayAbility>(Spec.Ability);
			if (!BaseGA || BaseGA->GetChainSystemType() != EChainSystemType::Passive)
				continue;

			UE_LOGFMT(LogASC, Log, "즉시 실행 모드 이벤트 감지 - EventTag: {0}, Chain Action EventTag: {1}",
				*EventTag.ToString(), *ChainActionEventTag.ToString());

			UE_LOGFMT(LogASC, Log, "패시브 체인의 즉시 실행 모드 이벤트를 기본 이벤트 처리로 전달");
			return Super::HandleGameplayEvent(EventTag, Payload);
		}
	}

	return Super::HandleGameplayEvent(EventTag, Payload);
}

void UBaseAbilitySystemComponent::StartChainWindowWithTag(const FGameplayTag& InAbilityToBindTag, FGameplayTag InTagToChain)
{
	auto TargetSpec = [&]() -> const FGameplayAbilitySpec* {
		for (const auto& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(InAbilityToBindTag))
			{
				return &Spec;
			}
		}
		return nullptr;
	}();

	if (!TargetSpec || !TargetSpec->Ability)
	{
		UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 태그에 해당하는 어빌리티를 찾을 수 없음: {0}",
			*InAbilityToBindTag.ToString());
		EndChainWindow();
		return;
	}

	auto BaseGA = Cast<UBaseGameplayAbility>(TargetSpec->Ability);
	if (!BaseGA || !BaseGA->bUseChainSystem)
	{
		UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패");
		EndChainWindow();
		return;
	}

	//@Chain System Type에 따른 처리
	if (BaseGA->GetChainSystemType() == EChainSystemType::Active)
	{
		//@Active Chain System - 기존 로직
		auto ChainMapping = BaseGA->GetChainActionMapping(InTagToChain);
		if (!ChainMapping.AbilityTag.IsValid())
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 체인 액션 매핑");
			EndChainWindow();
			return;
		}

		OriginAbilityTag = InAbilityToBindTag;
		CurrentChainMode = ChainMapping.ChainActionMode;
		ChainActionEventTag = ChainMapping.EventTag;
		AllowedChainMappings = { ChainMapping };
	}
	else
	{
		//@Passive Chain System
		auto ChainMapping = BaseGA->GetChainEventMapping(InTagToChain);
		if (!ChainMapping.RequiredEventTag.IsValid())
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 체인 이벤트 매핑");
			EndChainWindow();
			return;
		}

		OriginAbilityTag = InAbilityToBindTag;
		CurrentChainMode = ChainMapping.ChainActionMode;
		ChainActionEventTag = ChainMapping.EventTagToSend;
		AllowedChainEventMappings = { ChainMapping };
	}

	bChainWindowActive = true;
	bCanChainAction = false;

	UE_LOGFMT(LogASC, Log, "체인 윈도우 시작 성공 - 어빌리티: {0} | 체인 모드: {1} | 체인 타입: {2}",
		*BaseGA->GetName(),
		CurrentChainMode == EChainActionMode::DelayedActivation ? TEXT("Delayed") : TEXT("Immediate"),
		BaseGA->GetChainSystemType() == EChainSystemType::Active ? TEXT("Active") : TEXT("Passive"));
}

void UBaseAbilitySystemComponent::EndChainWindow()
{
	//@빈 Payload를 생성하여 전달
	FGameplayEventData EmptyPayload;
	EndChainWindow(&EmptyPayload);
}

void UBaseAbilitySystemComponent::EndChainWindow(const FGameplayEventData* Payload)
{
	//@체인 윈도우 체크
	if (!bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 - 체인 윈도우가 활성화되지 않음");
		goto Cleanup;
	}

	//@체인 액션 가능 여부 체크
	if (!bCanChainAction)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 - 체인 액션이 불가능한 상태");
		goto Cleanup;
	}

	//@이벤트 태그 유효성 체크
	if (!ChainActionEventTag.IsValid())
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 - 유효하지 않은 체인 액션 이벤트 태그");
		goto Cleanup;
	}


	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 시작");

	//@원본 어빌리티 찾기
	UGameplayAbility* OriginGA = nullptr;
	if (OriginAbilityTag.IsValid())
	{
		UE_LOGFMT(LogASC, Log, "원본 어빌리티 검색 시작 - 태그: {0}", *OriginAbilityTag.ToString());

		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.IsActive() && Spec.Ability->AbilityTags.HasTag(OriginAbilityTag))
			{
				OriginGA = Spec.Ability;
				UE_LOGFMT(LogASC, Log, "원본 어빌리티 찾음: {0}", *OriginGA->GetName());
				break;
			}
		}
	}

	if (!OriginGA)
	{
		UE_LOGFMT(LogASC, Warning, "체인 액션 실행 실패 - 원본 어빌리티를 찾을 수 없음");
	}
	else
	{
		//@체인 액션 활성화 이벤트 호출
		ChainActionActivated.ExecuteIfBound(ChainActionEventTag);

		if (CurrentChainMode == EChainActionMode::ImmediateActivation)
		{
			UE_LOGFMT(LogASC, Log, "즉시 실행 모드 - 원본 이벤트 전송: {0}", *ChainActionEventTag.ToString());

			//@이벤트 호출
			HandleGameplayEvent(ChainActionEventTag, Payload);
		}
	}

	goto Cleanup;

Cleanup:
	//@초기화
	ChainActionActivated.Clear();
	bChainWindowActive = false;
	bCanChainAction = false;
	AllowedChainMappings.Empty();
	AllowedChainEventMappings.Empty();
	ChainActionEventTag = FGameplayTag();
	CurrentChainMode = EChainActionMode::DelayedActivation;

	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 완료");
}

void UBaseAbilitySystemComponent::StartInteractionWindow(AActor* TargetActor, const FPotentialInteraction& Interaction)
{
	//@이미 상호작용 윈도우가 활성화된 경우 종료
	if (bInteractionAvailable)
	{
		UE_LOGFMT(LogASC, Log, "이전 상호작용 윈도우 종료 후 새 상호작용 시작");
		EndInteractionWindow(false);
	}

	//@Target Actor, Object Tag, Input Tag
	if (!TargetActor || !Interaction.ObjectTag.IsValid() || !Interaction.InputTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "상호작용 윈도우 시작 실패 - 유효하지 않은 정보");
		return;
	}

	//@Target Actor
	InteractionTargetActor = TargetActor;
	//@FPotentailInteraction
	CurrentPotentialInteraction = Interaction;
	//@상호 작용 활성화
	bInteractionAvailable = true;

	UE_LOGFMT(LogASC, Log, "상호작용 윈도우 시작 성공 - 액터: {0} | 태그: {1} | 입력: {2}",
		*TargetActor->GetName(),
		*Interaction.ObjectTag.ToString(),
		*Interaction.InputTag.ToString());
}

void UBaseAbilitySystemComponent::EndInteractionWindow(bool bSuccess)
{
	//@bInteractionAvailable
	if (!bInteractionAvailable)
	{
		UE_LOGFMT(LogASC, Log, "상호작용 윈도우 종료 - 상호작용 윈도우가 활성화되지 않음");
		return;
	}

	//@성공, HandleGmaeplayEvent
	if (bSuccess && CurrentPotentialInteraction.EventTag.IsValid())
	{
		UE_LOGFMT(LogASC, Log, "상호작용 성공 - 이벤트 태그 발생: {0}",
			*CurrentPotentialInteraction.EventTag.ToString());

		HandleInteractionEvent(CurrentPotentialInteraction.EventTag);

		//상호 작용 성공 이벤트
		InteractionCompleted.Broadcast(InteractionTargetActor.Get(), CurrentPotentialInteraction);
	}

	//@Clean Up
	AActor* PreviousTargetActor = InteractionTargetActor.Get();
	FPotentialInteraction PreviousInteraction = CurrentPotentialInteraction;

	InteractionTargetActor = nullptr;
	CurrentPotentialInteraction = FPotentialInteraction();
	bInteractionAvailable = false;

	UE_LOGFMT(LogASC, Log, "상호작용 윈도우 종료 완료 - 성공: {0}", bSuccess);

	//@실패한 경우에만 상호작용 불가능 이벤트 발생
	if (!bSuccess && PreviousTargetActor)
	{
		InteractionUnavailable.Broadcast(PreviousTargetActor, PreviousInteraction);
	}
}

void UBaseAbilitySystemComponent::HandleInteractionEvent(const FGameplayTag& EventTag)
{
	//@Event Tag
	if (!EventTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "상호작용 이벤트 처리 실패 - 유효하지 않은 이벤트 태그");
		return;
	}

	//@Payload
	FGameplayEventData EventData;
	EventData.Instigator = GetOwner();
	EventData.Target = InteractionTargetActor.Get();
	EventData.EventTag = EventTag;

	//@InteractionData 생성 및 초기화
	UInteractionData* InteractionData = NewObject<UInteractionData>(GetOwner());
	if (!InteractionData)
	{
		UE_LOGFMT(LogASC, Warning, "상호작용 데이터 객체 생성 실패");
	}

	//@현재 상호작용 데이터 설정
	InteractionData->SetInteractionData(CurrentPotentialInteraction);

	//@OptionalObject1
	EventData.OptionalObject = InteractionData;

	UE_LOGFMT(LogASC, Log, "상호작용 데이터 객체 생성 성공 - 타입: {0}, 객체 태그: {1}",
		static_cast<int32>(CurrentPotentialInteraction.InteractionType),
		*CurrentPotentialInteraction.ObjectTag.ToString());

	//@Handle Gameplay Event
	HandleGameplayEvent(EventTag, &EventData);

	UE_LOGFMT(LogASC, Log, "상호작용 이벤트 처리 완료 - 이벤트 태그: {0}", *EventTag.ToString());
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
	AbilityActivated.Broadcast(Ability);
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
	AbilityEnded.Broadcast(Ability);
}

void UBaseAbilitySystemComponent::OnGameplayEffectApplied(
	UAbilitySystemComponent* Source,
	const FGameplayEffectSpec& SpecApplied,
	FActiveGameplayEffectHandle ActiveHandle)
{
	const FGameplayTagContainer& AssetTags = SpecApplied.Def->InheritableGameplayEffectTags.Added;

	//@State 태그 확인 및 이벤트 발생
	for (const FGameplayTag& StateTag : AssetTags)
	{
		if (StateTag.ToString().StartsWith("State"))
		{
			UE_LOGFMT(LogASC, Log, "상태 변화 감지: {0}", *StateTag.ToString());

			//@캐릭터 상태 이벤트
			CharacterStateEventOnGameplay.Broadcast(GetAvatarActor(), StateTag);

			//@죽음 상태일 경우 이벤트 발생 전에 모든 구독 해제
			if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
			{
				//@이벤트 구독 해제
				CharacterStateEventOnGameplay.Clear();
				return;
			}
		}
	}
}

bool UBaseAbilitySystemComponent::OnRequestActivateAbilityBlockUnitByAI(const FGameplayTag& AbilityTag)
{
	//@FGameplayTag
	if (!AbilityTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "어빌리티 활성화 요청 실패: 유효하지 않은 태그");
		return false;
	}

	//@어빌리티 스펙 찾기
	FGameplayAbilitySpecHandle SpecHandle;
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(AbilityTag))
		{
			SpecHandle = Spec.Handle;
			break;
		}
	}
	//@Spec Handle
	if (!SpecHandle.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "어빌리티 활성화 요청 실패: 태그({0})에 해당하는 어빌리티를 찾을 수 없음",
			*AbilityTag.ToString());
		return false;
	}

	//@어빌리티 활성화 시도
	if (!TryActivateAbility(SpecHandle))
	{
		UE_LOGFMT(LogASC, Warning, "어빌리티 활성화 실패 - Tag: {0}", *AbilityTag.ToString());
		return false;
	}

	UE_LOGFMT(LogASC, Log, "어빌리티 활성화 성공 - Tag: {0}", *AbilityTag.ToString());
	return true;
}

void UBaseAbilitySystemComponent::OnPotentialInteractionChanged(AActor* TargetActor, const FPotentialInteraction& Interaction)
{
	// 상호작용 객체의 유효성 체크
	if (!Interaction.ObjectTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "{0}: 유효하지 않은 상호작용 정보 수신", __FUNCDNAME__);
		return;
	}

	//@상호작용 허용 여부 확인
	bool bIsInteractionAvailable = Interaction.bAdditionalConditionsMet;

	//@상호 작용 가능
	if (bIsInteractionAvailable)
	{
		UE_LOGFMT(LogASC, Log, "{0}: 상호작용 활성화 - 액터: {1} | 태그: {2} | 타입: {3} | 이벤트: {4}",
			__FUNCDNAME__,
			TargetActor ? *TargetActor->GetName() : TEXT("없음"),
			*Interaction.ObjectTag.ToString(),
			static_cast<uint8>(Interaction.InteractionType),
			*Interaction.EventTag.ToString());

		//@잠재적 상호작용 허용 시작
		StartInteractionWindow(TargetActor, Interaction);

		//@상호작용 가능 이벤트
		InteractionAvailable.Broadcast(TargetActor, Interaction);
	}
	//@상호작용 취소
	else
	{
		UE_LOGFMT(LogASC, Log, "{0}: 상호작용 비활성화 - 액터: {1} | 태그: {2} | 타입: {3}",
			__FUNCDNAME__,
			TargetActor ? *TargetActor->GetName() : TEXT("없음"),
			*Interaction.ObjectTag.ToString(),
			static_cast<uint8>(Interaction.InteractionType));

		//@현재 상호작용이 취소된 상호작용과 동일한 경우에만 종료
		if (bInteractionAvailable && CurrentPotentialInteraction.ObjectTag == Interaction.ObjectTag)
		{
			EndInteractionWindow(false);
		}

		//@상호작용 허용 불가 이벤트
		InteractionUnavailable.Broadcast(TargetActor, Interaction);
	}
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