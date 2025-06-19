#include "BaseAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/AIAbilitySequencerComponent.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

#include "17_GameMode/AOWGameState.h"

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
	ActiveChainActions.Empty();
	ActiveChainEvents.Empty();

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

void UBaseAbilitySystemComponent::ExternalBindToAIController(ABaseAIController* BaseAIC)
{
	//@AIC
	if (!BaseAIC)
	{
		UE_LOGFMT(LogASC, Warning, "바인딩 실패: AI 컨트롤러가 유효하지 않음");
		return;
	}

	//@어빌리티 활성화 요청 이벤트 바인딩
	BaseAIC->CrowdControlEventTriggered.BindUFunction(this, "OnCrowdControlEventTriggered");

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

void UBaseAbilitySystemComponent::ExternalBindToGameState()
{
	UE_LOGFMT(LogASC, Log, "Game State 이벤트 바인딩 시작");

	//@World 가져오기
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOGFMT(LogASC, Error, "Game State 바인딩 실패: World를 찾을 수 없음");
		return;
	}

	//@Game State 가져오기
	AGameStateBase* GameStateBase = World->GetGameState();
	if (!IsValid(GameStateBase))
	{
		UE_LOGFMT(LogASC, Error, "Game State 바인딩 실패: GameState를 찾을 수 없음");
		return;
	}

	//@AOWGameState로 캐스팅
	AAOWGameState* AOWGameState = Cast<AAOWGameState>(GameStateBase);
	if (!IsValid(AOWGameState))
	{
		UE_LOGFMT(LogASC, Error, "Game State 바인딩 실패: AOWGameState 캐스팅 실패");
		return;
	}

	//@PlayerRespawnCompleted 이벤트에 바인딩
	AOWGameState->PlayerRespawnCompleted.AddUFunction(this, "OnPlayerRespawnCompleted");

	UE_LOGFMT(LogASC, Log, "Game State 이벤트 바인딩 성공: {0}", GetNameSafe(AOWGameState));
}

