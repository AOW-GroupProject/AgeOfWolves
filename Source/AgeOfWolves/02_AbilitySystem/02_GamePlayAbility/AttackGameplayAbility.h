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

protected:

	UPROPERTY(EditDefaultsOnly, meta = (Category = "AttackAbilityInfo"))
	EAkoniAttackType AkoniAttackType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AttackAbilityInfo")
	bool bFirstAttack = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AttackAbilityInfo")
	int ComboIndex = 1;

	UPROPERTY(EditDefaultsOnly, Category = "AttackAbilityInfo")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "AttackAbilityInfo"))
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "AttackAbilityInfo"))
	float MontagePlayRate = 1.0f;

	UFUNCTION(BlueprintCallable)
	void CauseDamageToTarget(AActor* TargetActor);


private:

public: 
	UFUNCTION(BlueprintCallable, Category = "Ability | Getter")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability | Getter")
	UCombatComponent* GetCombatComponentFromPlayerCharacter() const;
	
};
