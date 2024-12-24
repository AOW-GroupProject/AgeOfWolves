// AbilityManagerSubsystem.cpp
#include "AbilityManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "02_AbilitySystem/BaseAbilitySetInfos.h"

DEFINE_LOG_CATEGORY(LogAbilityManager)

//@Defualt Setting
#pragma region Default Setting
UAbilityManagerSubsystem::UAbilityManagerSubsystem()
{
    AbilitySetInfos = nullptr;
    MAbilitySets.Empty();
}

void UAbilityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@Load
    const FString AssetPath = TEXT("/Game/Blueprints/02_AbilitySystem/DA_BaseAbilityInformations");
    AbilitySetInfos = LoadObject<UBaseAbilitySetInfos>(nullptr, *AssetPath);

    if (!AbilitySetInfos)
    {
        UE_LOGFMT(LogAbilityManager, Warning, "BaseAbilitySetInfos 로드 실패. 경로: {0}", AssetPath);
        return;
    }

    UE_LOGFMT(LogAbilityManager, Log, "BaseAbilitySetInfos 로드 성공");

    //@MAbilitySets
    if (!AbilitySetInfos)
    {
        UE_LOGFMT(LogAbilityManager, Warning, "캐시 초기화 실패 - AbilitySetInfos가 유효하지 않음");
        return;
    }

    MAbilitySets.Empty();

    //@TArray< UBaseAbilitySet>
    TArray<UBaseAbilitySet*> AllAbilitySets;
    AbilitySetInfos->GetAbilitySets(AllAbilitySets);

    UE_LOGFMT(LogAbilityManager, Log, "로드된 AbilitySet 개수: {0}", AllAbilitySets.Num());

    for (auto* AbilitySet : AllAbilitySets)
    {
        if (!AbilitySet)
            continue;

        const FGameplayTag& Tag = AbilitySet->GetCharacterTag();
        if (!Tag.IsValid())
        {
            UE_LOGFMT(LogAbilityManager, Warning, "유효하지 않은 PawnTag를 가진 AbilitySet 발견: {0}",
                *AbilitySet->GetName());
            continue;
        }

        MAbilitySets.Add(Tag, AbilitySet);
        UE_LOGFMT(LogAbilityManager, Log, "AbilitySet 캐시됨 - Tag: {0}", Tag.ToString());
    }

    UE_LOGFMT(LogAbilityManager, Log, "AbilitySet 캐시 초기화 완료. 총 {0}개의 세트가 캐시됨",
        MAbilitySets.Num());
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UBaseAbilitySet* UAbilityManagerSubsystem::GetAbilitySet(FGameplayTag CharacterTag) const
{
    // 먼저 캐시된 맵에서 찾기
    const TObjectPtr<UBaseAbilitySet>* FoundSet = MAbilitySets.Find(CharacterTag);
    if (FoundSet && FoundSet->Get())
    {
        return FoundSet->Get();
    }

    // 캐시에 없으면 Infos에서 찾기
    return AbilitySetInfos ? AbilitySetInfos->GetBaseAbilitySet(CharacterTag) : nullptr;
}

UTexture2D* UAbilityManagerSubsystem::GetAbilityIconTexture2D(const FGameplayTag& AbilityTag, const FGameplayTag& CharacterTag) const
{
    if (!AbilityTag.IsValid())
    {
        UE_LOGFMT(LogAbilityManager, Warning, "GetAbilityIconTexture2D - 유효하지 않은 AbilityTag");
        return nullptr;
    }

    if (!CharacterTag.IsValid())
    {
        UE_LOGFMT(LogAbilityManager, Warning, "GetAbilityIconTexture2D - 유효하지 않은 CharacterTag");
        return nullptr;
    }

    // 캐시된 맵에서 해당 캐릭터의 AbilitySet 찾기
    const TObjectPtr<UBaseAbilitySet>* FoundSet = MAbilitySets.Find(CharacterTag);
    if (!FoundSet || !FoundSet->Get())
    {
        UE_LOGFMT(LogAbilityManager, Warning, "GetAbilityIconTexture2D - CharacterTag {0}에 해당하는 AbilitySet을 찾을 수 없음", CharacterTag.ToString());
        return nullptr;
    }

    // 해당 AbilitySet의 GA들을 순회하며 매칭되는 AbilityTag 찾기
    const TArray<FBaseAbilitySet_GameplayAbility>& GAInfos = FoundSet->Get()->GetGameplayAbilities();
    for (const auto& AbilityInfo : GAInfos)
    {
        if (!AbilityInfo.AbilityTag.IsValid())
            continue;

        if (AbilityInfo.AbilityTag == AbilityTag)
        {
            if (!AbilityInfo.AbilityIconImage)
            {
                UE_LOGFMT(LogAbilityManager, Warning, "GetAbilityIconTexture2D - AbilityTag {0}의 아이콘 이미지가 null임", AbilityTag.ToString());
                return nullptr;
            }
            return AbilityInfo.AbilityIconImage;
        }
    }

    UE_LOGFMT(LogAbilityManager, Warning, "GetAbilityIconTexture2D - CharacterTag {0}의 AbilitySet에서 AbilityTag {1}를 찾을 수 없음",
        CharacterTag.ToString(), AbilityTag.ToString());
    return nullptr;
}
#pragma endregion