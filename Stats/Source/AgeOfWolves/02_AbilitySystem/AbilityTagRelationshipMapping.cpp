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

    UE_LOGFMT(LogATRM, Warning, "Cache Map 초기화를 시작합니다.");

    // AbilityTagRelationships 배열의 데이터로 맵 구성
    for (const FAbilityTagRelationship& Relationship : AbilityTagRelationships)
    {
        if (Relationship.AbilityTag.IsValid())
        {
            if (!Relationship.AbilityTagsToBlock.IsEmpty())
            {
                AbilityTagsToBlockMap.Add(Relationship.AbilityTag, Relationship.AbilityTagsToBlock);
                UE_LOGFMT(LogATRM, Warning, "{0}에 대한 Block Tags가 맵에 추가되었습니다: {1}",
                    Relationship.AbilityTag.GetTagName(), Relationship.AbilityTagsToBlock.ToString());
            }

            if (!Relationship.AbilityTagsToCancel.IsEmpty())
            {
                AbilityTagsToCancelMap.Add(Relationship.AbilityTag, Relationship.AbilityTagsToCancel);
                UE_LOGFMT(LogATRM, Warning, "{0}에 대한 Cancel Tags가 맵에 추가되었습니다: {1}",
                    Relationship.AbilityTag.GetTagName(), Relationship.AbilityTagsToCancel.ToString());
            }

            if (!Relationship.ActivationRequiredTags.IsEmpty())
            {
                ActivationRequiredTagsMap.Add(Relationship.AbilityTag, Relationship.ActivationRequiredTags);
                UE_LOGFMT(LogATRM, Warning, "{0}에 대한 Required Tags가 맵에 추가되었습니다: {1}",
                    Relationship.AbilityTag.GetTagName(), Relationship.ActivationRequiredTags.ToString());
            }

            if (!Relationship.ActivationBlockedTags.IsEmpty())
            {
                ActivationBlockedTagsMap.Add(Relationship.AbilityTag, Relationship.ActivationBlockedTags);
                UE_LOGFMT(LogATRM, Warning, "{0}에 대한 Blocked Tags가 맵에 추가되었습니다: {1}",
                    Relationship.AbilityTag.GetTagName(), Relationship.ActivationBlockedTags.ToString());
            }
        }
        else
        {
            UE_LOGFMT(LogATRM, Error, "유효하지 않은 Ability Tag가 있습니다.");
        }
    }

    UE_LOGFMT(LogATRM, Warning, "Cache Map 초기화가 완료되었습니다.");
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
                UE_LOGFMT(LogATRM, Warning, "{0}에 대한 Block Tags가 추가되었습니다: {1}",
                    Tag.GetTagName(), BlockTags->ToString());
            }
            else
            {
                UE_LOGFMT(LogATRM, Error, "{0}에 대한 Block Tags를 찾을 수 없습니다.",
                    Tag.GetTagName());
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
                UE_LOGFMT(LogATRM, Warning, "{0}에 대한 Cancel Tags가 추가되었습니다: {1}",
                    Tag.GetTagName(), CancelTags->ToString());
            }
            else
            {
                UE_LOGFMT(LogATRM, Error, "{0}에 대한 Cancel Tags를 찾을 수 없습니다.",
                    Tag.GetTagName());
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