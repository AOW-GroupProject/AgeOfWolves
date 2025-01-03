#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

#include "BaseAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASC, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UANS_AllowChainAction;
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

DECLARE_DYNAMIC_DELEGATE_OneParam(FChainActionActivated, FGameplayTag, ChainActionAbilityTag);
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
	friend class UAttackGameplayAbility;
	friend class UANS_AllowChainAction;
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
	//@Damage Event 발생 시 Armor 어빌리티 활성화
	bool TriggerDamageEvent(const FGameplayTag& EventTag, const FGameplayEventData* Payload);

protected:
	//@Chain System 활성화
	void StartChainWindow();
	//@Chain Sytsem 종료
	void EndChainWindow();

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
	bool bChainWindowActive;
	FGameplayTagContainer AllowedAbilityTags;
	FGameplayTag StoredChainabilityTag;

#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@어빌리티 등록 이벤트
	FAbilitySpecGiven AbilitySpecGiven;

public:
	//@체인 액션 활성화 이벤트
	FChainActionActivated ChainActionActivated;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@GA 활성화 이벤트 구독
	void OnAbilityActivated(UGameplayAbility* Ability);
	//@GA 종료 이벤트 구독
	virtual void OnAbilityEnded(UGameplayAbility* Ability);
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
#pragma endregion
};