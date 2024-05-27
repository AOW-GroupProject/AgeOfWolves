// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Perception/AIPerceptionTypes.h"
#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API ABaseMonsterAIController : public AAIController
{
	GENERATED_BODY()
public:
	ABaseMonsterAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

#pragma region BehaviorTree

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	class UBehaviorTree* BTree;


#pragma endregion

#pragma region Target
private:
	AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
	class UAISenseConfig_Sight* SightConfig;

	/// <summary>
	/// 타겟이 감지되었을 때 실행해줄 함수
	/// </summary>
	/// <param name="InActor"></param>
	/// <param name="Stimulus"></param>
	UFUNCTION(BlueprintCallable, Category = "Perception")
	void OnTargetDetected(AActor* InActor, FAIStimulus Stimulus);

#pragma endregion
	
};
