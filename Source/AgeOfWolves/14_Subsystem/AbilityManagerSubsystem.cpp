#include "AbilityManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/BaseAbilitySet.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogAbilityManager)

//@Defualt Setting
#pragma region Default Setting
UAbilityManagerSubsystem::UAbilityManagerSubsystem()
{
    AbilitySet = nullptr;
}

void UAbilityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@Tool Data Table
    AbilitySet = LoadObject<UBaseAbilitySet>(nullptr, TEXT("/Game/Blueprints/02_AbilitySystem/01_Character/01_AkaOni/AS_AkaOni"));
    if (AbilitySet)
    {
        UE_LOGFMT(LogAbilityManager, Log, "Ability Set 로드 성공");
    }
    else
    {
        UE_LOGFMT(LogAbilityManager, Error, "Ability Set 로드 실패");
    }
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
UTexture2D* UAbilityManagerSubsystem::GetAbilityIconTexture2D(const FGameplayTag& AbilityTag) const
{
    if (!AbilitySet)
    {
        UE_LOGFMT(LogAbilityManager, Warning, "AbilitySet이 유효하지 않습니다.");
        return nullptr;
    }

    if (!AbilityTag.IsValid())
    {
        UE_LOGFMT(LogAbilityManager, Warning, "유효하지 않은 AbilityTag가 전달되었습니다.");
        return nullptr;
    }

    // AbilitySet에서 GameplayAbilities 배열 가져오기
    TArray<FBaseAbilitySet_GameplayAbility> GameplayAbilities = AbilitySet->GetGameplayAbilities();

    // 각 Ability를 순회하면서 매칭되는 태그 찾기
    for (const FBaseAbilitySet_GameplayAbility& AbilityInfo : GameplayAbilities)
    {
        if (!AbilityInfo.Ability)
        {
            continue;
        }

        // Ability CDO 가져오기
        UBaseGameplayAbility* AbilityCDO = AbilityInfo.Ability->GetDefaultObject<UBaseGameplayAbility>();
        if (!AbilityCDO)
        {
            continue;
        }

        if (AbilityInfo.bActive && AbilityInfo.bInputBinded && AbilityInfo.InputTag == AbilityTag)
        {
            UE_LOGFMT(LogAbilityManager, Verbose, "AbilityTag {0}에 해당하는 아이콘 이미지를 찾았습니다.", AbilityTag.ToString());
            return AbilityInfo.AbilityIconImage;
        }
    }

    UE_LOGFMT(LogAbilityManager, Warning, "AbilityTag {0}에 해당하는 아이콘 이미지를 찾을 수 없습니다.", AbilityTag.ToString());
    return nullptr;
}
#pragma endregion