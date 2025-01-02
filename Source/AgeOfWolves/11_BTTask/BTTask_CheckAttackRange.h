#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_CheckAttackRange.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_CheckAttackRange, Log, All)

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
 *	@UBTTask_CheckAttackRange
 * 
 *	AI의 Attack Range를 체크합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_CheckAttackRange : public UBTTask_BlackboardBase
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBTTask_CheckAttackRange(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion

//@Property/Info...etc
protected:
	UPROPERTY(EditAnywhere, Category = "Attack Range")
		float Min = 0.f;
	UPROPERTY(EditAnywhere, Category = "Attack Ragne")
		float Max = 0.f;
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
#pragma endregion
};
