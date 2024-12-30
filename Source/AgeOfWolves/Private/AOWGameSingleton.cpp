// Fill out your copyright notice in the Description page of Project Settings.


#include "AOWGameSingleton.h"


UAOWGameSingleton::UAOWGameSingleton()
{
	
}

UAOWGameSingleton& UAOWGameSingleton::Get()
{
	UAOWGameSingleton* Singleton = CastChecked<UAOWGameSingleton>(GEngine->GameSingleton);

	if (Singleton)
	{
		return *Singleton;
	}

	//UE_LOG(LogABGameSingieton, Error, TEXT("Invaild Game Singleton"));
	return *NewObject<UAOWGameSingleton>();
}
