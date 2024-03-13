// Copyright Epic Games, Inc. All Rights Reserved.

#include "AgeOfWolvesGameMode.h"
#include "AgeOfWolvesCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
