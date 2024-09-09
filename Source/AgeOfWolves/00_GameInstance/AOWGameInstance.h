// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "AOWGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameInstance, Log, All);

class UAOWSaveGame;

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UAOWGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	UAOWGameInstance();
public:
	virtual void Init() override;
	virtual void Shutdown() override;
#pragma endregion

#pragma region Save File
private:
	//@Save 파일 인스턴스
	TObjectPtr<UAOWSaveGame> SaveGameInstance = nullptr;
	const FString SaveFileName = "AOW_SaveGame";
	const int32 SaveIndex = 0;
public:
	bool DoesSaveGameExist();
	FORCEINLINE UAOWSaveGame* GetSaveGameInstance() const { return SaveGameInstance; }
#pragma endregion

#pragma region Loading
public:
	void PreLoadMapEvent(const FString& LevelName);
	void PostLoadMapEvent(UWorld* LoadedWolrd);
#pragma endregion
};
