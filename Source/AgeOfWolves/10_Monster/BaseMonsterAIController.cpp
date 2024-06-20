// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "01_Character/CharacterBase.h"

ABaseMonsterAIController::ABaseMonsterAIController()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception")));

	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 500.f; 
	SightConfig->LoseSightRadius = 550.f; 
	SightConfig->SetMaxAge(3.0f);
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseMonsterAIController::OnTargetDetected);
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

}

void ABaseMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//RunBehaviorTree(BTree);
	Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);
}

void ABaseMonsterAIController::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	//태그?
	//인식 시 캐릭터의 소환물 등 다른 객체도 인식할 수 있도록 수정
	if (Cast<ACharacterBase>(InActor))
	{
		GetBlackboardComponent()->SetValueAsObject("Player", InActor);
	}
}
