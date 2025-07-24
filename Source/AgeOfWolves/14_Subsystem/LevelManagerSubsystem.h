#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "16_Level/LevelDataInfos.h"
#include "GameplayTagContainer.h"

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
DECLARE_DELEGATE_OneParam(FLevelTransitionCompleted, const FGameplayTag&);
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
	//@외부 바인딩...
	void ExternalBindinToGameState();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
	void PerformLevelStreamingOperations(const FLevelData& TargetLevelData);

protected:
	UPROPERTY()
	TObjectPtr<ULevelDataInfos> LevelDataInfos;

private:
	//@레벨 전환 동시성 제어
	FCriticalSection LevelTransitionLock;
	bool bIsLevelTransitionInProgress = false;

	//@현재 및 대기 중인 레벨 태그
	UPROPERTY()
	FGameplayTag CurrentLevelTag;
	UPROPERTY()
	FGameplayTag PendingLevelTag;
#pragma endregion

//@Delegates
#pragma region Delegates
protected:
	//@델리게이트 핸들을 저장해서 나중에 해제할 수 있도록 함
	FDelegateHandle WorldBeginPlayHandle;

public:
	//@레벨 전환 완료 이벤트
	FLevelTransitionCompleted LevelTransitionCompleted;
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
	//@World BeginPlay 시점에 호출될 함수
	void OnWorldBeginPlay();

private:
	UFUNCTION()
	void OnRequestStartLevelTransition(const FGameplayTag& NextLevelTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	//@특정 레벨 타입에 해당하는 모든 레벨 정보를 제공하는 함수
	TArray<FLevelData> GetLevelsByType(const ELevelType& LevelType) const;

	//@특정 레벨 태그로 레벨 정보를 제공하는 함수 (FGuid에서 FGameplayTag로 변경)
	bool GetLevelByTag(const FGameplayTag& LevelTag, FLevelData& OutLevelData) const;

	//@특정 레벨 이름으로 레벨 정보를 제공하는 함수
	bool GetLevelByName(const FText& LevelName, FLevelData& OutLevelData) const;

	//@활성화된 모든 레벨 정보를 제공하는 함수
	TArray<FLevelData> GetEnabledLevels() const;

	//@특정 레벨의 활성화 상태를 확인하는 함수 (FGuid에서 FGameplayTag로 변경)
	bool IsLevelEnabled(const FGameplayTag& LevelTag) const;

	//@특정 타입의 첫 번째 레벨을 가져오는 함수
	bool GetFirstLevelOfType(const ELevelType& LevelType, FLevelData& OutLevelData) const;

	//@전체 레벨 개수를 반환하는 함수
	int32 GetTotalLevelCount() const;

	//@특정 타입의 레벨 개수를 반환하는 함수
	int32 GetLevelCountByType(const ELevelType& LevelType) const;

	//@레벨 데이터가 유효한지 확인하는 함수
	bool IsLevelDataValid() const;

	//@PlayerStart 태그 관련 함수들
	UFUNCTION(BlueprintCallable, Category = "Player Start")
	bool GetDefaultPlayerStartTag(const FGameplayTag& LevelTag, FGameplayTag& OutPlayerStartTag) const;

	UFUNCTION(BlueprintCallable, Category = "Player Start")
	bool GetAvailablePlayerStartTags(const FGameplayTag& LevelTag, TArray<FGameplayTag>& OutPlayerStartTags) const;

	FGameplayTag GetDefaultPlayerStartTagForLevel(const FGameplayTag& LevelTag) const;

	TArray<FGameplayTag> GetAvailablePlayerStartTagsForLevel(const FGameplayTag& LevelTag) const;


public:
	FGameplayTag GetCurrentLevelTag() const { return CurrentLevelTag; }
#pragma endregion

};