// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTagRelationshipMapping.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogATRM)

// UE_LOGFMT(LogATRM, Warning, "");

void UAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
    OutTagsToBlock->Reset();
    OutTagsToCancel->Reset();

    for (const auto& AbilityTagRelationship : AbilityTagRelationships)
    {
        if (AbilityTags.HasTagExact(AbilityTagRelationship.AbilityTag))
        {
            if (OutTagsToBlock)
            {
                OutTagsToBlock->AppendTags(AbilityTagRelationship.AbilityTagsToBlock);
            }
            if (OutTagsToCancel)
            {
                OutTagsToCancel->AppendTags(AbilityTagRelationship.AbilityTagsToCancel);
            }
        }
    }
}

void UAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{

    OutActivationRequired->Reset();
    OutActivationBlocked->Reset();

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
