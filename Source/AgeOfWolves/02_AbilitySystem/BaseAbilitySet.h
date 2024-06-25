// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"

#include "BaseAbilitySet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilitySet, Log, All)

class UBaseAttributeSet;
class UBaseGameplayAbility;
class UGameplayEffect;
class UBaseAbilitySystemComponent;
class APlayerStateBase;

/**
 * FAbilitySet_GameplayAbility
 *
 *	@목적: GA 관련 구조체입니다. 모든 GA는 고유의 Gameplay Tag를 갖습니다. 
 *	
 *	@설명: 해당 구조체는 그 특징에 따라서 두 개의 종류가 되며, 구분점은 IsAcive(bool형)입니다.
 * 
 *	1. Passive GA: 외부 환경 혹은 다른 GA를 통해 활성화되며, GE 구현에 중점을 두어 캐릭터의 Attribute 수치 변화 혹은 다른 GA의 '행동 제어'의 기반이 됩니다.
 *	-> Trigger 매개가 다른 GA 혹은 외부 환경입니다.
 *	-> Input Tag 관련 정보가 필요없습니다.
 *	-> 별도의 활성화 함수를 블루프린트에서 정의하고, 캐릭터의 Attribute 수치 변화에 중점을 두어 다른 GA의 '행동 제어'를 목표합니다.
 * 
 *	2. Active GA: 사용자 입력에 의해 활성화되어, GA 구현에 중점을 두어 캐릭터의 특정 '행동 개시'를 정의합니다.
 *	-> Trigger 매개가 사용자 입력입니다.
 *	-> Input Tag 관련 정보가 필요합니다.
 *	-> 별도의 활성화 함수를 블루프린트에서 정의하고, GA 구현에 중점을 두어 GE를 통한 캐릭터의 Attribute 수치 변화에 부가적인 영향을 끼쳐 캐릭터의 '행동 개시'의 기반이 됩니다. 
 * 
 *	@참고: Native Action으로 활성화 함수가 정의되는 GA들의 경우, 특정 GE에 의해 모두가 함께 영향을 받거나, 영향을 받지 않을 수 있습니다.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UBaseGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
		int32 AbilityLevel = 1;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
		bool IsActive = false;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag", EditCondition = "IsActive==true"))
		FGameplayTag InputTag;
};

/**
 * FAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
		float EffectLevel = 1.0f;
};

/**
 * FBaseAbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Attribute Set
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UBaseAttributeSet> AttributeSet;

};

/**
 * FBaseAbilitySet_GrantedHandles
 *
 *	@목적 : GA, GE, 그리고 AttributeSet의 ASC 등록 이벤트 관련 핸들을 관리하는 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UBaseAttributeSet* Set);

	//void TakeFromAbilitySystem(ULyraAbilitySystemComponent* ASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
		TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
		TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
		TArray<TObjectPtr<UBaseAttributeSet>> GrantedAttributeSets;
};

/**
 * @목적	: 사용자 캐릭터가 부여받을 Gameplay Ability System 관련 정보들을 한데 묶어 놓은 Data Asset입니다.
 * @설명	: Gameplay Ability, Gameplay Effect, 그리고 Attribute Set과 각 항목에 대한 부가 정보들을 구조체 형식으로 한데 묶어 놓습니다.
 * @참조 : APlayerStateBase::InitializeGameplayAbilitySystem()
 */
UCLASS()
class AGEOFWOLVES_API UBaseAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UBaseAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* @목적 : 사용자 캐릭터의 기본 AttributeSet, Gameplay Ability, Gameplay Effect를 해당 캐릭터 객체의 ASC에 등록하는 함수들입니다.
	* @설명 : 각자의 방식으로 ASC에 등록한 후, 해당 등록 이벤트 관련한 Out Param으로 전달 받은 GrantHandle을 반환합니다.
	* @참조 : -
	*/
	void GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;
	void GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;
	void GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = GameplayAbility))
		TArray<FBaseAbilitySet_GameplayAbility> GameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
		TArray<FBaseAbilitySet_GameplayEffect> GameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
		TArray<FBaseAbilitySet_AttributeSet> AttributeSets;
};
