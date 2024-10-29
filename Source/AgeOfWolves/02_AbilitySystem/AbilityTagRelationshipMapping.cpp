// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityTagRelationshipMapping.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogATRM)

UAbilityTagRelationshipMapping::UAbilityTagRelationshipMapping()
{
    InitializeCacheMaps();
}

void UAbilityTagRelationshipMapping::InitializeCacheMaps()
{
    // 모든 맵 초기화
    AbilityTagsToBlockMap.Empty();
    AbilityTagsToCancelMap.Empty();
    ActivationRequiredTagsMap.Empty();
    ActivationBlockedTagsMap.Empty();

    // AbilityTagRelationships 배열의 데이터로 맵 구성
    for (const FAbilityTagRelationship& Relationship : AbilityTagRelationships)
    {
        if (Relationship.AbilityTag.IsValid())
        {
            if (!Relationship.AbilityTagsToBlock.IsEmpty())
            {
                AbilityTagsToBlockMap.Add(Relationship.AbilityTag, Relationship.AbilityTagsToBlock);
            }
            if (!Relationship.AbilityTagsToCancel.IsEmpty())
            {
                AbilityTagsToCancelMap.Add(Relationship.AbilityTag, Relationship.AbilityTagsToCancel);
            }
            if (!Relationship.ActivationRequiredTags.IsEmpty())
            {
                ActivationRequiredTagsMap.Add(Relationship.AbilityTag, Relationship.ActivationRequiredTags);
            }
            if (!Relationship.ActivationBlockedTags.IsEmpty())
            {
                ActivationBlockedTagsMap.Add(Relationship.AbilityTag, Relationship.ActivationBlockedTags);
            }
        }
    }
}

#if WITH_EDITOR
void UAbilityTagRelationshipMapping::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // AbilityTagRelationships가 변경되었을 때만 캐시 맵 업데이트
    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UAbilityTagRelationshipMapping, AbilityTagRelationships))
    {
        InitializeCacheMaps();
    }
}
#endif

void UAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
    FGameplayTagContainer* OutTagsToBlock,
    FGameplayTagContainer* OutTagsToCancel) const
{
    if (OutTagsToBlock)
    {
        for (const FGameplayTag& Tag : AbilityTags)
        {
            if (const FGameplayTagContainer* BlockTags = AbilityTagsToBlockMap.Find(Tag))
            {
                OutTagsToBlock->AppendTags(*BlockTags);
            }
        }
    }

    if (OutTagsToCancel)
    {
        for (const FGameplayTag& Tag : AbilityTags)
        {
            if (const FGameplayTagContainer* CancelTags = AbilityTagsToCancelMap.Find(Tag))
            {
                OutTagsToCancel->AppendTags(*CancelTags);
            }
        }
    }
}

void UAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
    FGameplayTagContainer* OutActivationRequired,
    FGameplayTagContainer* OutActivationBlocked) const
{
    if (OutActivationRequired)
    {
        for (const FGameplayTag& Tag : AbilityTags)
        {
            if (const FGameplayTagContainer* RequiredTags = ActivationRequiredTagsMap.Find(Tag))
            {
                OutActivationRequired->AppendTags(*RequiredTags);
            }
        }
    }

    if (OutActivationBlocked)
    {
        for (const FGameplayTag& Tag : AbilityTags)
        {
            if (const FGameplayTagContainer* BlockedTags = ActivationBlockedTagsMap.Find(Tag))
            {
                OutActivationBlocked->AppendTags(*BlockedTags);
            }
        }
    }
}

bool UAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
    if (const FGameplayTagContainer* CancelTags = AbilityTagsToCancelMap.Find(ActionTag))
    {
        return CancelTags->HasAny(AbilityTags);
    }
    return false;
}