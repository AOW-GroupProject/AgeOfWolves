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

    UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Block Ability Tag")
        FGameplayTagContainer AbilityTagsToBlock;

    UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Cancel Ability Tag")
        FGameplayTagContainer AbilityTagsToCancel;

    UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Activation Required Ability Tag")
        FGameplayTagContainer ActivationRequiredTags;

    UPROPERTY(EditAnywhere, Category = "Ability Tag Relationship | Activation Blocked Ability Tag")
        FGameplayTagContainer ActivationBlockedTags;
};

/**
 * UAbilityTagRelationshipMapping
 */
UCLASS()
class AGEOFWOLVES_API UAbilityTagRelationshipMapping : public UDataAsset
{
    GENERATED_BODY()

private:
    // 에디터에서 설정할 원본 데이터
    UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
        TArray<FAbilityTagRelationship> AbilityTagRelationships;

    // 캐시된 맵 데이터
    TMap<FGameplayTag, FGameplayTagContainer> AbilityTagsToBlockMap;
    TMap<FGameplayTag, FGameplayTagContainer> AbilityTagsToCancelMap;
    TMap<FGameplayTag, FGameplayTagContainer> ActivationRequiredTagsMap;
    TMap<FGameplayTag, FGameplayTagContainer> ActivationBlockedTagsMap;

    // 캐시 맵 초기화
    void InitializeCacheMaps();

public:
    // 생성자에서 캐시 맵 초기화
    UAbilityTagRelationshipMapping();

#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
        OUT FGameplayTagContainer* OutTagsToBlock,
        OUT FGameplayTagContainer* OutTagsToCancel) const;

    void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
        OUT FGameplayTagContainer* OutActivationRequired,
        OUT FGameplayTagContainer* OutActivationBlocked) const;

    bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};