void UBaseAbilitySystemComponent::InitializeComponent()
{

	// @Ability 생명 주기 이벤트에 커스텀 콜백 함수 등록
	//@어빌리티 활성화 이벤트
	AbilityActivatedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityActivated);
	//@어빌리티 활성화 종료 이베느
	AbilityEndedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityEnded);
	//@어빌리티 활성화 실패 이벤트
	AbilityFailedCallbacks.AddUObject(this, &UBaseAbilitySystemComponent::OnAbilityFailed);

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
	//@Input Tag 유효성 확인
	if (!InputTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "입력 Tag가 유효하지 않음");
		return;
	}

	UE_LOGFMT(LogASC, Log, "입력된 Input Tag: {0}", *InputTag.ToString());

	//@상호작용 처리 (최우선 순위)
	if (bInteractionAvailable && InteractionTargetActor.IsValid())
	{
		if (CurrentPotentialInteraction.InputTag == InputTag)
		{
			UE_LOGFMT(LogASC, Log, "상호작용 입력 태그 매칭 성공 - 입력: {0}",
				*InputTag.ToString());
			EndInteractionWindow(true);
			return;
		}
	}

	//@체인 시스템 활성화 여부 확인 및 일반 입력 처리
	if (!bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 시스템 비활성화 - 일반 입력 처리");

		// 일반 어빌리티 입력 처리
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (!AbilitySpec.Ability || !AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
				continue;

			UE_LOGFMT(LogASC, Log, "입력 Tag에 매칭된 어빌리티: {0}",
				*AbilitySpec.Ability->GetName());

			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
		return;
	}

	//@이미 활성화된 체인이 있는지 확인
	bool bAnyChainAllowed = false;
	FGameplayTag AllowedAbilityTag;
	FGameplayTag AllowedInputTag;

	// 중첩 맵에서 허용된 체인 검색
	for (const auto& AbilityPair : ChainActionAllowedMap)
	{
		const FGameplayTag& AbilityTag = AbilityPair.Key;
		const TMap<FGameplayTag, bool>& InputTagMap = AbilityPair.Value;

		for (const auto& InputPair : InputTagMap)
		{
			if (InputPair.Value) // 이미 활성화된 체인 발견
			{
				bAnyChainAllowed = true;
				AllowedAbilityTag = AbilityTag;
				AllowedInputTag = InputPair.Key;
				break;
			}
		}

		if (bAnyChainAllowed)
			break;
	}

	//@이미 매칭에 성공한 체인 액션이 존재한다면?
	if (bAnyChainAllowed)
	{
		UE_LOGFMT(LogASC, Log, "이미 체인 액션이 활성화됨 - 어빌리티: {0}, 입력: {1}, 추가 입력 무시",
			*AllowedAbilityTag.ToString(), *AllowedInputTag.ToString());
		return;
	}

	//@4. 체인 액션 매칭 시도
	UE_LOGFMT(LogASC, Log, "체인 액션 매칭 시도 시작 - Input Tag: {0}", *InputTag.ToString());

	//@ActiveChainActions에서 매칭되는 체인 찾기
	for (const auto& ChainPair : ActiveChainActions)
	{
		const FGameplayTag& OriginAbilityTag = ChainPair.Key;
		const TArray<FChainActionMapping>& ChainMappings = ChainPair.Value;

		// 각 체인 매핑 검사
		for (const FChainActionMapping& ChainMapping : ChainMappings)
		{
			if (ChainMapping.Find(InputTag))
			{
				UE_LOGFMT(LogASC, Log, "체인 매핑 발견 - 어빌리티: {0}, 입력: {1}, 이벤트 태그: {2}",
					*OriginAbilityTag.ToString(), *InputTag.ToString(), *ChainMapping.EventTag.ToString());

				//@중첩 맵 구조에서 모든 체인 비활성화
				for (auto& AbilityPair : ChainActionAllowedMap)
				{
					for (auto& InputPair : AbilityPair.Value)
					{
						InputPair.Value = false;
					}
				}

				//@현재 체인 활성화
				TMap<FGameplayTag, bool>& InputTagMap = ChainActionAllowedMap.FindOrAdd(OriginAbilityTag);
				InputTagMap.Add(InputTag, true);

				//@원본 어빌리티 찾기 및 델리게이트 바인딩
				for (const FGameplayAbilitySpec& OriginSpec : GetActivatableAbilities())
				{
					if (OriginSpec.Ability && OriginSpec.IsActive() &&
						OriginSpec.Ability->AbilityTags.HasTag(OriginAbilityTag))
					{
						UE_LOGFMT(LogASC, Log, "원본 어빌리티 찾음 - Ability: {0}",
							*OriginSpec.Ability->GetName());

						if (auto OriginInstance = OriginSpec.GetPrimaryInstance())
						{
							ChainActionActivated.BindUFunction(OriginInstance, "OnChainActionActivated");
							ChainActionFinished.BindUFunction(OriginInstance, "OnChainActionFinished");
							UE_LOGFMT(LogASC, Log, "체인 액션 델리게이트 바인딩 완료");
						}
						break;
					}
				}

				//@즉시 활성화 모드인 경우 체인 윈도우 종료
				if (ChainMapping.ChainActionMode == EChainActionMode::ImmediateActivation)
				{
					UE_LOGFMT(LogASC, Log, "즉시 활성화 모드로 체인 윈도우 종료");
					EndChainWindow(OriginAbilityTag);
				}

				return;
			}
		}
	}

	//@5. 매칭되는 체인이 없는 경우
	UE_LOGFMT(LogASC, Log, "매칭되는 체인 액션을 찾지 못함 - Input Tag: {0}",
		*InputTag.ToString());
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

	//@Event Tag 유효성 검사
	if (!FGameplayTag::RequestGameplayTag(EventTag.GetTagName()).IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "유효하지 않은 Event Tag: {0}", *EventTag.ToString());
		return 0;
	}

	//@체인 액션 종료 이벤트 처리
	if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("EventTag.OnChainActionFinished")))
	{
		UE_LOGFMT(LogASC, Log, "체인 액션 종료 이벤트 처리");

		// 모든 활성화된 체인에 대해 체인 액션 종료 처리
		for (const auto& ChainPair : ActiveChainActions)
		{
			const FGameplayTag& AbilityTag = ChainPair.Key;
			const TArray<FChainActionMapping>& ChainMappings = ChainPair.Value;

			// 해당 어빌리티의 모든 체인 매핑을 검사
			for (const FChainActionMapping& ChainMapping : ChainMappings)
			{
				// 현재 활성화된 체인 매핑인지 확인
				if (ChainMapping.EventTag == EventTag &&
					ChainActionAllowedMap.Contains(AbilityTag))
				{
					// 중첩 맵에서 해당 입력 태그가 활성화되었는지 확인
					TMap<FGameplayTag, bool>* InputTagMap = ChainActionAllowedMap.Find(AbilityTag);
					for (const auto& InputPair : *InputTagMap)
					{
						if (InputPair.Value && ChainMapping.Find(InputPair.Key))
						{
							UE_LOGFMT(LogASC, Log, "체인 액션 종료 - 어빌리티: {0}, 이벤트: {1}",
								*AbilityTag.ToString(), *EventTag.ToString());

							// 델리게이트 호출 및 정리
							ChainActionFinished.ExecuteIfBound(EventTag);
							ChainActionFinished.Clear();

							// 체인 윈도우 종료
							EndChainWindow(AbilityTag);
							return 0;
						}
					}
				}
			}
		}

		return 0;
	}

	//@Chain Window 활성화 시
	if (bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우 활성화 상태에서 이벤트 처리 시작 - EventTag: {0}",
			*EventTag.ToString());

		// 각 활성화된 체인 이벤트에 대해 처리
		for (const auto& ChainPair : ActiveChainEvents)
		{
			const FGameplayTag& OriginAbilityTag = ChainPair.Key;
			const TArray<FChainEventMapping>& ChainMappings = ChainPair.Value;

			// 해당 어빌리티의 모든 체인 이벤트 매핑을 검사
			for (const FChainEventMapping& ChainMapping : ChainMappings)
			{
				// 해당 어빌리티가 현재 활성화되어 있는지 확인
				for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
				{
					if (!Spec.Ability || !Spec.IsActive() || !Spec.Ability->AbilityTags.HasTag(OriginAbilityTag))
						continue;

					auto BaseGA = Cast<UBaseGameplayAbility>(Spec.Ability);
					if (!BaseGA || BaseGA->GetChainSystemType() != EChainSystemType::Passive)
						continue;

					UE_LOGFMT(LogASC, Log, "패시브 체인 시스템 검사 중 - Ability: {0}", *BaseGA->GetName());

					// Required Event Tag가 일치하는지 확인
					if (ChainMapping.RequiredEventTag == EventTag)
					{
						UE_LOGFMT(LogASC, Log, "체인 이벤트 매핑 매치 발견");

						// 다른 모든 체인 비활성화
						for (auto& AbilityPair : ChainActionAllowedMap)
						{
							for (auto& InputPair : AbilityPair.Value)
							{
								InputPair.Value = false;
							}
						}

						// 해당 체인 활성화
						TMap<FGameplayTag, bool>& EventTagMap = ChainActionAllowedMap.FindOrAdd(OriginAbilityTag);
						EventTagMap.Add(ChainMapping.RequiredEventTag, true);

						if (auto OriginInstance = Spec.GetPrimaryInstance())
						{
							ChainActionActivated.BindUFunction(OriginInstance, "OnChainActionActivated");
							ChainActionFinished.BindUFunction(OriginInstance, "OnChainActionFinished");

							UE_LOGFMT(LogASC, Log, "체인 액션 이벤트 바인딩 완료 - 어빌리티: {0}",
								*OriginInstance->GetName());
						}

						UE_LOGFMT(LogASC, Log, "Chain Event 매칭 성공 - RequiredTag: {0}, EventToSend: {1}",
							*ChainMapping.RequiredEventTag.ToString(),
							*ChainMapping.EventTagToSend.ToString());

						// 즉시 실행 모드일 경우 체인 윈도우 종료
						if (ChainMapping.ChainActionMode == EChainActionMode::ImmediateActivation)
						{
							UE_LOGFMT(LogASC, Log, "체인 윈도우 즉시 종료 시작");
							EndChainWindow(OriginAbilityTag);
						}
						return 0;
					}
				}
			}
		}
	}

	//@체인 시스템의 실행 모드가 "즉시 실행"일 경우
	for (const auto& ChainPair : ActiveChainEvents)
	{
		const FGameplayTag& OriginAbilityTag = ChainPair.Key;
		const TArray<FChainEventMapping>& ChainMappings = ChainPair.Value;

		// 각 체인 이벤트 매핑 검사
		for (const FChainEventMapping& ChainMapping : ChainMappings)
		{
			// 즉시 실행 모드이고 해당 이벤트가 체인 액션 이벤트인지 확인
			if (ChainMapping.ChainActionMode == EChainActionMode::ImmediateActivation &&
				EventTag == ChainMapping.EventTagToSend)
			{
				for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
				{
					if (!Spec.Ability || !Spec.IsActive() || !Spec.Ability->AbilityTags.HasTag(OriginAbilityTag))
						continue;

					auto BaseGA = Cast<UBaseGameplayAbility>(Spec.Ability);
					if (!BaseGA || BaseGA->GetChainSystemType() != EChainSystemType::Passive)
						continue;

					UE_LOGFMT(LogASC, Log, "즉시 실행 모드 이벤트 감지 - EventTag: {0}, Chain Event Tag: {1}",
						*EventTag.ToString(), *ChainMapping.EventTagToSend.ToString());

					UE_LOGFMT(LogASC, Log, "패시브 체인의 즉시 실행 모드 이벤트를 기본 이벤트 처리로 전달");
					return Super::HandleGameplayEvent(EventTag, Payload);
				}
			}
		}
	}

	return Super::HandleGameplayEvent(EventTag, Payload);
}

