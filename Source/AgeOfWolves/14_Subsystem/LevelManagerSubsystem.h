#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "16_Level/LevelDataInfos.h"

#include "LevelManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelManager, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ULevelDataInfos;
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
 *	@ULevelManagerSubsystem
 *
 *	Level 정보 로딩 및 관련 처리를 수행하는 Subsystem
 */
UCLASS()
class AGEOFWOLVES_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
//@친추 클래스
#pragma region Friend Class
	friend class UAOWGameInstance;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	ULevelManagerSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩
	void ExternalBindToUIManager();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY()
	TObjectPtr<ULevelDataInfos> LevelDataInfos;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Loading UI Fade-In 완료 이벤트 구독, 본격적으로, 레벨 전환이 발생하는 시점(비동기 처리)
	UFUNCTION()
	void OnLoadingUIFadeInComplete();
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	//@특정 레벨 타입에 해당하는 모든 레벨 정보를 제공하는 함수
	TArray<FLevelData> GetLevelsByType(const ELevelType& LevelType) const;

	//@특정 레벨 ID로 레벨 정보를 제공하는 함수
	bool GetLevelByID(const FGuid& LevelID, FLevelData& OutLevelData) const;

	//@특정 레벨 이름으로 레벨 정보를 제공하는 함수
	bool GetLevelByName(const FText& LevelName, FLevelData& OutLevelData) const;

	//@활성화된 모든 레벨 정보를 제공하는 함수
	TArray<FLevelData> GetEnabledLevels() const;

	//@특정 레벨의 활성화 상태를 확인하는 함수
	bool IsLevelEnabled(const FGuid& LevelID) const;

	//@특정 타입의 첫 번째 레벨을 가져오는 함수
	bool GetFirstLevelOfType(const ELevelType& LevelType, FLevelData& OutLevelData) const;

	//@전체 레벨 개수를 반환하는 함수
	int32 GetTotalLevelCount() const;

	//@특정 타입의 레벨 개수를 반환하는 함수
	int32 GetLevelCountByType(const ELevelType& LevelType) const;

	//@레벨 데이터가 유효한지 확인하는 함수
	bool IsLevelDataValid() const;
#pragma endregion

};