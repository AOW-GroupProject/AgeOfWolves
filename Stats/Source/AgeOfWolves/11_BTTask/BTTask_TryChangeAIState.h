#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "10_Ai/BaseAIController.h"

#include "BTTask_TryChangeAIState.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_TryChangeAIState, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
UENUM(BlueprintType)
enum class ERunMode : uint8
{
	E_ThisAgent			UMETA(DisplayName = "This Agent"),
	E_AllAgent			UMETA(DisplayName = "All Agent"),
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UBTTask_TryChangeAIState
 * 
 *	AI State 변경을 시도합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_TryChangeAIState : public UBTTask_BlackboardBase
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBTTask_TryChangeAIState(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
		EAIState DesireState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
		ERunMode RunMode;
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
