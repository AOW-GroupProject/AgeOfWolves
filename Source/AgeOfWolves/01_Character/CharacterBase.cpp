// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "MotionWarpingComponent.h"

DEFINE_LOG_CATEGORY(LogCharacter)

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//@Motion Warping Component
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}


void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr;
}

void ACharacterBase::SetAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
	check(ASC)

	AbilitySystemComponent = ASC;
}
