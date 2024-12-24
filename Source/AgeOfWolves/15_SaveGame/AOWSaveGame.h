// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "04_Component/InventoryComponent.h"

#include "AOWSaveGame.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSaveGame, Log, All)

USTRUCT(BlueprintType)
struct FInventoryItemSaveInfo
{
	GENERATED_BODY()

public:
	FInventoryItemSaveInfo() {}
	FInventoryItemSaveInfo(const FGuid UniqueItemId, FString& ItemClasssPath, int32 ItemCount, int EnhancementCount)
	{
		this->UniqueItemId = UniqueItemId;
		this->ItemClassPath = ItemClasssPath;
		this->ItemCount = ItemCount;
		this->EnhancementCount = EnhancementCount;
	}
	//연산자 오버로딩
	const bool operator=(const FInventoryItemSaveInfo& Other)
	{
		return UniqueItemId == Other.UniqueItemId;
	}
public:
	//@FGuid
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Item")
		FGuid UniqueItemId;
	//@Item Class 경로
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Item")
		FString ItemClassPath = "";
	//@Item 갯수
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Item")
		int32 ItemCount = -1;
	//@강화 횟수
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Enhancement")
		int32 EnhancementCount = -1;

};


/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UAOWSaveGame : public USaveGame
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	UAOWSaveGame();
#pragma endregion

#pragma region Inventory
public:
	TArray<FInventoryItemSaveInfo> InventoryItemSaveInfos;
#pragma endregion
};
