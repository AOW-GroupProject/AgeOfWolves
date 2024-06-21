// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_GameplayAbility/BaseGameplayAbility.h"
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

// Todo : Akaoni와 다른 케릭터들의 UClass 구분?
/**
 * 
 */
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
	TArray<UAnimMontage*> Montages;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Animation"))
	float PlayRate = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Ability | Getter")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;



	UFUNCTION(BlueprintCallable, Category = "Ability | Getter")
	UCombatComponent* GetCombatComponentFromPlayerCharacter() const;
	
};