void UBaseAbilitySystemComponent::StartChainWindowWithTag(const FGameplayTag& AbilityTagWaitingChainAction, FGameplayTag InputTagToChain)
{
	// 1단계: 어빌리티 찾기
	auto TargetSpec = [&]() -> const FGameplayAbilitySpec* {
		for (const auto& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(AbilityTagWaitingChainAction))
			{
				return &Spec;
			}
		}
		return nullptr;
		}();

	if (!TargetSpec || !TargetSpec->Ability)
	{
		UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 태그에 해당하는 어빌리티를 찾을 수 없음: {0}",
			*AbilityTagWaitingChainAction.ToString());
		// 특정 어빌리티에 대한 체인 윈도우 종료
		EndChainWindow(AbilityTagWaitingChainAction);
		return;
	}

	//@Base Gameplay Ability
	auto BaseGA = Cast<UBaseGameplayAbility>(TargetSpec->Ability);
	if (!BaseGA || !BaseGA->bUseChainSystem)
	{
		UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 어빌리티가 체인 시스템을 사용하지 않음");
		// 특정 어빌리티에 대한 체인 윈도우 종료
		EndChainWindow(AbilityTagWaitingChainAction);
		return;
	}

	// 2단계: 체인 타입에 따른 매핑 저장
	if (BaseGA->GetChainSystemType() == EChainSystemType::Active)
	{
		auto ChainMapping = BaseGA->GetChainActionMapping(InputTagToChain);
		if (!ChainMapping.InputTag.IsValid())
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 체인 액션 매핑");
			// 특정 어빌리티에 대한 체인 윈도우 종료
			EndChainWindow(AbilityTagWaitingChainAction);
			return;
		}

		// 해당 어빌리티의 체인 액션 배열에 추가
		TArray<FChainActionMapping>& ChainMappings = ActiveChainActions.FindOrAdd(AbilityTagWaitingChainAction);
		ChainMappings.Add(ChainMapping);

		// 중첩 맵 구조에서 초기 허용 상태를 설정
		TMap<FGameplayTag, bool>& InputTagMap = ChainActionAllowedMap.FindOrAdd(AbilityTagWaitingChainAction);
		InputTagMap.Add(InputTagToChain, false); // 초기에는 허용하지 않음

		UE_LOGFMT(LogASC, Log, "체인 액션 추가 - 어빌리티: {0}, 입력 태그: {1}",
			*AbilityTagWaitingChainAction.ToString(), *InputTagToChain.ToString());
	}
	else
	{
		auto ChainMapping = BaseGA->GetChainEventMapping(InputTagToChain);
		if (!ChainMapping.RequiredEventTag.IsValid())
		{
			UE_LOGFMT(LogASC, Warning, "체인 윈도우 시작 실패 - 유효하지 않은 체인 이벤트 매핑");
			// 특정 어빌리티에 대한 체인 윈도우 종료
			EndChainWindow(AbilityTagWaitingChainAction);
			return;
		}

		//@해당 어빌리티의 체인 이벤트 배열에 추가
		TArray<FChainEventMapping>& ChainEventMappings = ActiveChainEvents.FindOrAdd(AbilityTagWaitingChainAction);
		ChainEventMappings.Add(ChainMapping);

		//@중첩 맵 구조에서 초기 허용 상태를 설정
		TMap<FGameplayTag, bool>& EventTagMap = ChainActionAllowedMap.FindOrAdd(AbilityTagWaitingChainAction);
		EventTagMap.Add(ChainMapping.RequiredEventTag, false); // 초기에는 허용하지 않음

		UE_LOGFMT(LogASC, Log, "체인 이벤트 추가 - 어빌리티: {0}, 이벤트 태그: {1}",
			*AbilityTagWaitingChainAction.ToString(), *ChainMapping.RequiredEventTag.ToString());
	}

	//@체인 시스템 전체 활성화
	bChainWindowActive = true;

	UE_LOGFMT(LogASC, Log, "체인 윈도우 시작 성공 - 어빌리티: {0}",
		*AbilityTagWaitingChainAction.ToString());
}

