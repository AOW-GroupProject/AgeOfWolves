#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"
#include "04_Component/InteractionComponent.h"

#include "BaseAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASC, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UANS_AllowChainAction;
class UBaseAttributeSet;
class ABaseAIController;
class APlayerStateBase;

struct FDeathInformation;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@어빌리티 스펙 등록 완료 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitySpecGiven, FGameplayAbilitySpec)

//@어빌리티 활성화/종료 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityActivated, UGameplayAbility*);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityEnded, UGameplayAbility*);

//@어빌리티 취소 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityCancelled, UGameplayAbility*);

//@연결 동작 활성화 이벤트
DECLARE_DYNAMIC_DELEGATE_OneParam(FChainActionActivated, FGameplayTag, ChainActionAbilityTag);
//@연결 동작 활성화 종료 이벤트
DECLARE_DYNAMIC_DELEGATE_OneParam(FChainActionFinished, FGameplayTag, ChainActionAbilityTag);

//@상태 변화 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FCharacterStateEventOnGameplay, AActor*, const FGameplayTag&);

//@상호작용 활성화 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionActivated, AActor*, InteractableActor, const FPotentialInteraction&, PotentialInteraction);
//@상호작용 실패 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionFailed, AActor*, InteractableActor, const FPotentialInteraction&, FailedInteraction);
//@상호작용 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionCompleted, AActor*, InteractableActor, const FPotentialInteraction&, CompletedInteraction);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FDamageDealtByActor, AActor* /* Source */, AActor* /* Target */, const FGameplayEventData& /* EventData */)
#pragma endregion

