// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "Item.generated.h"

UCLASS()
class AGEOFWOLVES_API AItem : public AActor
{
	GENERATED_BODY()
	

#pragma region Default Setting
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// 	//~UObject interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	//~End Of UObject Interface
#pragma endregion

#pragma region Property
	UPROPERTY(EditAnywhere, Category = "Item | Item Iformation")
		FGameplayTag ItemTag;
#pragma endregion
};
