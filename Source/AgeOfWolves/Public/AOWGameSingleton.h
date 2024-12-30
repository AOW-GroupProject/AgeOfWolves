// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AOWGameSingleton.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UAOWGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UAOWGameSingleton();
	static UAOWGameSingleton& Get();


};
