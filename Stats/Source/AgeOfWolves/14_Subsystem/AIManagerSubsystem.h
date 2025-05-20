#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "10_AI/AIDataSetInfos.h"

#include "AIManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAIManager, Log, All)

//@전방 선언
#pragma region Forward Declaration
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
 *	@UAIManagerSubsystem
 * 
 *	AI 관련 정보를 관리하는 서브 시스템
 */
UCLASS()
class AGEOFWOLVES_API UAIManagerSubsystem : public UGameInstanceSubsystem
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAIManagerSubsystem();

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩

protected:
	//@초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
	
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
	//@AI 정보 참조
	UPROPERTY()
		TObjectPtr<UDataAsset> AIDataSetInfos;

private:
	const UAIDataSetInfos* GetAIDataSetInfos() const;

public:
	//@단일 AI 데이터셋 반환
	UFUNCTION(BlueprintCallable, Category = "AI|DataSet")
		bool GetAIDataSet(EAIType AIType, FAIDataSet& OutDataSet) const;

	//@여러 AI 데이터셋 반환
	UFUNCTION(BlueprintCallable, Category = "AI|DataSet")
		TArray<FAIDataSet> GetAIDataSets(const TArray<EAIType>& AITypes) const;
#pragma endregion
	
};
