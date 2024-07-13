// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "09_Item/Item.h"

#include "EquipmentItem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEquipmentItem, Log, All);

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API AEquipmentItem : public AItem
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	AEquipmentItem(const FObjectInitializer& ObjectInitializer);
#pragma endregion

#pragma region Activation
protected:
	virtual bool ActivateItem_Implementation() override;
#pragma endregion

#pragma region Property
public:
	virtual const FEquipmentItemInformation* GetItemInformation();
#pragma endregion
};
