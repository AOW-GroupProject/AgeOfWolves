// Fill out your copyright notice in the Description page of Project Settings.


#include "01_Character/BaseMonster.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"


// Sets default values
ABaseMonster::ABaseMonster()
{
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAbilitiesInitialized = false;

	static ConstructorHelpers::FObjectFinder<UMonsterData> dataAsset(TEXT("/Script/AgeOfWolves.MonsterData'/Game/Blueprints/09_Monster/MonsterData.MonsterData'"));
	if (dataAsset.Object)
	{
		MonsterDataFile = dataAsset.Object;
	}
	if (MonsterDataFile->MonsterData.Find(MonsterName))
	{
		SingleMonsterData = *(MonsterDataFile->MonsterData.Find(MonsterName));
		MonsterDataFile = nullptr;
	}
	else
	{
		//일단 주석처리
		//Destroy();
	}
	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("MonsterAbilitySystemComponent"));
	//AbilitySystemComponent->

	//싱글이 아닌 서버를 할 때 해줘야 할 것
	//AbilitySystemComponent->SetIsReplicated(true);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	//AttributeSet = CreateDefaultSubobject<UBaseMonsterAttributeSet>(TEXT("AttributSet"));
	
	
	

}

void ABaseMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
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

void ABaseMonster::ControllRotation()
{
	
}




UAbilitySystemComponent* ABaseMonster::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseMonster::InitializeGameplayAbilitySystem()
{
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// #2. GAS 초기화
	if (Cast<AController>(GetController()))
	{
		if (IsValid(AbilitySet))
		{
			// 캐릭터의 기본 AttributeSet을 ASC에 최초 등록합니다.
			{
				AbilitySet->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

				// 각 Attribute 항목 수치 변화 이벤트에 콜백함수를 등록합니다.
				for (auto& AS : AbilitySystemComponent->GetSpawnedAttributes())
				{
					if (IsValid(AS))
					{
						AttributeSet = AS;
						TArray<FGameplayAttribute> Attributes = AttributeSet->GetAllAttributes();
						for (const FGameplayAttribute& Attribute : Attributes)
						{
							//FDelegateHandle DelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ABaseMonster::OnAttributeValueChanged);
						}
						break;
					}
				}
			}
			// 캐릭터의 기본 GA에 대한 Ability Tag Relationpship을 등록합니다.
			{
				if (TagRelationship)
				{
					AbilitySystemComponent->SetAbilityTagRelationshipMapping(TagRelationship);
				}
			}
			// 캐릭터의 기본 Gameplay Effect를 ASC에 최초 등록/적용합니다.
			{
				AbilitySet->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
			}
			// 캐릭터의 기본 Gameplay Ability를 ASC에 최초 등록/적용합니다.
			{
				AbilitySet->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
			}

		}
	}
}

FSingleMonsterData ABaseMonster::GetSingleMonsterData()
{
	return SingleMonsterData;
}

void ABaseMonster::OnAttributeValueChanged(FOnAttributeChangeData& Data)
{
	if (OnAnyMonsterAttributeValueChanged.IsBound())
	{
		OnAnyMonsterAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);
	}
}





