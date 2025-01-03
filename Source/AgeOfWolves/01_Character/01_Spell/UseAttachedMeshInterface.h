// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UseAttachedMeshInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUseAttachedMeshInterface : public UInterface
{
	GENERATED_BODY()
};

class USkeletalMeshComponent;

/**
 * 
 */
class AGEOFWOLVES_API IUseAttachedMeshInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual USkeletalMeshComponent* GetWeaponSkeletalMeshComponent() = 0;
	virtual void AttachToWeapon(AActor* Other, FName BoneSocket) = 0;

};
