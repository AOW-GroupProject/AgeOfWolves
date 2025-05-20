#include "BaseAbilitySetInfos.h"
#include "Logging/StructuredLog.h"

#include "BaseAbilitySet.h"

DEFINE_LOG_CATEGORY(LogBaseAbilitySetInfos)

UBaseAbilitySetInfos::UBaseAbilitySetInfos(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    BaseAbilitySets.Empty();
}

void UBaseAbilitySetInfos::GetAbilitySets(TArray<UBaseAbilitySet*>& OutArray)
{
    if (BaseAbilitySets.IsEmpty())
    {
        UE_LOGFMT(LogBaseAbilitySetInfos, Warning, "BaseAbilitySets이 비어있음");
        return;
    }

    // OutArray를 비워서 새로운 데이터를 담을 준비
    OutArray.Empty();

    for (const auto& Set : BaseAbilitySets)
    {
        if (!Set)
        {
            continue;
        }

        OutArray.Add(Set.Get());
    }

    if (OutArray.IsEmpty())
    {
        UE_LOGFMT(LogBaseAbilitySetInfos, Warning, "유효한 AbilitySet이 없음");
    }
}

UBaseAbilitySet* UBaseAbilitySetInfos::GetBaseAbilitySet(const FGameplayTag& Tag) const
{
    if (!Tag.IsValid())
    {
        UE_LOGFMT(LogBaseAbilitySetInfos, Warning, "전달된 CharacterTag가 유효하지 않음");
        return nullptr;
    }

    // CharacterTag와 정확히 일치하는 AbilitySet 찾기
    for (const auto& AbilitySet : BaseAbilitySets)
    {
        if (!AbilitySet)
            continue;

        if (AbilitySet->GetCharacterTag() == Tag)
        {
            return AbilitySet;
        }
    }

    UE_LOGFMT(LogBaseAbilitySetInfos, Warning, "Character Tag {0}에 해당하는 AbilitySet을 찾을 수 없음", Tag.ToString());
    return nullptr;
}