/**
 *	@UBaseAbilitySystemComponent
 *
 *	AOW의 캐릭터 기본 ASC 유형입니다.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UBaseAbilitySystemComponent : public UAbilitySystemComponent
{

//@친추 클래스
#pragma region Friend Class
	friend class UBaseGameplayAbility;
	friend class UANS_AllowChainAction;
	friend class ABaseAIController;
	friend class APlayerStateBase;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩
	void ExternalBindToPlayerState(APlayerStateBase* PlayerState);
	void ExternalBindToAIAbilitySequencer(ABaseAIController* BaseAIC);
	void ExternalBindToAIController(ABaseAIController* BaseAIC);
	void ExternalBindToInteractionComp(AController* Controller);
	void ExternalBindToGameState();

protected:
	//@초기화
	virtual void InitializeComponent() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@오버로딩
	FGameplayAbilitySpecHandle GiveAbility(const FGameplayAbilitySpec& AbilitySpec);

public:
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:
	//@Cancel 작업
	virtual void CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore) override;
	//@Block 되었던 Passive GA의 재 활성화 작업
	void ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags);

public:
	//@Block, Cancel 태그에 해당되는 GA들에 대하여 Block, Cancel 적용
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

protected:
	//@Gameplay Event에 의한 GA 활성화 작업 처리
	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;

protected:
	//@Death Event 발생 처리
	void HandleCharacterDead();

protected:
	//@Chain System 시작
	UFUNCTION(BlueprintCallable, Category = "Chain System")
		void StartChainWindowWithTag(const FGameplayTag& AbilityTagWaitingChainAction, FGameplayTag InputTagToChain);

	//@Chain Sytsem 종료
	UFUNCTION(BlueprintCallable, Category = "Chain System")
		void EndChainWindow(const FGameplayTag& AbilityTag);

	void EndChainWindow(const FGameplayTag& AbilityTag, const FGameplayEventData* Payload);

protected:
	// 상호작용 시스템 시작
	void StartInteractionWindow(AActor* TargetActor, const FPotentialInteraction& Interaction);

	// 상호작용 시스템 종료
	UFUNCTION(BlueprintCallable, Category = "Interaction System")
		void EndInteractionWindow(bool bSuccess = false);

	// 상호작용 이벤트 처리
	void HandleInteractionEvent(const FGameplayTag& EventTag);

protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;

protected:
	//@현재 활성화 중인 Ability 들의 Tag
	FGameplayTagContainer ActivatingAbilityTags;

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

protected:
	//@입력 해제로 활성화되어야 하지만 블록된 어빌리티들의 예약 목록
	TArray<FGameplayAbilitySpecHandle> PendingReleaseAbilities;

protected:
	//@체인 시스템 활성화 여부
	bool bChainWindowActive;

	//@체인 액션 허용 여부
	TMap<FGameplayTag, TMap<FGameplayTag, bool>> ChainActionAllowedMap;

protected:
	//@체인 액션 대기 상태의 어빌리티와 해당 어빌리티에 대한 체인 액션 매핑 목록 (복수 지원)
	TMap<FGameplayTag, TArray<FChainActionMapping>> ActiveChainActions;

	//@체인 이벤트 대기 상태의 어빌리티와 해당 어빌리티에 대한 체인 이벤트 매핑 목록 (복수 지원)
	TMap<FGameplayTag, TArray<FChainEventMapping>> ActiveChainEvents;

protected:
	//@현재 가능한 상호작용 정보
	UPROPERTY()
		FPotentialInteraction CurrentPotentialInteraction;

	//@상호작용 가능 상태
	UPROPERTY()
		bool bInteractionAvailable;

	//@상호작용 타겟 액터
	UPROPERTY()
		TWeakObjectPtr<AActor> InteractionTargetActor;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@어빌리티 등록 이벤트
	FAbilitySpecGiven AbilitySpecGiven;

public:
	//@어빌리티 활성화 이벤트
	FAbilityActivated AbilityActivated;
	//@어빌리티 종료 이벤트
	FAbilityEnded AbilityEnded;
	//@어빌리티 취소 이벤트
	FAbilityCancelled AbilityCancelled;

public:
	//@체인 액션 활성화 이벤트
	FChainActionActivated ChainActionActivated;
	//@체인 액션 종료 이벤트
	FChainActionFinished ChainActionFinished;

public:
	//@캐릭터의 게임 플레이 과정에서 발생하는 주요 이벤트
	FCharacterStateEventOnGameplay CharacterStateEventOnGameplay;

public:
	// 상호작용 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Interaction System")
		FInteractionActivated InteractionActivated;

	UPROPERTY(BlueprintAssignable, Category = "Interaction System")
		FInteractionFailed InteractionFailed;

	UPROPERTY(BlueprintAssignable, Category = "Interaction System")
		FInteractionCompleted InteractionCompleted;

public:
	//@데미지 전달 이벤트
	FDamageDealtByActor DamageDealtByActor;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@GA 활성화 이벤트 구독
	void OnAbilityActivated(UGameplayAbility* Ability);
	//@GA 종료 이벤트 구독
	virtual void OnAbilityEnded(UGameplayAbility* Ability);
	//@GA 실패 이벤트 구독 
	void OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& Tags);

protected:
	void OnGameplayEffectApplied(
		UAbilitySystemComponent* Source,
		const FGameplayEffectSpec& SpecApplied,
		FActiveGameplayEffectHandle ActiveHandle);

protected:
	//@태그 기반 어빌리티 활성화 요청
	UFUNCTION()
		bool OnRequestActivateAbilityBlockUnitByAI(const FGameplayTag& AbilityTag);

protected:
	UFUNCTION()
		void OnPotentialInteractionChanged(AActor* TargetActor, const FPotentialInteraction& Interaction);

protected:
	UFUNCTION()
		void OnCrowdControlEventTriggered(const FGameplayTag& CrowControlTag);

private:
	//@Game State 리스폰 완료 콜백 함수
	UFUNCTION()
	void OnPlayerRespawnCompleted(APlayerController* RespawnedPlayerController);

private:
	//@어빌리티 등록 요청 이벤트
	UFUNCTION()
		void OnRequestGrantAbilities(const TArray<TSubclassOf<UBaseGameplayAbility>>& Abilities, const FGameplayTag& ItemTag, bool bAllowDuplicate);
	UFUNCTION()
		void OnRequestActivateAbilities(const TArray<TSubclassOf<UBaseGameplayAbility>>& Abilities, const FGameplayTag& ItemTag, bool bForceActivate);
	//@이팩트 적용 요청 이벤트
	UFUNCTION()
		void OnRequestApplyEffects(const TArray<TSubclassOf<UGameplayEffect>>& Effects, const FGameplayTag& ItemTag, bool bAllowDuplicate);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	//@FString으로 FGameplayTag 생성
	UFUNCTION(BlueprintPure, Category = "Gameplay Tags")
	static FGameplayTag GetGameplayTagFromString(const FString& TagString);

public:
	//@Cancel, Block Tag
	void GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel);
	//@AR, AB Tag
	void GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const;

public:
	FORCEINLINE void GetActivatingAbilityTags(OUT FGameplayTagContainer& OutGameplayTagContainer) const { OutGameplayTagContainer = ActivatingAbilityTags; }

public:
	FORCEINLINE void SetAbilityTagRelationshipMapping(UAbilityTagRelationshipMapping* ATRM) { AbilityTagRelationshipMapping = ATRM; }

protected:
	/**
	 * 체인 관련 이벤트의 Payload를 구성하는 헬퍼 함수
	 * @param EventTag - 처리할 이벤트 태그
	 * @param OutEventData - 구성된 이벤트 데이터 (출력)
	 * @return 체인 관련 이벤트이고 유효한 Payload를 구성했는지 여부
	 */
	bool CreateChainEventPayload(const FGameplayTag& EventTag, OUT FGameplayEventData& OutEventData);

public:
	FORCEINLINE bool IsChainWindowActive() const { return bChainWindowActive; }

public:
	FORCEINLINE bool IsInteractionAvailable() const { return bInteractionAvailable; }
	FORCEINLINE const FPotentialInteraction& GetCurrentPotentialInteraction() const { return CurrentPotentialInteraction; }
	FORCEINLINE AActor* GetInteractionTargetActor() const { return InteractionTargetActor.Get(); }

private:
	FString CleanStateTagName(const FString& OriginalTagName);
#pragma endregion

};