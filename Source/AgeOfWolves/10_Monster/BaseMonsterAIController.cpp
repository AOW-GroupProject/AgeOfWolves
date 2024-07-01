// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception")));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f; 
	SightConfig->LoseSightRadius = 1050.f; 
	SightConfig->SetMaxAge(3.0f);
	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseMonsterAIController::OnTargetDetected);


}

void ABaseMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//RunBehaviorTree(BTree);
	//Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);
}

void ABaseMonsterAIController::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	//태그?
	GetBlackboardComponent()->SetValueAsObject("Player", InActor);
}
