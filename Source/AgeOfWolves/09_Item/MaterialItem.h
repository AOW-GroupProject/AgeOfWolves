// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Item/Item.h"

#include "MaterialItem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMaterialItem, Log, All);

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API AMaterialItem : public AItem
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	AMaterialItem(const FObjectInitializer& ObjectInitializer);
#pragma endregion

#pragma region Activation
protected:
	virtual bool ActivateItem_Implementation() override;
#pragma endregion

#pragma region Property
public:
	virtual const FMaterialItemInformation* GetItemInformation();
#pragma endregion
};
