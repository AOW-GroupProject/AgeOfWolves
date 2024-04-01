// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogCharacter)

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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
	return AbilitySystemComponent.Get();
}
