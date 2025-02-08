// Fill out your copyright notice in the Description page of Project Settings.


#include "AOWGameInstance.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"

#include "15_SaveGame/AOWSaveGame.h"

DEFINE_LOG_CATEGORY(LogGameInstance)
 //UE_LOGFMT(LogGameInstance, Log, "");


UAOWGameInstance::UAOWGameInstance()
{}

void UAOWGameInstance::Init()
{
    Super::Init();

    //@Save Game Instance
    if (DoesSaveGameExist())
    {
        SaveGameInstance = Cast<UAOWSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFileName, SaveIndex));

        if (!SaveGameInstance)
        {
            SaveGameInstance = CreateNewSaveGameInstance();
        }
    }
    else
    {
        SaveGameInstance = CreateNewSaveGameInstance();
    }

    //@Loading
    {
        FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAOWGameInstance::PreLoadMapEvent);
        FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAOWGameInstance::PostLoadMapEvent);
    }
}

void UAOWGameInstance::Shutdown()
{
    Super::Shutdown();

    //@Save Game 정리
    if (SaveGameInstance)
    {
        SaveGameInstance = nullptr;
    }
}

UAOWSaveGame* UAOWGameInstance::CreateNewSaveGameInstance()
{
	SaveGameInstance = Cast<UAOWSaveGame>(UGameplayStatics::CreateSaveGameObject(UAOWSaveGame::StaticClass()));

	UE_LOGFMT(LogGameInstance, Log, "새로운 SaveGame 인스턴스 생성");

	return SaveGameInstance;
}

bool UAOWGameInstance::DoesSaveGameExist()
{
	return UGameplayStatics::DoesSaveGameExist(SaveFileName, SaveIndex);
}

UAOWSaveGame* UAOWGameInstance::GetSaveGameInstance()
{   
    if (!SaveGameInstance.Get())
    {
        return CreateNewSaveGameInstance();
    }

    return SaveGameInstance.Get();
}

void UAOWGameInstance::PreLoadMapEvent(const FString& LevelName)
{
	//@TODO: Show Loading Screen Widget
}

void UAOWGameInstance::PostLoadMapEvent(UWorld* LoadedWolrd)
{
	//@TODO: Collapse Loading Screen Widget	
}
