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
DECLARE_MULTICAST_DELEGATE_TwoParams(FCharacterStateEventOnGameplay, AActor*, const FGameplayTag&)

//@상호작용 허용 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionAvailable, AActor*, InteractableActor, const FPotentialInteraction&, PotentialInteraction);
// 상호작용 불가능 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionUnavailable, AActor*, InteractableActor, const FPotentialInteraction&, PotentialInteraction);
// 상호작용 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionCompleted, AActor*, InteractableActor, const FPotentialInteraction&, CompletedInteraction);
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
	void ExternalBindToAIAbilitySequencer(ABaseAIController* BaseAIC);
	void ExternalBindToInteractionComp(AController* Controller);

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
		void StartChainWindowWithTag(const FGameplayTag& InAbilityToBindTag, FGameplayTag InTagToChain);

	//@Chain Sytsem 종료
	UFUNCTION(BlueprintCallable, Category = "Chain System")
		void EndChainWindow();

	void EndChainWindow(const FGameplayEventData* Payload);

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
	//@체인 시스템 활성화 여부
	bool bChainWindowActive;
	//@체인 액션 허용 여부
	bool bCanChainAction;
	//@체인 액션 시작한 원본 어빌리티
	FGameplayTag OriginAbilityTag;
	//@체인 액션 실행 모드
	EChainActionMode CurrentChainMode;
	//@허용 받은 체인 액션들
	TArray<FChainActionMapping> AllowedChainMappings;
	//@허용 받은 체인 이벤트들
	TArray<FChainEventMapping> AllowedChainEventMappings;
	//@다음 실행할 체인 액션의 이벤트 태그
	FGameplayTag ChainActionEventTag;

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
		FInteractionAvailable InteractionAvailable;

	UPROPERTY(BlueprintAssignable, Category = "Interaction System")
		FInteractionUnavailable InteractionUnavailable;

	UPROPERTY(BlueprintAssignable, Category = "Interaction System")
		FInteractionCompleted InteractionCompleted;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@GA 활성화 이벤트 구독
	void OnAbilityActivated(UGameplayAbility* Ability);
	//@GA 종료 이벤트 구독
	virtual void OnAbilityEnded(UGameplayAbility* Ability);

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
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	//@Cancel, Block Tag
	void GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel);
	//@AR, AB Tag
	void GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const;

public:
	FORCEINLINE void GetActivatingAbilityTags(OUT FGameplayTagContainer& OutGameplayTagContainer) const { OutGameplayTagContainer = ActivatingAbilityTags; }

public:
	FORCEINLINE void SetAbilityTagRelationshipMapping(UAbilityTagRelationshipMapping* ATRM) { AbilityTagRelationshipMapping = ATRM; }

public:
	FORCEINLINE bool IsChainWindowActive() const { return bChainWindowActive; }
	FORCEINLINE bool CanChainAction() const { return bCanChainAction; }

public:
	FORCEINLINE bool IsInteractionAvailable() const { return bInteractionAvailable; }
	FORCEINLINE const FPotentialInteraction& GetCurrentPotentialInteraction() const { return CurrentPotentialInteraction; }
	FORCEINLINE AActor* GetInteractionTargetActor() const { return InteractionTargetActor.Get(); }
#pragma endregion
};