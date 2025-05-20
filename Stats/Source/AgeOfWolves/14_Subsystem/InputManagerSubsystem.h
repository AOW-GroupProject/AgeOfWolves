// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"

#include "InputManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInputManager, Log, All)

class UInputConfig;

/**
 * InputManagerSubsystem
 * 
 * 사용자의 Input Config 목록을 관리하는 Subsystem
 */
UCLASS()
class AGEOFWOLVES_API UInputManagerSubsystem : public UEnhancedInputLocalPlayerSubsystem
{
	GENERATED_BODY()

#pragma region Default Setting
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
private:
    void LoadInputConfigs();
#pragma endregion

#pragma region Input Configs
private:
    UPROPERTY()
        TArray<UInputConfig*> InputConfigs;
public:
    const TArray<UInputConfig*>& GetInputConfigs() const { return InputConfigs; }
#pragma endregion

};
