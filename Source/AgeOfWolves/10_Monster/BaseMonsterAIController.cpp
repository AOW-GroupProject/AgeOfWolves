// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "01_Character/CharacterBase.h"
#include "01_Character/BaseMonster.h"
#include "01_Character/BaseMonster_Spline.h"

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
	GetPerceptionComponent()->OnTargetPerceptionForgotten.AddDynamic(this, &ABaseMonsterAIController::OnTargetForgotten);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	
}

void ABaseMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//RunBehaviorTree(BTree);
	Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);

	ABaseMonster* Chr = Cast<ABaseMonster>(InPawn);
	//이거 BeginPlay에서 GetOwner, GetCharacter, GetPawn등으로 캐스팅 해봤는데 안됨. 빙의가 BeginPlay보다 늦게 되는 것 같음.

	if (Chr)
	{
		Agent = Chr;
		TeamId = FGenericTeamId(Agent->ID);

	}

	
}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	
}

//인터페이스 오버라이딩 함수
ETeamAttitude::Type ABaseMonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if (OtherPawn == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	//하나의 액터를 확인할 때 플레이어인지 몬스터인지 모르므로 둘 다 캐스팅해보는 과정. 하나는 플레이어(PlayerTI), 하나는 몬스터(BotTI). 
	auto PlayerTI = Cast<IGenericTeamAgentInterface>(&Other);
	class IGenericTeamAgentInterface* BotTI = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());

	//만약 TeamAgent인터페이스가 구현되어있지 않다면 중립
	if (BotTI == nullptr && PlayerTI == nullptr)
	{
		return ETeamAttitude::Neutral;
	}


	FGenericTeamId OtherActorTeamId = NULL;
	if (BotTI != nullptr) //만약 이 액터가 몬스터라면 몬스터의 TeamID 저장
	{
		OtherActorTeamId = BotTI->GetGenericTeamId();
	}
	else if (PlayerTI != nullptr) //만약 이 액터가 플레이어라면 플레이어의 TeamID 저장
	{
		OtherActorTeamId = PlayerTI->GetGenericTeamId();
	}

	//현재 본인 TeamID 호출해서 비교.
	FGenericTeamId ThisId = GetGenericTeamId();
	if (OtherActorTeamId == 8) //이 숫자는 8일 필요가 없이 아무 숫자나 가능. 8은 무조건 중립 객체라는 뜻.  
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Neutral"));
		return ETeamAttitude::Neutral;
	}
	else if (OtherActorTeamId == ThisId) //같다면 우호적 반환
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Friendly"));
		return ETeamAttitude::Friendly;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hostile"));
		return ETeamAttitude::Hostile;
	}
	

	return ETeamAttitude::Type();
}

void ABaseMonsterAIController::OnTargetDetected(AActor* InActor, FAIStimulus Stimulus)
{
	//태그?
	//인식 시 캐릭터의 소환물 등 다른 객체도 인식할 수 있도록 수정
	if (Cast<ACharacterBase>(InActor))
	{
		if (IsPossibleDetecting)
		{
			GetBlackboardComponent()->SetValueAsObject("Player", InActor);
			GetBlackboardComponent()->SetValueAsBool("DetectingPlayer", true);
			Cast<ABaseMonster>(GetPawn())->ChangeState(EMonsterState::DetectingPlayer);
		}
	}
}

void ABaseMonsterAIController::OnTargetForgotten(AActor* InActor)
{
	if (Cast<ACharacterBase>(InActor))
	{
		GetBlackboardComponent()->SetValueAsObject("Player", nullptr);
		GetBlackboardComponent()->SetValueAsBool("DetectingPlayer", false);
	}
}

void ABaseMonsterAIController::SetIsPossibleDetecting(bool InBool)
{
	IsPossibleDetecting = InBool;
	if (!InBool)
	{
		Cast<ABaseMonster>(GetPawn())->ChangeState(EMonsterState::Patrol);
		GetBlackboardComponent()->SetValueAsBool("DetectingPlayer", false);
	}
	
}
