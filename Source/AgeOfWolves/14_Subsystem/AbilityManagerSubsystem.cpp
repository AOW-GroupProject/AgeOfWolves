// AbilityManagerSubsystem.cpp
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

    if (!AbilitySet)
    {
        UE_LOG(LogAbilityManager, Warning, TEXT("Failed to load AbilitySet: AS_AkaOni"));
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
UTexture2D* UAbilityManagerSubsystem::GetAbilityIconTexture2D(FGameplayTag AbilityTag)
{
    if (!AbilityTag.IsValid())
    {
        UE_LOG(LogAbilityManager, Warning, TEXT("GetAbilityIconTexture2D - Invalid AbilityTag"));
        return nullptr;
    }

    if (!AbilitySet)
    {
        UE_LOG(LogAbilityManager, Warning, TEXT("GetAbilityIconTexture2D - AbilitySet is null"));
        return nullptr;
    }

    const TArray<FBaseAbilitySet_GameplayAbility> GAInfos = AbilitySet->GetGameplayAbilities();

    for (const auto& AbilityInfo : GAInfos)
    {
        if (!AbilityInfo.AbilityTag.IsValid())
            continue;

        if (AbilityInfo.AbilityTag == AbilityTag)
        {
            if (!AbilityInfo.AbilityIconImage)
            {
                UE_LOG(LogAbilityManager, Warning, TEXT("GetAbilityIconTexture2D - Icon not found for AbilityTag: %s"), *AbilityTag.ToString());
                return nullptr;
            }

            return AbilityInfo.AbilityIconImage;
        }
    }

    UE_LOG(LogAbilityManager, Warning, TEXT("GetAbilityIconTexture2D - No matching ability for AbilityTag: %s"), *AbilityTag.ToString());
    return nullptr;
}
#pragma endregion