// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

#include "01_Character/CharacterBase.h"
#include "01_Character/BaseMonster.h"
#include "01_Character/BaseMonster_Spline.h"
#include "10_Monster/MonsterData.h"
#include "10_Monster/MonsterDataSubsystem.h"

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
		TeamId = FGenericTeamId();

	}

	
}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	
}

//인터페이스 오버라이딩 함수
ETeamAttitude::Type ABaseMonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GetTeamAttitude"));
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if (OtherPawn == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	//하나의 액터를 확인할 때 플레이어인지 몬스터인지 모르므로 둘 다 캐스팅해보는 과정. 하나는 플레이어(PlayerTI), 하나는 몬스터(BotTI).
	auto BotAI = Cast<ABaseMonster>(&Other);


	EMonsterName TargetName;

	//만약 TeamAgent인터페이스가 구현되어있지 않다면 중립
	if (BotAI == nullptr)
	{
		TargetName = EMonsterName::Player;
	}
	else
	{
		TargetName = BotAI->MonsterName;
	}

	bool IsEnemy = false;
	bool IsFriend = false;
	if (UGameplayStatics::GetGameInstance(GetWorld()))
	{

		UMonsterDataSubsystem* MonsterDataSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UMonsterDataSubsystem>();

		if (MonsterDataSubSystem)
		{
			MonsterDataSubSystem->DecideEnemyOrElse(Agent->MonsterName, TargetName, IsEnemy, IsFriend);
			
		}
	}

	if (IsEnemy)
	{
		return ETeamAttitude::Hostile;
		
	}
	else if (IsFriend)
	{
		return ETeamAttitude::Friendly;
		
	}
	else
	{
		//AddOnScreenDebugMessage는 잘 출력이 안됨
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Neutral"));
		return ETeamAttitude::Neutral;
		
	}
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
