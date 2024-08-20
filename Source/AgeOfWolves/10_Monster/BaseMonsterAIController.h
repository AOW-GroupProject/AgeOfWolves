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
	TObjectPtr<UBehaviorTree> BTree;


#pragma endregion

#pragma region SightConfig
private:
	// @설명: Perception이 가능한지 저장하는 변수
	UPROPERTY(BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
	bool IsPossibleDetecting = true;

	UPROPERTY(BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
	AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception", meta = (AllowPrivateAccess = true))
	class UAISenseConfig_Sight* SightConfig;

	// @설명: 타겟이 감지되었을 때 실행해줄 함수
	UFUNCTION(BlueprintCallable, Category = "Perception")
	void OnTargetDetected(AActor* InActor, FAIStimulus Stimulus);

	// @설명: 타겟이 감지 범위를 나갔을 때 실행해줄 함수
	UFUNCTION(BlueprintCallable, Category = "Perception")
	void OnTargetForgotten(AActor* InActor);
public:
	// @설명: BaseMonster_Spline에서 캐릭터를 쫓다가 너무 멀어졌을 시 제자리로 돌아가는 동안 몇초간은 캐릭터를 쫓지 않아야 하므로 잠깐동안 false로 해준다.
	void SetIsPossibleDetecting(bool InBool);


#pragma endregion
	
};