void UBaseAbilitySystemComponent::EndChainWindow(const FGameplayTag& AbilityTag)
{
	//@빈 Payload를 생성하여 전달
	FGameplayEventData EmptyPayload;
	EndChainWindow(AbilityTag ,&EmptyPayload);
}

void UBaseAbilitySystemComponent::EndChainWindow(const FGameplayTag& AbilityTag, const FGameplayEventData* Payload)
{
	//@체인 윈도우 활성화 상태 확인
	if (!bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우가 이미 비활성화 상태입니다");
		return;
	}

	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 시작 - 어빌리티: {0}", *AbilityTag.ToString());

	//@해당 어빌리티가 활성화된 체인인지 확인
	// 어빌리티에 대한 허용된 체인 매핑 찾기
	bool bIsAllowedChain = false;
	FGameplayTag SelectedInputTag; // 활성화된 입력 태그 저장

	if (TMap<FGameplayTag, bool>* InputTagMap = ChainActionAllowedMap.Find(AbilityTag))
	{
		// 허용된 입력 태그 찾기
		for (const auto& InputPair : *InputTagMap)
		{
			if (InputPair.Value)
			{
				bIsAllowedChain = true;
				SelectedInputTag = InputPair.Key;
				break;
			}
		}
	}

	UE_LOGFMT(LogASC, Log, "체인 상태 확인 - 어빌리티: {0}, 허용 상태: {1}, 선택된 입력: {2}",
		*AbilityTag.ToString(), bIsAllowedChain, SelectedInputTag.IsValid() ? *SelectedInputTag.ToString() : TEXT("없음"));

	//@Step 1: 선택된 체인 실행 및 정리
	if (bIsAllowedChain)
	{
		// 체인 액션 처리 (액티브 체인)
		if (TArray<FChainActionMapping>* ChainMappings = ActiveChainActions.Find(AbilityTag))
		{
			// 선택된 입력 태그에 해당하는 체인 매핑 찾기
			for (const FChainActionMapping& ChainMapping : *ChainMappings)
			{
				if (ChainMapping.Find(SelectedInputTag))
				{
					UE_LOGFMT(LogASC, Log, "체인 액션 실행 - 어빌리티: {0}, 입력: {1}, 이벤트: {2}",
						*AbilityTag.ToString(), *SelectedInputTag.ToString(), *ChainMapping.EventTag.ToString());

					// 원본 어빌리티 찾기
					UGameplayAbility* OriginGA = nullptr;
					for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
					{
						if (Spec.Ability && Spec.IsActive() && Spec.Ability->AbilityTags.HasTag(AbilityTag))
						{
							OriginGA = Spec.Ability;
							break;
						}
					}

					if (OriginGA)
					{
						// 체인 액션 활성화 이벤트 호출
						ChainActionActivated.ExecuteIfBound(ChainMapping.EventTag);

						// 즉시 실행 모드인 경우 이벤트 전송
						if (ChainMapping.ChainActionMode == EChainActionMode::ImmediateActivation)
						{
							HandleGameplayEvent(ChainMapping.EventTag, Payload);
						}
					}

					break; // 매칭된 체인 찾았으므로 루프 종료
				}
			}
		}

		// 체인 이벤트 처리 (패시브 체인)
		if (TArray<FChainEventMapping>* ChainEventMappings = ActiveChainEvents.Find(AbilityTag))
		{
			// 선택된 입력 태그에 해당하는 체인 이벤트 매핑 찾기
			for (const FChainEventMapping& ChainEventMapping : *ChainEventMappings)
			{
				if (ChainEventMapping.RequiredEventTag == SelectedInputTag)
				{
					UE_LOGFMT(LogASC, Log, "체인 이벤트 실행 - 어빌리티: {0}, 요구 이벤트: {1}, 전송 이벤트: {2}",
						*AbilityTag.ToString(), *SelectedInputTag.ToString(), *ChainEventMapping.EventTagToSend.ToString());

					// 원본 어빌리티 찾기
					UGameplayAbility* OriginGA = nullptr;
					for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
					{
						if (Spec.Ability && Spec.IsActive() && Spec.Ability->AbilityTags.HasTag(AbilityTag))
						{
							OriginGA = Spec.Ability;
							break;
						}
					}

					if (OriginGA)
					{
						// 체인 액션 활성화 이벤트 호출
						ChainActionActivated.ExecuteIfBound(ChainEventMapping.EventTagToSend);

						// 즉시 실행 모드인 경우 이벤트 전송
						if (ChainEventMapping.ChainActionMode == EChainActionMode::ImmediateActivation)
						{
							HandleGameplayEvent(ChainEventMapping.EventTagToSend, Payload);
						}
					}

					break; // 매칭된 체인 찾았으므로 루프 종료
				}
			}
		}
	}

	//@Step 2: 실행되지 않은 모든 체인 정리
	// 제거할 체인 목록 수집
	TArray<FGameplayTag> ChainsToRemove;

	// 현재 어빌리티가 허용된 체인이 아니거나, 다른 모든 어빌리티의 체인들을 제거 대상으로 추가
	for (const auto& ChainPair : ChainActionAllowedMap)
	{
		if (ChainPair.Key != AbilityTag || !bIsAllowedChain)
		{
			ChainsToRemove.Add(ChainPair.Key);
		}
	}

	// 사용되지 않은 체인들을 시스템에서 제거
	for (const FGameplayTag& ChainToRemove : ChainsToRemove)
	{
		ActiveChainActions.Remove(ChainToRemove);
		ActiveChainEvents.Remove(ChainToRemove);
		ChainActionAllowedMap.Remove(ChainToRemove);

		UE_LOGFMT(LogASC, Log, "미사용 체인 제거 - 어빌리티: {0}",
			*ChainToRemove.ToString());
	}

	// 선택된 체인도 제거 (허용된 체인이었을 경우)
	if (bIsAllowedChain)
	{
		ActiveChainActions.Remove(AbilityTag);
		ActiveChainEvents.Remove(AbilityTag);
		ChainActionAllowedMap.Remove(AbilityTag);

		UE_LOGFMT(LogASC, Log, "선택된 체인 제거 - 어빌리티: {0}",
			*AbilityTag.ToString());
	}

	//@Step 3: 시스템 상태 최종 확인
	// 모든 체인이 정리되었다면 체인 시스템 전체를 비활성화
	if (ActiveChainActions.Num() == 0 && ActiveChainEvents.Num() == 0)
	{
		bChainWindowActive = false;
		ChainActionAllowedMap.Empty();

		// 델리게이트 정리
		ChainActionActivated.Clear();
		ChainActionFinished.Clear();

		UE_LOGFMT(LogASC, Log, "모든 체인 종료로 인해 체인 시스템 비활성화");
	}

	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 완료 - 어빌리티: {0}", *AbilityTag.ToString());
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

	//@상호작용 활성화 이벤트 호출
	InteractionActivated.Broadcast(TargetActor, Interaction);
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
	}

	//@Clean Up
	AActor* PreviousTargetActor = InteractionTargetActor.Get();
	FPotentialInteraction PreviousInteraction = CurrentPotentialInteraction;

	InteractionTargetActor = nullptr;
	CurrentPotentialInteraction = FPotentialInteraction();
	bInteractionAvailable = false;

	UE_LOGFMT(LogASC, Log, "상호작용 윈도우 종료 완료 - 성공: {0}", bSuccess);

	//@상호작용 실패 이벤트 호출
	if (!bSuccess && PreviousTargetActor)
	{
		InteractionFailed.Broadcast(PreviousTargetActor, PreviousInteraction);
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

	//@상호작용 어빌리티
	if (Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag("Ability.Interaction")))
	{
		UE_LOGFMT(LogASC, Log, "상호작용 어빌리티 완료 - {0}", *Ability->GetName());

		//@상호작용 완료 이벤트
		InteractionCompleted.Broadcast(InteractionTargetActor.Get(), CurrentPotentialInteraction);
	}

	UE_LOGFMT(LogASC, Warning, "{0}가 종료되었습니다.", Ability->GetName());

	// @TODO: Ability 활성화 종료 시점에 ASC에서 할 일들...
	AbilityEnded.Broadcast(Ability);
}

