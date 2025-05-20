#include "AgeOfWolvesGameMode.h"
#include "AgeOfWolvesCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "01_Character/CharacterBase.h"

AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<ACharacterBase> PlayerPawnBPClass(TEXT("Blueprint'/Game/Blueprints/01_Character/02_Killer/BP_Killer'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}
