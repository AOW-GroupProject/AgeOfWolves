// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/BaseMonster.h"

#include "04_Component/BaseAbilitySystemComponent.h"

// Sets default values
ABaseMonster::ABaseMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMonsterData> dataAsset(TEXT("/Script/AgeOfWolves.MonsterData'/Game/Blueprints/09_Monster/MonsterData.MonsterData'"));
	if (dataAsset.Object)
	{
		MonsterDataFile = dataAsset.Object;
	}
	if (MonsterDataFile->MonsterData.Find(MonsterName))
	{
		SingleMonsterData = *(MonsterDataFile->MonsterData.Find(MonsterName));
		InitializeMonsterData();
	}
	else
	{
		//일단 주석처리
		//Destroy();
	}
	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));
	

}

// Called when the game starts or when spawned
void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseMonster::ControllRotation()
{
	
}



// Called every frame
void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* ABaseMonster::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseMonster::InitializeMonsterData()
{
}