void UBaseAbilitySystemComponent::OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& ReasonTags)
{
	if (!Ability)
	{
		UE_LOGFMT(LogASC, Warning, "OnAbilityFailed: 유효하지 않은 어빌리티");
		return;
	}

	// 상호작용 어빌리티인지 확인
	if (Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag("Ability.Interaction")))
	{
		UE_LOGFMT(LogASC, Log, "상호작용 어빌리티 실패 - {0}, 실패 사유: {1}",
			*Ability->GetName(), *ReasonTags.ToString());

		//@상호작용 실패 이벤트
		InteractionFailed.Broadcast(InteractionTargetActor.Get(), CurrentPotentialInteraction);
	}
}

void UBaseAbilitySystemComponent::OnGameplayEffectApplied(
	UAbilitySystemComponent* Source,
	const FGameplayEffectSpec& SpecApplied,
	FActiveGameplayEffectHandle ActiveHandle)
{
	const FGameplayTagContainer& AssetTags = SpecApplied.Def->InheritableGameplayEffectTags.Added;

	// 디버깅을 위한 로그 출력
	UE_LOGFMT(LogASC, Log, "GameplayEffect 적용 - 총 AssetTags 개수: {0}", AssetTags.Num());
	for (const FGameplayTag& Tag : AssetTags)
	{
		UE_LOGFMT(LogASC, Log, "AssetTag: {0}", Tag.ToString());
	}

	// 정적 태그 한 번만 생성 (성능 최적화)
	FGameplayTag StateTag = FGameplayTag::RequestGameplayTag("State");
	FGameplayTag DeadStateTag = FGameplayTag::RequestGameplayTag("State.Dead");

	// State 태그 확인 및 이벤트 발생
	for (const FGameplayTag& TagFromEffect : AssetTags)
	{
		// State 계층 태그 확인 (State 또는 모든 자식 태그)
		if (TagFromEffect.MatchesTag(StateTag))
		{
			UE_LOGFMT(LogASC, Log, "상태 변화 감지: {0}", TagFromEffect.ToString());

			CharacterStateEventOnGameplay.Broadcast(GetAvatarActor(), TagFromEffect);
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
		else
		{
			//@상호작용 실패 이벤트
			InteractionFailed.Broadcast(TargetActor, Interaction);
		}
	}
}

void UBaseAbilitySystemComponent::OnCrowdControlEventTriggered(const FGameplayTag& CrowdControlTag)
{
	//@Tag 유효성 검사
	if (!CrowdControlTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "군중 제어 이벤트 처리 실패: 유효하지 않은 태그");
		return;
	}

	UE_LOGFMT(LogASC, Log, "군중 제어 이벤트 수신: {0}", *CrowdControlTag.ToString());

	//@Avatar Actor 체크
	AActor* Avatar = GetAvatarActor();
	if (!Avatar)
	{
		UE_LOGFMT(LogASC, Warning, "군중 제어 이벤트 처리 실패: 유효한 아바타 액터가 없음");
		return;
	}

	//@태그에 따른 이벤트 처리
	if (CrowdControlTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("CrowdControl.Threatened")))
	{
		UE_LOGFMT(LogASC, Log, "위협(Threatened) 군중 제어 처리 시작 - Actor: {0}", *Avatar->GetName());

		//@위협 GameplayEvent 발생
		FGameplayEventData EventData;
		EventData.EventTag = FGameplayTag::RequestGameplayTag("EventTag.CrowdControl.OnThreatened");
		EventData.Instigator = Avatar;

		HandleGameplayEvent(EventData.EventTag, &EventData);
	}

	UE_LOGFMT(LogASC, Log, "군중 제어 이벤트 처리 완료: {0}", *CrowdControlTag.ToString());
}

void UBaseAbilitySystemComponent::OnPlayerRespawnCompleted(APlayerController* RespawnedPlayerController)
{
	//@기본 유효성 검증
	if (!IsValid(RespawnedPlayerController))
	{
		UE_LOGFMT(LogASC, Warning, "리스폰 완료 콜백 실패: 유효하지 않은 PlayerController");
		return;
	}

	//@소유자 확인 (이 ASC의 소유자와 리스폰된 PlayerController가 같은지)
	if (GetAvatarActor() == RespawnedPlayerController->GetPawn())
	{
		UE_LOGFMT(LogASC, Log, "자신의 리스폰 완료 감지: {0}", GetNameSafe(RespawnedPlayerController));

		FGameplayEventData EmptyPayload;
		HandleGameplayEvent(FGameplayTag::RequestGameplayTag("EventTag.OnRevivalActivated"), &EmptyPayload);
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