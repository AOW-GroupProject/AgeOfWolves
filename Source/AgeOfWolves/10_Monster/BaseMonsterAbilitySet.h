// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"


#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"

#include "BaseMonsterAbilitySet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMonsterAbilitySet, Log, All)


class UBaseMonsterAttributeSet;
class UBaseGameplayAbility;
class UGameplayEffect;
class UBaseMonsterASC;

USTRUCT(BlueprintType)
struct FBaseMonsterAbilitySet_GameplayAbility
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

USTRUCT(BlueprintType)
struct FBaseMonsterAbilitySet_GameplayEffect
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
struct FBaseMonsterAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Attribute Set
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBaseMonsterAttributeSet> AttributeSet;

};



/**

 
 */
UCLASS()
class AGEOFWOLVES_API UBaseMonsterAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/*
	* @목적 : 사용자 캐릭터의 기본 AttributeSet, Gameplay Ability, Gameplay Effect를 해당 캐릭터 객체의 ASC에 등록하는 함수들입니다.
	* @참조 : -
	*/
	void GiveStartupAttributeSetToAbilitySystem(UBaseMonsterASC* ASC, UObject* SourceObject) const;
	void GiveStartupGameplayEffectToAbilitySystem(UBaseMonsterASC* ASC, UObject* SourceObject) const;
	void GiveStartupGameplayAbilityToAbilitySystem(UBaseMonsterASC* ASC, UObject* SourceObject) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = GameplayAbility))
	TArray<FBaseMonsterAbilitySet_GameplayAbility> GameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FBaseMonsterAbilitySet_GameplayEffect> GameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
	TArray<FBaseMonsterAbilitySet_AttributeSet> AttributeSets;
};
