// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterInterface_GAS.generated.h"

class UBaseMonsterASC;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMonsterInterface_GAS : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGEOFWOLVES_API IMonsterInterface_GAS
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UBaseMonsterASC* GetMonsterASC() = 0;



};
