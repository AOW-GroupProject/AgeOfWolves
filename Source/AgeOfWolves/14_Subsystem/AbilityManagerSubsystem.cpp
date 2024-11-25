#include "AbilityManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogAbilityManager)

//@Defualt Setting
#pragma region Default Setting
UAbilityManagerSubsystem::UAbilityManagerSubsystem()
{}

void UAbilityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    AbilitySet = LoadObject<UBaseAbilitySet>(nullptr, TEXT("/Game/Blueprints/02_AbilitySystem/01_Character/01_AkaOni/AS_AkaOni"));

    if (AbilitySet)
    {
        CacheAbilitySetData();
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
void UAbilityManagerSubsystem::CacheAbilitySetData()
{
    if (!AbilitySet)
        return;

    CachedAbilities = AbilitySet->GetGameplayAbilities();
}

UTexture2D* UAbilityManagerSubsystem::GetAbilityIconTexture2D(FGameplayTag AbilityTag)
{
    if (!AbilityTag.IsValid())
        return nullptr;

    for (const auto& AbilityInfo : CachedAbilities)
    {
        if (!AbilityInfo.AbilityTag.IsValid())
            continue;

        if (AbilityInfo.AbilityTag == AbilityTag)
        {
            if (AbilityInfo.AbilityIconImage)
            {
                return AbilityInfo.AbilityIconImage;
            }


        }
    }

    return nullptr;
}
#pragma endregion