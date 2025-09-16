#include "BaseAbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/AIAbilitySequencerComponent.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

#include "17_GameMode/AgeOfWolvesGameMode.h"
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

	//@Pending Abilities
	PendingReleaseAbilities.Reset();

	//@Chain Window 활성화
	bChainWindowActive = false;
	//@Chain Mapping 목록
	ActiveChainActions.Empty();
	ActiveChainEvents.Empty();

	//@상호작용 시스템 초기화
	bInteractionAvailable = false;
	InteractionTargetActor = nullptr;
}

void UBaseAbilitySystemComponent::ExternalBindToPlayerState(APlayerStateBase* PlayerState)
{
	//@PS
	if (!PlayerState)
	{
		UE_LOGFMT(LogASC, Warning, "ExternalBindToPlayerState: PlayerState가 유효하지 않습니다");
		return;
	}

	//외부 바인딩...
	PlayerState->RequestGrantAbilities.AddUFunction(this, "OnRequestGrantAbilities");
	PlayerState->RequestActivateAbilities.AddUFunction(this, "OnRequestActivateAbilities");
	PlayerState->RequestApplyEffects.AddUFunction(this, "OnRequestApplyEffects");

	UE_LOGFMT(LogASC, Log, "PlayerState 이벤트 바인딩 완료");
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

	//@Press 입력 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
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

	//@활성화 대상 어빌리티들 실행
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	//@InputReleased 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				const UBaseGameplayAbility* BaseAbilityCDO = Cast<UBaseGameplayAbility>(AbilitySpec->Ability);
				if (BaseAbilityCDO)
				{
					//@OnInputReleased 정책 어빌리티 활성화
					if (BaseAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputReleased)
					{
						if (!AbilitySpec->IsActive())
						{
							if (TryActivateAbility(AbilitySpec->Handle))
							{
								UE_LOGFMT(LogASC, Log, "입력 해제로 어빌리티 활성화: {0}", *BaseAbilityCDO->GetName());
							}
							else
							{
								// 활성화 실패 시 예약 목록에 추가
								PendingReleaseAbilities.AddUnique(AbilitySpec->Handle);
								UE_LOGFMT(LogASC, Log, "입력 해제 어빌리티 예약됨: {0}", *BaseAbilityCDO->GetName());
							}
						}
					}
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
	PendingReleaseAbilities.Reset(); // 추가
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
		// 체인 종료가 필요한 어빌리티들을 수집
		TArray<FGameplayTag> ChainAbilitiesToEnd;

		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				UE_LOGFMT(LogASC, Log, "입력 해제된 어빌리티: {0} | 해제된 Tag: {1}",
					AbilitySpec.Ability->GetName(),
					InputTag.ToString());

				// WhileInputActive 정책인지 확인
				const UBaseGameplayAbility* BaseAbilityCDO = Cast<UBaseGameplayAbility>(AbilitySpec.Ability);
				if (BaseAbilityCDO && BaseAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					UE_LOGFMT(LogASC, Log, "WhileInputActive 어빌리티 감지 - 체인 종료 대상 추가: {0}",
						AbilitySpec.Ability->GetName());

					// 해당 어빌리티의 태그들 중 체인과 관련된 것들을 찾아서 종료 목록에 추가
					for (const FGameplayTag& AbilityTag : AbilitySpec.Ability->AbilityTags)
					{
						// 활성화된 체인에서 해당 어빌리티가 있는지 확인
						if (ActiveChainActions.Contains(AbilityTag) || ActiveChainEvents.Contains(AbilityTag))
						{
							ChainAbilitiesToEnd.AddUnique(AbilityTag);
							UE_LOGFMT(LogASC, Log, "체인 종료 예정 어빌리티 태그: {0}", AbilityTag.ToString());
						}
					}
				}

				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}

		// 수집된 어빌리티들의 체인 종료
		for (const FGameplayTag& AbilityTag : ChainAbilitiesToEnd)
		{
			UE_LOGFMT(LogASC, Log, "WhileInputActive 어빌리티 입력 해제로 인한 체인 종료: {0}", AbilityTag.ToString());
			EndChainWindow(AbilityTag);
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
	UE_LOGFMT(LogASC, Log, "{0}: GameplayEvent 처리 시작 - EventTag: {1}",GetAvatarActor()->GetName(), *EventTag.ToString());

	//@Event Tag 유효성 검사
	if (!FGameplayTag::RequestGameplayTag(EventTag.GetTagName()).IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "유효하지 않은 Event Tag: {0}", *EventTag.ToString());
		return 0;
	}

	//@데미지 이벤트 전처리 (강공격 파훼 등)
	if (EventTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("EventTag.OnDamaged")))
	{
		bool bShouldContinueProcessing = true;

		//@전처리 델리게이트 호출
		DamageEventPreProcess.Broadcast(EventTag, *Payload, bShouldContinueProcessing);

		if (!bShouldContinueProcessing)
		{
			UE_LOGFMT(LogASC, Log, "데미지 이벤트 처리 중단됨 - 파훼 매커니즘에 의해 차단");
			return 0; // 처리 중단
		}
	}

	//@체인 액션 종료 이벤트 처리
	if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("EventTag.OnChainActionFinished")))
	{
		UE_LOGFMT(LogASC, Log, "체인 액션 종료 이벤트 처리");

		for (const auto& ChainPair : ActiveChainActions)
		{
			const FGameplayTag& AbilityTag = ChainPair.Key;
			const TArray<FChainActionMapping>& ChainMappings = ChainPair.Value;

			for (const FChainActionMapping& ChainMapping : ChainMappings)
			{
				if (ChainMapping.EventTag == EventTag && ChainActionAllowedMap.Contains(AbilityTag))
				{
					TMap<FGameplayTag, bool>* InputTagMap = ChainActionAllowedMap.Find(AbilityTag);
					for (const auto& InputPair : *InputTagMap)
					{
						if (InputPair.Value && ChainMapping.Find(InputPair.Key))
						{
							UE_LOGFMT(LogASC, Log, "체인 액션 종료 - 어빌리티: {0}, 이벤트: {1}",
								*AbilityTag.ToString(), *EventTag.ToString());

							ChainActionFinished.ExecuteIfBound(EventTag);
							ChainActionFinished.Clear();
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
		UE_LOGFMT(LogASC, Log, "체인 윈도우 활성화 상태에서 이벤트 처리 시작 - EventTag: {0}", *EventTag.ToString());

		for (auto& ChainPair : ActiveChainEvents)
		{
			const FGameplayTag& OriginAbilityTag = ChainPair.Key;
			TArray<FChainEventMapping>& ChainMappings = ChainPair.Value;

			for (FChainEventMapping& ChainMapping : ChainMappings)
			{
				for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
				{
					if (!Spec.Ability || !Spec.IsActive() || !Spec.Ability->AbilityTags.HasTag(OriginAbilityTag))
						continue;

					auto BaseGA = Cast<UBaseGameplayAbility>(Spec.Ability);
					if (!BaseGA || BaseGA->GetChainSystemType() != EChainSystemType::Passive)
						continue;

					if (ChainMapping.RequiredEventTag == EventTag)
					{
						UE_LOGFMT(LogASC, Log, "체인 이벤트 매핑 매치 발견");

						// Payload 정보를 ChainEventMapping에 저장
						if (Payload)
						{
							ChainMapping.SetChainEventData(Payload->Instigator.Get(), Payload->Target.Get(), Payload->ContextHandle);
							UE_LOGFMT(LogASC, Log, "체인 이벤트 데이터 저장 - Instigator: {0}, Target: {1}",
								Payload->Instigator.Get() ? *Payload->Instigator->GetName() : TEXT("없음"),
								Payload->Target.Get() ? *Payload->Target->GetName() : TEXT("없음"));
						}

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
						}

						UE_LOGFMT(LogASC, Log, "Chain Event 매칭 성공 - RequiredTag: {0}, EventToSend: {1}",
							*ChainMapping.RequiredEventTag.ToString(), *ChainMapping.EventTagToSend.ToString());

						// 실행 모드에 따른 처리
						if (ChainMapping.ChainActionMode == EChainActionMode::ImmediateActivation)
						{
							UE_LOGFMT(LogASC, Log, "즉시 실행 모드 - 원본 Payload로 EventTagToSend 재호출");
							ChainActionActivated.ExecuteIfBound(ChainMapping.EventTagToSend);
							return HandleGameplayEvent(ChainMapping.EventTagToSend, Payload);
						}
						else
						{
							UE_LOGFMT(LogASC, Log, "지연 실행 모드 - 데이터 저장 후 체인 윈도우 종료 대기");
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

		for (const FChainEventMapping& ChainMapping : ChainMappings)
		{
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

					UE_LOGFMT(LogASC, Log, "즉시 실행 모드 이벤트 감지 - EventTag: {0}", *EventTag.ToString());
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
	if (!bChainWindowActive)
	{
		UE_LOGFMT(LogASC, Log, "체인 윈도우가 이미 비활성화 상태입니다");
		return;
	}

	UE_LOGFMT(LogASC, Log, "체인 윈도우 종료 시작 - 어빌리티: {0}", *AbilityTag.ToString());

	bool bIsAllowedChain = false;
	FGameplayTag SelectedInputTag;

	if (TMap<FGameplayTag, bool>* InputTagMap = ChainActionAllowedMap.Find(AbilityTag))
	{
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

	if (bIsAllowedChain)
	{
		// 체인 액션 처리 (액티브 체인)
		if (TArray<FChainActionMapping>* ChainMappings = ActiveChainActions.Find(AbilityTag))
		{
			for (const FChainActionMapping& ChainMapping : *ChainMappings)
			{
				if (ChainMapping.Find(SelectedInputTag))
				{
					UE_LOGFMT(LogASC, Log, "체인 액션 실행 - 어빌리티: {0}, 입력: {1}, 이벤트: {2}",
						*AbilityTag.ToString(), *SelectedInputTag.ToString(), *ChainMapping.EventTag.ToString());

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
						ChainActionActivated.ExecuteIfBound(ChainMapping.EventTag);
						if (ChainMapping.ChainActionMode == EChainActionMode::ImmediateActivation)
						{
							HandleGameplayEvent(ChainMapping.EventTag, Payload);
						}
					}
					break;
				}
			}
		}

		// 체인 이벤트 처리 (패시브 체인)
		if (TArray<FChainEventMapping>* ChainEventMappings = ActiveChainEvents.Find(AbilityTag))
		{
			for (const FChainEventMapping& ChainEventMapping : *ChainEventMappings)
			{
				if (ChainEventMapping.RequiredEventTag == SelectedInputTag)
				{
					UE_LOGFMT(LogASC, Log, "체인 이벤트 실행 - 어빌리티: {0}, 요구 이벤트: {1}, 전송 이벤트: {2}",
						*AbilityTag.ToString(), *SelectedInputTag.ToString(), *ChainEventMapping.EventTagToSend.ToString());

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
						ChainActionActivated.ExecuteIfBound(ChainEventMapping.EventTagToSend);

						// 지연 실행 모드일 경우 저장된 데이터로 Payload 구성하여 이벤트 전송
						if (ChainEventMapping.ChainActionMode == EChainActionMode::DelayedActivation)
						{
							FGameplayEventData ChainPayload;
							ChainPayload.EventTag = ChainEventMapping.EventTagToSend;
							ChainPayload.Instigator = ChainEventMapping.ChainInstigator.Get();
							ChainPayload.Target = ChainEventMapping.ChainTarget.Get();
							ChainPayload.ContextHandle = ChainEventMapping.ChainContextHandle;

							UE_LOGFMT(LogASC, Log, "지연 실행 모드 - 저장된 데이터로 Payload 구성하여 이벤트 발생: {0}",
								*ChainEventMapping.EventTagToSend.ToString());

							HandleGameplayEvent(ChainEventMapping.EventTagToSend, &ChainPayload);
						}
					}
					break;
				}
			}
		}
	}

	// 체인 정리
	TArray<FGameplayTag> ChainsToRemove;
	for (const auto& ChainPair : ChainActionAllowedMap)
	{
		if (ChainPair.Key != AbilityTag || !bIsAllowedChain)
		{
			ChainsToRemove.Add(ChainPair.Key);
		}
	}

	for (const FGameplayTag& ChainToRemove : ChainsToRemove)
	{
		if (TArray<FChainActionMapping>* ChainMappings = ActiveChainActions.Find(ChainToRemove))
		{
			for (FChainActionMapping& ChainMapping : *ChainMappings)
			{
				ChainMapping.ClearChainEventData();
			}
		}

		if (TArray<FChainEventMapping>* ChainEventMappings = ActiveChainEvents.Find(ChainToRemove))
		{
			for (FChainEventMapping& ChainEventMapping : *ChainEventMappings)
			{
				ChainEventMapping.ClearChainEventData();
			}
		}

		ActiveChainActions.Remove(ChainToRemove);
		ActiveChainEvents.Remove(ChainToRemove);
		ChainActionAllowedMap.Remove(ChainToRemove);
	}

	if (bIsAllowedChain)
	{
		if (TArray<FChainActionMapping>* ChainMappings = ActiveChainActions.Find(AbilityTag))
		{
			for (FChainActionMapping& ChainMapping : *ChainMappings)
			{
				ChainMapping.ClearChainEventData();
			}
		}

		if (TArray<FChainEventMapping>* ChainEventMappings = ActiveChainEvents.Find(AbilityTag))
		{
			for (FChainEventMapping& ChainEventMapping : *ChainEventMappings)
			{
				ChainEventMapping.ClearChainEventData();
			}
		}

		ActiveChainActions.Remove(AbilityTag);
		ActiveChainEvents.Remove(AbilityTag);
		ChainActionAllowedMap.Remove(AbilityTag);
	}

	if (ActiveChainActions.Num() == 0 && ActiveChainEvents.Num() == 0)
	{
		bChainWindowActive = false;
		ChainActionAllowedMap.Empty();
		ChainActionActivated.Clear();
		ChainActionFinished.Clear();
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

	// 마지막에 추가: 예약된 Release 어빌리티들 활성화 시도
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

	for (const FGameplayAbilitySpecHandle& Handle : PendingReleaseAbilities)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			if (TryActivateAbility(Handle))
			{
				UE_LOGFMT(LogASC, Log, "예약된 Release 어빌리티 활성화: {0}", *Spec->Ability->GetName());
				AbilitiesToRemove.Add(Handle);
				break; // 하나만 활성화하고 종료
			}
		}
		else
		{
			// 유효하지 않은 핸들은 제거 대상에 추가
			AbilitiesToRemove.Add(Handle);
		}
	}

	// 처리된 어빌리티들을 예약 목록에서 제거
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToRemove)
	{
		PendingReleaseAbilities.Remove(Handle);
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

			// 죽음 상태 특별 처리
			if (TagFromEffect.MatchesTagExact(DeadStateTag))
			{
				if (APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetAvatarActor())->GetController()))
				{
					if (AAgeOfWolvesGameMode* GameMode = GetWorld()->GetAuthGameMode<AAgeOfWolvesGameMode>())
					{
						GameMode->HandlePlayerDeath(PC);
					}
				}
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

void UBaseAbilitySystemComponent::OnRequestGrantAbilities(const TArray<TSubclassOf<UBaseGameplayAbility>>& Abilities, const FGameplayTag& ItemTag, bool bAllowDuplicate)
{
	if (Abilities.IsEmpty())
	{
		UE_LOGFMT(LogASC, Warning, "OnRequestGrantAbilities: 빈 어빌리티 배열 - {0}", ItemTag.ToString());
		return;
	}

	UE_LOGFMT(LogASC, Log, "어빌리티 부여 시작: {0} ({1}개)", ItemTag.ToString(), Abilities.Num());

	int32 SuccessCount = 0;
	for (const auto& AbilityClass : Abilities)
	{
		if (!AbilityClass)
		{
			UE_LOGFMT(LogASC, Warning, "유효하지 않은 어빌리티 클래스");
			continue;
		}

		//@중복 검사
		if (!bAllowDuplicate)
		{
			bool bAlreadyExists = false;
			const TArray<FGameplayAbilitySpec>& Abilities = GetActivatableAbilities();
			for (const FGameplayAbilitySpec& AbilitySpec : Abilities)
			{
				if (AbilitySpec.Ability && AbilitySpec.Ability->GetClass() == AbilityClass)
				{
					bAlreadyExists = true;
					break;
				}
			}

			if (bAlreadyExists)
			{
				UE_LOGFMT(LogASC, Warning, "어빌리티 {0} 이미 존재", *AbilityClass->GetName());
				continue;
			}
		}

		//@어빌리티 등록
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, INDEX_NONE);
		FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

		if (Handle.IsValid())
		{
			SuccessCount++;
			UE_LOGFMT(LogASC, Log, "어빌리티 {0} 부여 성공", *AbilityClass->GetName());
		}
	}

	UE_LOGFMT(LogASC, Log, "어빌리티 부여 완료: {0}/{1}", SuccessCount, Abilities.Num());
}

void UBaseAbilitySystemComponent::OnRequestActivateAbilities(const TArray<TSubclassOf<UBaseGameplayAbility>>& Abilities, const FGameplayTag& ItemTag, bool bForceActivate)
{
	if (Abilities.IsEmpty())
	{
		UE_LOGFMT(LogASC, Warning, "OnRequestActivateAbilities: 빈 어빌리티 배열 - {0}", ItemTag.ToString());
		return;
	}

	UE_LOGFMT(LogASC, Log, "어빌리티 활성화 시작: {0} ({1}개)", ItemTag.ToString(), Abilities.Num());

	int32 SuccessCount = 0;
	for (const auto& AbilityClass : Abilities)
	{
		if (!AbilityClass)
		{
			UE_LOGFMT(LogASC, Warning, "유효하지 않은 어빌리티 클래스");
			continue;
		}

		FGameplayAbilitySpec* FoundSpec = FindAbilitySpecFromClass(AbilityClass);
		if (!FoundSpec)
		{
			UE_LOGFMT(LogASC, Warning, "어빌리티 {0} 스펙을 찾을 수 없음", *AbilityClass->GetName());
			continue;
		}

		FGameplayAbilitySpecHandle SpecHandle = FoundSpec->Handle;
		if (!SpecHandle.IsValid())
		{
			UE_LOGFMT(LogASC, Warning, "어빌리티 {0} 핸들이 유효하지 않음", *AbilityClass->GetName());
			continue;
		}

		if (!TryActivateAbility(SpecHandle))
		{
			UE_LOGFMT(LogASC, Warning, "어빌리티 {0} 활성화 실패", *AbilityClass->GetName());
			continue;
		}

		SuccessCount++;
		UE_LOGFMT(LogASC, Log, "어빌리티 {0} 활성화 성공", *AbilityClass->GetName());
	}

	UE_LOGFMT(LogASC, Log, "어빌리티 활성화 완료: {0}/{1}", SuccessCount, Abilities.Num());
}

void UBaseAbilitySystemComponent::OnRequestApplyEffects(const TArray<TSubclassOf<UGameplayEffect>>& Effects, const FGameplayTag& ItemTag, bool bAllowDuplicate)
{
	if (Effects.IsEmpty())
	{
		UE_LOGFMT(LogASC, Warning, "OnRequestApplyEffects: 빈 이펙트 배열 - {0}", ItemTag.ToString());
		return;
	}

	UE_LOGFMT(LogASC, Log, "이펙트 적용 시작: {0} ({1}개)", ItemTag.ToString(), Effects.Num());

	int32 SuccessCount = 0;
	for (const auto& EffectClass : Effects)
	{
		if (!EffectClass)
		{
			UE_LOGFMT(LogASC, Warning, "유효하지 않은 이펙트 클래스");
			continue;
		}

		// 중복 검사
		if (!bAllowDuplicate)
		{
			bool bAlreadyActive = false;
			const FActiveGameplayEffectsContainer& ActiveEffects = GetActiveGameplayEffects();
			for (FActiveGameplayEffectsContainer::ConstIterator It = ActiveEffects.CreateConstIterator(); It; ++It)
			{
				if (It->Spec.Def && It->Spec.Def->GetClass() == EffectClass)
				{
					bAlreadyActive = true;
					break;
				}
			}

			if (bAlreadyActive)
			{
				UE_LOGFMT(LogASC, Warning, "이펙트 {0} 이미 활성화됨", *EffectClass->GetName());
				continue;
			}
		}

		// 이펙트 적용
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (ActiveHandle.IsValid())
			{
				SuccessCount++;
				UE_LOGFMT(LogASC, Log, "이펙트 {0} 적용 성공", *EffectClass->GetName());
			}
		}
	}

	UE_LOGFMT(LogASC, Log, "이펙트 적용 완료: {0}/{1}", SuccessCount, Effects.Num());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FGameplayTag UBaseAbilitySystemComponent::GetGameplayTagFromString(const FString& TagString)
{
	//@TagString 유효성 검사
	if (TagString.IsEmpty())
	{
		UE_LOGFMT(LogASC, Warning, "GetGameplayTagFromString: 빈 문자열입니다");
		return FGameplayTag::EmptyTag;
	}

	//@FString을 FName으로 변환 후 FGameplayTag 요청
	FName TagName = FName(*TagString);
	FGameplayTag ResultTag = FGameplayTag::RequestGameplayTag(TagName);

	if (!ResultTag.IsValid())
	{
		UE_LOGFMT(LogASC, Warning, "GetGameplayTagFromString: 유효한 GameplayTag를 찾을 수 없음 - {0}",
			*TagString);
		return FGameplayTag::EmptyTag;
	}

	UE_LOGFMT(LogASC, Log, "GetGameplayTagFromString: 성공 - {0}", *ResultTag.ToString());
	return ResultTag;
}

void UBaseAbilitySystemComponent::GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel)
{
	if (AbilityTagRelationshipMapping.Get())
	{
		AbilityTagRelationshipMapping.Get()->GetAbilityTagsToBlockAndCancel(AbilityTags, &OutAbilityTagsToBlock, &OutAbilityTagsToCancel);
	}
}

bool UBaseAbilitySystemComponent::CreateChainEventPayload(const FGameplayTag& EventTag, OUT FGameplayEventData& OutEventData)
{
	UE_LOGFMT(LogASC, Log, "체인 이벤트 Payload 구성 시도 - EventTag: {0}", *EventTag.ToString());

	// 체인 액션에서 매칭되는 EventTag 찾기 (Active Chain)
	for (const auto& ChainPair : ActiveChainActions)
	{
		const FGameplayTag& AbilityTag = ChainPair.Key;
		const TArray<FChainActionMapping>& ChainMappings = ChainPair.Value;

		for (const FChainActionMapping& ChainMapping : ChainMappings)
		{
			// 해당 EventTag와 매칭되는 체인 매핑 찾기
			if (ChainMapping.EventTag == EventTag)
			{
				UE_LOGFMT(LogASC, Log, "Active 체인 매핑 발견 - Ability: {0}, EventTag: {1}",
					*AbilityTag.ToString(), *EventTag.ToString());

				// 저장된 체인 정보의 유효성 검사
				bool bHasValidInstigator = ChainMapping.ChainInstigator.IsValid();
				bool bHasValidTarget = ChainMapping.ChainTarget.IsValid();
				bool bHasValidContext = ChainMapping.ChainContextHandle.IsValid();

				UE_LOGFMT(LogASC, Log, "체인 데이터 유효성 검사 - Instigator: {0}, Target: {1}, Context: {2}",
					bHasValidInstigator, bHasValidTarget, bHasValidContext);

				// 최소 요구사항: Instigator 또는 Target 중 하나는 유효해야 함
				if (!bHasValidInstigator && !bHasValidTarget)
				{
					UE_LOGFMT(LogASC, Warning, "체인 Payload 구성 실패 - Instigator와 Target이 모두 유효하지 않음");
					return false;
				}

				// FGameplayEventData 구성
				OutEventData.EventTag = EventTag;
				OutEventData.Instigator = ChainMapping.ChainInstigator.Get();
				OutEventData.Target = ChainMapping.ChainTarget.Get();
				OutEventData.ContextHandle = ChainMapping.ChainContextHandle;

				// 추가 로깅: 구성된 데이터 정보
				UE_LOGFMT(LogASC, Log, "Active 체인 Payload 구성 완료 - Instigator: {0}, Target: {1}",
					OutEventData.Instigator.Get() ? *OutEventData.Instigator->GetName() : TEXT("없음"),
					OutEventData.Target.Get() ? *OutEventData.Target->GetName() : TEXT("없음"));

				return true;
			}
		}
	}

	// 체인 이벤트에서 매칭되는 EventTag 찾기 (Passive Chain)
	for (const auto& ChainPair : ActiveChainEvents)
	{
		const FGameplayTag& AbilityTag = ChainPair.Key;
		const TArray<FChainEventMapping>& ChainEventMappings = ChainPair.Value;

		for (const FChainEventMapping& ChainEventMapping : ChainEventMappings)
		{
			// 해당 EventTag와 매칭되는 체인 이벤트 매핑 찾기
			if (ChainEventMapping.EventTagToSend == EventTag)
			{
				UE_LOGFMT(LogASC, Log, "Passive 체인 매핑 발견 - Ability: {0}, EventTag: {1}",
					*AbilityTag.ToString(), *EventTag.ToString());

				// 저장된 체인 정보의 유효성 검사
				bool bHasValidInstigator = ChainEventMapping.ChainInstigator.IsValid();
				bool bHasValidTarget = ChainEventMapping.ChainTarget.IsValid();
				bool bHasValidContext = ChainEventMapping.ChainContextHandle.IsValid();

				UE_LOGFMT(LogASC, Log, "체인 데이터 유효성 검사 - Instigator: {0}, Target: {1}, Context: {2}",
					bHasValidInstigator, bHasValidTarget, bHasValidContext);

				// 최소 요구사항: Instigator 또는 Target 중 하나는 유효해야 함
				if (!bHasValidInstigator && !bHasValidTarget)
				{
					UE_LOGFMT(LogASC, Warning, "체인 Payload 구성 실패 - Instigator와 Target이 모두 유효하지 않음");
					return false;
				}

				// FGameplayEventData 구성
				OutEventData.EventTag = EventTag;
				OutEventData.Instigator = ChainEventMapping.ChainInstigator.Get();
				OutEventData.Target = ChainEventMapping.ChainTarget.Get();
				OutEventData.ContextHandle = ChainEventMapping.ChainContextHandle;

				// 추가 로깅: 구성된 데이터 정보
				UE_LOGFMT(LogASC, Log, "Passive 체인 Payload 구성 완료 - Instigator: {0}, Target: {1}",
					OutEventData.Instigator.Get() ? *OutEventData.Instigator->GetName() : TEXT("없음"),
					OutEventData.Target.Get() ? *OutEventData.Target->GetName() : TEXT("없음"));

				return true;
			}
		}
	}

	// 매칭되는 체인 매핑을 찾지 못한 경우
	UE_LOGFMT(LogASC, Log, "체인 관련 EventTag가 아님 - EventTag: {0}", *EventTag.ToString());
	return false;
}

void UBaseAbilitySystemComponent::GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{

	if (AbilityTagRelationshipMapping)
	{
		AbilityTagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, OutActivationRequired, OutActivationBlocked);
	}
}
#pragma endregion