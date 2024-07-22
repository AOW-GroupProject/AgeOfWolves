// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTagRelationshipMapping.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogATRM)

// UE_LOGFMT(LogATRM, Warning, "");

void UAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
    // OutTagsToBlock->Reset();
    // OutTagsToCancel->Reset();

    for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
    {
        const FAbilityTagRelationship& Tags = AbilityTagRelationships[i];
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            if (OutTagsToBlock)
            {
                OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
            }
            if (OutTagsToCancel)
            {
                OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
            }
        }
    }
}

void UAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
   // OutActivationRequired->Reset();
   // OutActivationBlocked->Reset();

   for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
   {
       const FAbilityTagRelationship& Tags = AbilityTagRelationships[i];
       if (AbilityTags.HasTag(Tags.AbilityTag))
       {
           if (OutActivationRequired)
           {
               OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
           }
           if (OutActivationBlocked)
           {
               OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
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
