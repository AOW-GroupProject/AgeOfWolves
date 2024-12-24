// Fill out your copyright notice in the Description page of Project Settings.


#include "InputConfig.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogInputConfig)
// 로크 매크로
//UE_LOGFMT(LogInputConfig, Log, "<FString 형식>")


UInputConfig::UInputConfig(const FObjectInitializer& ObjectInitializer)
{}

const UInputAction* UInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const auto& InputActionStruct : NativeInputActions)
    {
        if (InputActionStruct.InputAction && (InputActionStruct.InputTag == InputTag))
        {
            return InputActionStruct.InputAction;
        }
    }

    if (bLogNotFound)
    {
        UE_LOGFMT(LogInputConfig, Error, "Can't find NativeInputAction for InputTag {0} on InputConfig {1}.", *InputTag.ToString(), *GetNameSafe(this));
    }

    return nullptr;
}

const UInputAction* UInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const auto& InputActionStruct : AbilityInputActions)
    {
        if (InputActionStruct.InputAction && (InputActionStruct.InputTag == InputTag))
        {
            return InputActionStruct.InputAction;
        }
    }

    if (bLogNotFound)
    {
        UE_LOGFMT(LogInputConfig, Error, "Can't find AbilityInputAction for InputTag {0} on InputConfig {1}.", *InputTag.ToString(), *GetNameSafe(this));
    }

    return nullptr;
}

const UInputAction* UInputConfig::FindUIInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const auto& InputActionStruct : UIInputActions)
    {
        if (InputActionStruct.InputAction && (InputActionStruct.InputTag == InputTag))
        {
            return InputActionStruct.InputAction;
        }
    }

    if (bLogNotFound)
    {
        UE_LOGFMT(LogInputConfig, Error, "Can't find UIInputAction for InputTag {0} on InputConfig {1}.", *InputTag.ToString(), *GetNameSafe(this));
    }

    return nullptr;
}
