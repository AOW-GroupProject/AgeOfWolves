#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

#include "AttackGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAttackGA, Log, All);

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class UCombatComponent;
class UAnimMontage;
#pragma endregion

//@열거형
#pragma region Enums
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
	friend class UANS_AttackTrace;
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
	//@Anim Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "AttackAbilityInfo"))
		TArray<UAnimMontage*> AttackMontages;

	//@Anim Montage 재생 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "AttackAbilityInfo"))
		float MontagePlayRate = 1.0f;

protected:
	// 트레이스 이벤트 함수들
	UFUNCTION(BlueprintCallable, Category = "Ability|Trace")
		virtual void StartWeaponTrace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Trace")
		virtual void ProcessWeaponTrace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Trace")
		virtual void EndWeaponTrace();

protected:
	// 트레이스에 사용될 소켓 이름들
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Trace")
		FName WeaponTraceStartSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Ability|Trace")
		FName WeaponTraceEndSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Ability|Trace")
		float TraceRadius = 20.0f;

	// 트레이스 활성화 여부
	bool bIsTracing = false;

	UPROPERTY()
		TArray<AActor*> ActorsToIgnore;
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
		ACharacterBase* GetCharacterFromActorInfo() const;
#pragma endregion
	
};
