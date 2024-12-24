// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

#include "BaseAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASC, Log, All);

//@���� ����
#pragma region Forward Declaration
#pragma endregion

//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitySpecGiven, FGameplayAbilitySpec)
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
	UPROPERTY(EditAnywhere)
		TObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;

protected:
	//@���� Ȱ��ȭ ���� Ability ���� Tag
	FGameplayTagContainer ActivatingAbilityTags;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@�����Ƽ ��� �̺�Ʈ
	FAbilitySpecGiven AbilitySpecGiven;
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