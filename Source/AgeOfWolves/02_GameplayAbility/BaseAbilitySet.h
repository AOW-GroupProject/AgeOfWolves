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
 *	Data used by the ability set to grant gameplay abilities.
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

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
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
 * FAbilitySet_AttributeSet
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
 * FLyraAbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
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
		TArray<FBaseAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
		TArray<FBaseAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
		TArray<FBaseAbilitySet_AttributeSet> GrantedAttributes;
};
