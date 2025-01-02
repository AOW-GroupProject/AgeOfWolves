#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "AIDataSetInfos.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAIDataSetInfos, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBehaviorTree;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EAIType
* 
*	AI 유형을 열거형으로 정의합니다.
*/
UENUM(BlueprintType)
enum class EAIType : uint8
{
	Wolf = 0		UMETA(DisplayName = "Wolf"),
	MAX,
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 *	@FAbilitySet_GameplayAbility
 *
 *	Gameplay Ability 와 함께 관리할 정보들을 정의한 구조체
 */
USTRUCT(BlueprintType)
struct FAIDataSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, category = "AI 정보 | AI 유형")
		EAIType AIType;

	UPROPERTY(EditDefaultsOnly, category = "AI 정보 | Behavior Tree")
		TObjectPtr<UBehaviorTree> BehaviorTree;

};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAIDataSet
 * 
 *	AI 데이터 정보를 담은 Data Asset
 */
UCLASS()
class AGEOFWOLVES_API UAIDataSetInfos : public UPrimaryDataAsset
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAIDataSetInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	UPROPERTY(EditDefaultsOnly, Category = "AI 정보 목록")
		TArray<FAIDataSet> AIDataSets;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
