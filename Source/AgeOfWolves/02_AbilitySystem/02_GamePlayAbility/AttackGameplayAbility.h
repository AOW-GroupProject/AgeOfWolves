// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

#include "AttackGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAttackGA, Log, All);

//@전방 선언
#pragma region Forward Declaration
class APlayerCharacter;
class UCombatComponent;
class UAnimMontage;
#pragma endregion

//@열거형
#pragma region Enums
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

UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	EAT_MAX  UMETA(Hidden),
};
#pragma endregion

//@구조체
#pragma region Structs

#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UAttackGameplayAbility : public UBaseGameplayAbility
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAttackGameplayAbility(const FObjectInitializer& ObjectInitializer);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@BP 에서 Damage Info 멤버 설정 필수
	UFUNCTION(BlueprintCallable, Category = "Ability | Damage")
		void SendDamageEvent(const FHitResult& HitResult);

protected:
	//@공격 유형
	UPROPERTY(EditDefaultsOnly, meta = (Category = "AttackAbilityInfo"))
		EAkoniAttackType AkoniAttackType;

	//@첫 공격 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AttackAbilityInfo")
		bool bFirstAttack = false;

	//@Damage 유형
	UPROPERTY(EditDefaultsOnly, Category = "AttackAbilityInfo")
		TMap<FGameplayTag, FScalableFloat> DamageTypes;

	//@Anim Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "AttackAbilityInfo"))
		TArray<UAnimMontage*> AttackMontages;

	//@Anim Montage 재생 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "AttackAbilityInfo"))
		float MontagePlayRate = 1.0f;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Getter")
		APlayerCharacter* GetPlayerCharacterFromActorInfo() const;
#pragma endregion
	
};
