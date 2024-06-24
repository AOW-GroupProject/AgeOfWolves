// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AbilityTagRelationshipMapping.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogATRM, Log, All);

/*
* FAbilityTagRelationship
* 
* @목적: GA의 활성화 조건을 정의하는 외부 Gameplay Tag와의 관계성을 정의합니다.
* @설명: Block, Cancel, Activation Required, Activation Blocked, 4 종류의 Gameplay Tag들을 통해 해당 GA와 다른 GA 간의 관계성을 정의합니다.
*/
USTRUCT()
struct FAbilityTagRelationship
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Ability Tag")
		FGameplayTag AbilityTag;

	/*
	* @목적:	다른 GA 중 해당 Gameplay Tag를 갖는 GA의 활성화를 Block 합니다.
	* @설명:	Block Gameplay Tag GA 활성화 시, Block된 GA가 이미 활성화 되어 관련 이벤트를 처리중이라면 해당 내용은 계속 진행되지만,
	*		이후 발생하는 추가적인 활성화 이벤트가 중지됩니다.
	* @참고:	Block -> Cancel 순서로 GA에 대한 제제 강도가 강해집니다.
	*/
	UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Block Ability Tag")
		FGameplayTagContainer AbilityTagsToBlock;

	/*
	* @목적:	다른 GA 중 해당 Gameplay Tag를 갖는 GA의 활성화를 Cancel 합니다.
	* @설명:	Cancel Gameplay Tag 관련 GA의 활성화 시, Cancel된 GA가 이미 활성화 되어 관련 이벤트를 처리중이라면 해당 내용은 중지되고,
	*		이후 발생하는 추가적인 활성화 이벤트 또한 중지됩니다.
	* @참고:	Block -> Cancel 순서로 GA에 대한 제제 강도가 강해집니다.
	*/
	UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Cancel Ability Tag")
		FGameplayTagContainer AbilityTagsToCancel;

	/*
	* @목적:	다른 GA 중 해당 Gameplay Tag를 갖는 GA가 모두 활성화 되어야만, 해당 GA를 활성화 할 수 있습니다.
	*/
	UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Activation Required Ability Tag")
		FGameplayTagContainer ActivationRequiredTags;

	/*
	* @목적: 다른 GA 중 해당 Gameplay Tag를 갖는 GA가 활성화되어 있다면, 해당 GA를 활성화 할 수 없습니다.
	*/
	UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Activation Blocked Ability Tag")
		FGameplayTagContainer ActivationBlockedTags;
};

/**
 * UAbilityTagRelationshipMapping
 * 
 * @목적: 각 GA의 여타 다른 GA와의 관계성을 정의합니다.
 * @설명
 *		1. "AbilityTagsToBlock": 해당 GA 활성화 시 활성화 작업을 미연에 방지할 GA 목록
 *		2. "AbilityTagsToCancel": 해당 GA 활성화 시 현재 활성화 중인 GA들 중 활성화 작업 중단할 GA 목록
 *		3. "ActivationRequiredTags": 해당 GA 활성화에 필요한 현자 활성화 중인 GA 목록
 *		4. "ActivationBlockedTags": 해당 GA 활성화를 Block 하는 현재 활성화 중인 GA 목록
 * @참고: 관계성은 "양방향"으로 정의하며, 'A' Ability가 "AbilityTagsToBlock"에 'B' Ability를 추가했다면, 'B' Ability는 "ActivationBlockedTags"에 'A' Ability를 추가해야 합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()
	
private:
	/*
	* FAbilityTagRelationship
	*
	* @목적: GA의 활성화 조건을 정의하는 외부 Gameplay Tag와의 관계성을 정의한 FAbilityTagRelationship 목록
	* @설명: Block, Cancel, Activation Required, Activation Blocked, 4 종류의 Gameplay Tag들을 통해 해당 GA와 다른 GA 간의 관계성을 정의합니다.
	*/
	UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
		TArray<FAbilityTagRelationship> AbilityTagRelationships;
public:
	/*
	* @목적: AbilityTagRelationships 목록을 순회하며 Block/Cancel할 GA들의 Gameplay Tag 목록을 반환합니다.
	* @설명: Ability Tag 목록을 가져와서 "AbilityTagRelationships" 목록과 비교하여 "Block"혹은 "Cancel"할 Ability Tag를 찾아서 반환합니다.
	* @참고: OutParameter 사용
	*/
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutTagsToBlock, OUT FGameplayTagContainer* OutTagsToCancel) const;

	/*
	* @목적: AbilityTagRelationship 목록을 순회하며 Activation Required/Blocked GA들의 Gameplay Tag 목록을 반환합니다.
	* @설명: Ability Tag 목록을 가져와서 "AbilityTagRelationships" 목록과 비교하여 "Activation Required" 혹은 "Activation Blocked" Abiility Tag를 찾아서 반환합니다.
	* @참고: OutParameter 사용
	*/
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const;

	/*
	* @목적: 현재 처리 중인 Ability 중 다른 Abilit Tag에 의하여 "Cancel" Ability Tag로 지정된 Ability의 처리 이벤트를 중단시킵니다.
	*/
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};
