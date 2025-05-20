#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "AOWGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameInstance, Log, All);


//@전방 선언
#pragma region Forward Declaration
class UAOWSaveGame;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAOWGameInstance
 * 
 *	AOW의 Game Instance를 정의합니다.
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
protected:
	UAOWSaveGame* CreateNewSaveGameInstance();

public:
	bool DoesSaveGameExist();

private:
	//@Save 파일 인스턴스
	TObjectPtr<UAOWSaveGame> SaveGameInstance = nullptr;
	const FString SaveFileName = "AOW_SaveGame";
	const int32 SaveIndex = 0;
#pragma endregion

#pragma region Loading
public:
	void PreLoadMapEvent(const FString& LevelName);
	void PostLoadMapEvent(UWorld* LoadedWolrd);
#pragma endregion


public:
	UAOWSaveGame* GetSaveGameInstance();
};
