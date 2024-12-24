// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"

#include "BaseAbilitySet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilitySet, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAttributeSet;
class UBaseGameplayAbility;
class UGameplayEffect;
class UAbilityTagRelationshipMapping;
class UBaseAbilitySystemComponent;
class APlayerStateBase;
class UInputConfig;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
/**
 *	@FAbilitySet_GameplayAbility
 *
 *	Gameplay Ability 와 함께 관리할 정보들을 정의한 구조체
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	//@어빌리티 태그
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag AbilityTag;
	/// @어빌리티 유형의 BP 클래스
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UBaseGameplayAbility> Ability = nullptr;

	/// @어빌리티의 아이콘 이미지
	UPROPERTY(EditDefaultsOnly)
		UTexture2D* AbilityIconImage;

	/// @어빌리티 레벨
	UPROPERTY(EditDefaultsOnly)
		int32 AbilityLevel = 1;

	/// @Active/Passive 여부
	UPROPERTY(EditDefaultsOnly)
		bool bActive = false;

	/// @사용자 입력 바인딩 여부(체크를 위해 bActive가 참이여만 합니다)
	UPROPERTY(EditDefaultsOnly)
		bool bInputBinded = false;

	/// @입력 태그
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag", EditCondition = "bActive&&bInputBinded"))
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
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion


/**
*	@UBaseAbilitySet
* 
*	Attribute Set, GA, GE 목록을 담는 Data Asset
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
	UPROPERTY(EditDefaultsOnly, Category = "캐릭터 태그")
		FGameplayTag CharacterTag;

protected:
	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "어트리뷰트 목록")
		TArray<FBaseAbilitySet_AttributeSet> AttributeSets;

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "GA 목록")
		TArray<FBaseAbilitySet_GameplayAbility> GameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "GE 목록")
		TArray<FBaseAbilitySet_GameplayEffect> GameplayEffects;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 관계성")
		TObjectPtr<UAbilityTagRelationshipMapping> TagRelationship;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "입력")
		TObjectPtr<UInputConfig> InputConfig;

public:
	FGameplayTag GetCharacterTag() const { return CharacterTag; }

public:
	TArray<FBaseAbilitySet_AttributeSet> GetAttributeSets() const { return AttributeSets; }
	const TArray<FBaseAbilitySet_GameplayAbility>& GetGameplayAbilities() const { return GameplayAbilities; }
	TArray<FBaseAbilitySet_GameplayEffect> GetGameplayEffects() const { return GameplayEffects; }
	UAbilityTagRelationshipMapping* GetATMR() const{ return TagRelationship; }
	UInputConfig* GetInputConfig() const { return InputConfig; }

};
