// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "AttackGameplayAbility.generated.h"

class APlayerCharacter;
class UCombatComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EAkoniAttackType : uint8
{
	// Normal Attack
	AAT_NormalAttack  UMETA(DisplayName = "Normal Attack"),

	// Baldo Attack
	AAT_Baldo UMETA(DisplayName = "Baldo Attack"),

	// Magic
	ATT_Magic  UMETA(DisplayName = "Magic Attack"),

	ATT_MAX  UMETA(Hidden),
};

// Todo : Enemy도 사용할 수 있게 Class 분류?

UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	EAT_MAX  UMETA(Hidden),
};


UCLASS()
class AGEOFWOLVES_API UAttackGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UAttackGameplayAbility();

	UPROPERTY(EditDefaultsOnly, meta = (Category = "AttackInfo | Type"))
	EAkoniAttackType AkoniAttackType;

	UPROPERTY(EditDefaultsOnly, Category = "AttackInfo | Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Animation"))
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Animation"))
	float MontagePlayRate = 1.0f;




	UFUNCTION(BlueprintCallable, Category = "Ability | Getter")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability | Getter")
	UCombatComponent* GetCombatComponentFromPlayerCharacter() const;
	
};
