// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Item/Item.h"

#include "ToolItem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogToolItem, Log, All);

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API AToolItem : public AItem
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	AToolItem(const FObjectInitializer& ObjectInitializer);
#pragma endregion

#pragma region Activation
protected:
	virtual bool ActivateItem_Implementation() override;
#pragma endregion

#pragma region Property
public:
	virtual const FToolItemInformation* GetItemInformation();
#pragma endregion


};
