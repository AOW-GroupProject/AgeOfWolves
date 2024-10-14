// Fill out your copyright notice in the Description page of Project Settings.

#include "01_Character/BaseMonster.h"

#include "Kismet/GameplayStatics.h"


#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "10_Monster/BaseMonsterAIController.h"
#include "10_Monster/MonsterDataSubsystem.h"
//#include "MotionWarpingComponent.h"




// Sets default values
ABaseMonster::ABaseMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAbilitiesInitialized = false;
	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));
	

	//싱글이 아닌 서버를 할 때 해줘야 할 것
	//AbilitySystemComponent->SetIsReplicated(true);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributSet"));

	//MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComp_Monster"));
	
	
	

}

void ABaseMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//여기서 서브시스템 호출하면 BP_Wolf 켜기만 하면 에디터 꺼져서 BeginPlay로 옮김

	if (UGameplayStatics::GetGameInstance(GetWorld()))
	{

		UMonsterDataSubsystem* MonsterDataSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UMonsterDataSubsystem>();

		if (MonsterDataSubSystem)
		{
			MonsterDataSubSystem->CustomFunction(MonsterName, SingleMonsterData, AbilitySystemComponent, SetGrantedHandles);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("CustomFunction"));
		}
	}
	
}

// Called when the game starts or when spawned
void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	
	
	
}

// Called every frame
void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

void ABaseMonster::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeGameplayAbilitySystem();
	

}

EMonsterState ABaseMonster::GetCurrentState()
{
	return CurrentState;
}

void ABaseMonster::ChangeState(EMonsterState inValue)
{
	
	WhenEndState();
	CurrentState = inValue;
}

void ABaseMonster::WhenEndState()
{
	switch (CurrentState)
	{
	case EMonsterState::Patrol:

		break;
	case EMonsterState::Strafe:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("ClearFocus"));
		Cast<AAIController>(GetController())->ClearFocus(EAIFocusPriority::Gameplay);
		break;
	case EMonsterState::Attacking:
		CurrentState = EMonsterState::DetectingPlayer;
		GetMesh()->GetAnimInstance()->Montage_Stop(0.5);
		break;
	
	case EMonsterState::DetectingPlayer:

		break;
	case EMonsterState::Stunned:

		break;
	
	}
}


void ABaseMonster::ControllRotation()
{
	
}

void ABaseMonster::OnWarpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void ABaseMonster::SetWarpTarget(FName name, FVector vector)
{
}




UAbilitySystemComponent* ABaseMonster::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ABaseMonster::InitializeGameplayAbilitySystem()
{
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

}

FSingleMonsterData ABaseMonster::GetSingleMonsterData()
{
	return SingleMonsterData;
}

void ABaseMonster::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	if (OnAnyMonsterAttributeValueChanged.IsBound())
	{
		OnAnyMonsterAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);
	}
}






