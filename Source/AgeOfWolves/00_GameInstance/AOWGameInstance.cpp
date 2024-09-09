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
	{
		//@Save File이 있는 경우
		if (DoesSaveGameExist())
		{
			if (SaveGameInstance = Cast<UAOWSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFileName, SaveIndex)))
			{
				UE_LOGFMT(LogGameInstance, Log, "Save File : {0}이 정상적으로 Load 되었습니다.", SaveFileName);
			}
			else
			{
				UE_LOGFMT(LogGameInstance, Log, "Save File : {0}이 Load에 실패했습니다.", SaveFileName);
			}
		}
	}
	//@Loading
	{
		//@LoadMap 이전 작업
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAOWGameInstance::PreLoadMapEvent);
		//@LoadMap 이후 작업
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAOWGameInstance::PostLoadMapEvent);
	}
}

void UAOWGameInstance::Shutdown()
{
	Super::Shutdown();

}

bool UAOWGameInstance::DoesSaveGameExist()
{
	return UGameplayStatics::DoesSaveGameExist(SaveFileName, SaveIndex);
}

void UAOWGameInstance::PreLoadMapEvent(const FString& LevelName)
{
	//@TODO: Show Loading Screen Widget
}

void UAOWGameInstance::PostLoadMapEvent(UWorld* LoadedWolrd)
{
	//@TODO: Collapse Loading Screen Widget	
}
