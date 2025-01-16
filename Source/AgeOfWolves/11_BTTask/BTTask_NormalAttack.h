#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_NormalAttack.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBTTask_NormalAttack, Log, All)

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
 *	@UBTTask_NormalAttack
 * 
 *	임시 테스크, 약 공격 수행
 */
UCLASS()
class AGEOFWOLVES_API UBTTask_NormalAttack : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTTask_NormalAttack(const FObjectInitializer& ObjectInitializer);

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    // 실행할 어빌리티의 태그
    UPROPERTY(EditAnywhere, Category = "Ability")
        FGameplayTag AbilityTag;
	
};
