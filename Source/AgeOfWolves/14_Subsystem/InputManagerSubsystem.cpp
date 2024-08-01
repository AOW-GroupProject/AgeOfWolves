// Fill out your copyright notice in the Description page of Project Settings.

#include "InputManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "06_Input/InputConfig.h"

DEFINE_LOG_CATEGORY(LogInputManager)

#pragma region Default Setting
void UInputManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LoadInputConfigs();
}

void UInputManagerSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UInputManagerSubsystem::LoadInputConfigs()
{
    //@InputConfigs
    InputConfigs.Empty();
    //@Player Character Input Config
    {
        UInputConfig* InputConfig = LoadObject<UInputConfig>(nullptr, TEXT("/Script/AgeOfWolves.InputConfig'/Game/Blueprints/06_Input/Aka_Oni/OnGround/IC_AkaOni_OnGround.IC_AkaOni_OnGround'"));
        if (InputConfig)
        {
            InputConfigs.Add(InputConfig);
            UE_LOGFMT(LogInputManager, Log, "플레이어 캐릭터 InputConfig를 성공적으로 로드했습니다.");
        }
        else
        {
            UE_LOGFMT(LogInputManager, Error, "플레이어 캐릭터 InputConfig 로드에 실패했습니다.");
        }
    }
    //@Player UI Input Config
    {
        UInputConfig* InputConfig = LoadObject<UInputConfig>(nullptr, TEXT("/Script/AgeOfWolves.InputConfig'/Game/Blueprints/06_Input/SystemUI/IC_SystemUI.IC_SystemUI'"));
        if (InputConfig)
        {
            InputConfigs.Add(InputConfig);
            UE_LOGFMT(LogInputManager, Log, "플레이어 UI InputConfig를 성공적으로 로드했습니다.");
        }
        else
        {
            UE_LOGFMT(LogInputManager, Error, "플레이어 UI InputConfig 로드에 실패했습니다.");
        }
    }

    UE_LOGFMT(LogInputManager, Log, "총 {0}개의 InputConfig를 로드했습니다.", InputConfigs.Num());

}
#pragma endregion
