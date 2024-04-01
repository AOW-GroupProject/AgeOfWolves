// Fill out your copyright notice in the Description page of Project Settings.


#include "02_GameplayAbility/AbilityTagRelationshipMapping.h"

void UAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
    for (const auto& AbilityTagRelationship : AbilityTagRelationships)
    {
        if (AbilityTags.HasTagExact(AbilityTagRelationship.AbilityTag))
        {
            if (OutTagsToBlock->IsValid())
            {
                OutTagsToBlock->AppendTags(AbilityTagRelationship.AbilityTagsToBlock);
            }
            if (OutTagsToCancel->IsValid())
            {
                OutTagsToCancel->AppendTags(AbilityTagRelationship.AbilityTagsToCancel);
            }
        }
    }
}

void UAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{

    for (const auto& AbilityTagRelationship : AbilityTagRelationships)
    {
        if (AbilityTags.HasTagExact(AbilityTagRelationship.AbilityTag))
        {
            if (OutActivationRequired->IsValid())
            {
                OutActivationRequired->AppendTags(AbilityTagRelationship.ActivationRequiredTags);
            }
            if (OutActivationBlocked->IsValid())
            {
                OutActivationBlocked->AppendTags(AbilityTagRelationship.ActivationBlockedTags);
            }
        }
    }
}

bool UAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{

    for (const auto& AbilityTagRelationship : AbilityTagRelationships)
    {
        if (AbilityTagRelationship.AbilityTag == ActionTag && AbilityTagRelationship.AbilityTagsToCancel.HasAny(AbilityTags))
        {
            return true;
        }
    }

    return false;
}
