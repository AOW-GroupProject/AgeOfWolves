#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

#include "BaseAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASC, Log, All);

//@���� ����
#pragma region Forward Declaration
class UANS_AllowChainAction;
#pragma endregion

//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
//@�����Ƽ ���� ��� �Ϸ� �̺�Ʈ
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitySpecGiven, FGameplayAbilitySpec)

DECLARE_DYNAMIC_DELEGATE_OneParam(FChainActionActivated, FGameplayTag, ChainActionAbilityTag);
#pragma endregion

/**	
 *	@UBaseAbilitySystemComponent
 * 
 *	AOW�� ĳ���� �⺻ ASC �����Դϴ�.
 */	
UCLASS(meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UBaseAbilitySystemComponent : public UAbilitySystemComponent
{

//@ģ�� Ŭ����
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
	//@���� ���ε�

protected:
	//@�ܺ� ���ε�

protected:
	//@�ʱ�ȭ
	virtual void InitializeComponent() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@�����ε�
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
	//@Cancel �۾�
	virtual void CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore) override;
	//@Block �Ǿ��� Passive GA�� �� Ȱ��ȭ �۾�
	void ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags);

public:
	//@Block, Cancel �±׿� �ش�Ǵ� GA�鿡 ���Ͽ� Block, Cancel ����
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

protected:
	//@Gameplay Event�� ���� GA Ȱ��ȭ �۾� ó��
	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;
	//@Damage Event �߻� �� Armor �����Ƽ Ȱ��ȭ
	bool TriggerDamageEvent(const FGameplayTag& EventTag, const FGameplayEventData* Payload);

protected:
	//@Chain System Ȱ��ȭ
	void StartChainWindow();
	//@Chain Sytsem ����
	void EndChainWindow();

protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;

protected:
	//@���� Ȱ��ȭ ���� Ability ���� Tag
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
	//@�����Ƽ ��� �̺�Ʈ
	FAbilitySpecGiven AbilitySpecGiven;

public:
	//@ü�� �׼� Ȱ��ȭ �̺�Ʈ
	FChainActionActivated ChainActionActivated;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@GA Ȱ��ȭ �̺�Ʈ ����
	void OnAbilityActivated(UGameplayAbility* Ability);
	//@GA ���� �̺�Ʈ ����
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