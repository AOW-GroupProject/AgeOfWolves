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
/*
*	@무기 트레이스 유형
* 
*	무기의 충돌 유형을 정의합니다.
*/
UENUM(BlueprintType)
enum class EWeaponTraceType : uint8
{
	Line    UMETA(DisplayName = "Line Trace"),
	Sphere  UMETA(DisplayName = "Sphere Trace"),
	Box     UMETA(DisplayName = "Box Trace")
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
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 데미지")
		void SendDamageEvent(const FHitResult& HitResult);

protected:
	//@트레이스 시작
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌")
		virtual void StartWeaponTrace();

	//@트레이스 처리
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌")
		virtual void ProcessWeaponTrace();

	//@트레이스 종료
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌")
		virtual void EndWeaponTrace();

	//@Line Trace
	void PerformLineTrace(const FVector& Start, const FVector& End,
		FCollisionQueryParams& QueryParams, TArray<FHitResult>& OutHitResults);

	//@통합된 Sweep 함수를 위한 템플릿 함수
	template<typename TShape>
	void PerformSweepTrace(const FVector& Start, const FVector& End,
		const TShape& Shape, FCollisionQueryParams& QueryParams,
		TArray<FHitResult>& OutHitResults)
	{
		{
			GetWorld()->SweepMultiByChannel(
				OutHitResults,
				Start,
				End,
				FQuat::Identity,
				ECC_Visibility,
				Shape,
				QueryParams
			);
		}
	}

protected:
	//@트레이스 활성화 여부
	bool bIsTracing = false;

	//@무시 대상
	UPROPERTY()
		TArray<AActor*> ActorsToIgnore;

	//@Trace 타입 설정
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		EWeaponTraceType TraceType = EWeaponTraceType::Line;

protected:
	//@트레이스에 사용될 소켓 이름들
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		FName WeaponTraceStartSocket;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		FName WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		FName WeaponTraceEndSocket;

protected:
	//@Sphere Trace용 반경
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌",
		meta = (EditCondition = "TraceType == EWeaponTraceType::Sphere"))
		float SphereTraceRadius = 20.0f;

protected:
	//@Box Trace용 크기
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌",
		meta = (EditCondition = "TraceType == EWeaponTraceType::Box"))
		FVector BoxTraceHalfSize = FVector(20.0f, 20.0f, 20.0f);
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@오버라이드
	virtual void OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag) override;
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Getter")
		ACharacterBase* GetCharacterFromActorInfo() const;
#pragma endregion
	
};
