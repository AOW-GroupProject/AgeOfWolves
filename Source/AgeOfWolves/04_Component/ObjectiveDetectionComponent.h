#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ObjectiveDetectionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UObjectiveDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObjectiveDetectionComponent();

protected:
	virtual void BeginPlay() override;
		
};
