// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectPad_Test.generated.h"

class UAbilitySystemComponent;
class UBaseAttributeSet;
class UGameplayEffect;
class USphereComponent;

UCLASS()
class AGEOFWOLVES_API AGameplayEffectPad_Test : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayEffectPad_Test();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Abilities")
	UAbilitySystemComponent* ASC;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOW|Abilities")
	UBaseAttributeSet* AS;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOW|Abilities")
	TSubclassOf<UGameplayEffect> GameplayEffectToApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOW|Abilities")
	TSubclassOf<UGameplayEffect> GameplayEfectDamaToHealth;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	USphereComponent* Sphere;


	UFUNCTION()
	void OnPadOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
