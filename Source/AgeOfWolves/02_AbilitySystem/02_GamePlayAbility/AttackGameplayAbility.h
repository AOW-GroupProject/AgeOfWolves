#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "14_Subsystem/TimeManipulationSubsystem.h"

#include "AttackGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAttackGA, Log, All);

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
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

/*
*	@EHitStopSettingMode
* 
*	히트 스탑 적용 모드를 설정합니다.
*/
UENUM(BlueprintType)
enum class EHitStopSettingMode : uint8
{
	//@어빌리티 전체에 하나의 공통 히트스탑 설정 적용
	Global UMETA(DisplayName = "전역 설정"),

	//@몽타주별로 개별 히트스탑 설정 적용
	PerMontage UMETA(DisplayName = "몽타주별 설정")
};
#pragma endregion

//@구조체
#pragma region Structs
/*
*	@FMontageHitStopSettings
*
*	몽타주별 HitStop 설정을 정의합니다.
*/
USTRUCT(BlueprintType)
struct FMontageHitStopSettings
{
	GENERATED_BODY()

public:
	//@HitStop 활성화 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitStop")
		bool bEnableHitStop = false;

	//@HitStop 모드 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitStop", meta = (EditCondition = "bEnableHitStop"))
		ETimeDilationMode HitStopMode = ETimeDilationMode::HitStop;

	//@HitStop 강도 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitStop", meta = (EditCondition = "bEnableHitStop"))
		ETimeDilationIntensity HitStopIntensity = ETimeDilationIntensity::Low;

	//@글로벌 HitStop 적용 여부 (false면 캐릭터에만 적용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitStop", meta = (EditCondition = "bEnableHitStop"))
		bool bGlobalHitStop = false;
};
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

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@인덱스 지정 버전 몽타주 재생
	UFUNCTION(BlueprintCallable, Category = "Ability|Montage")
		UAbilityTask_PlayMontageAndWait* PlayMontageWithCallbackAndIndex(
			UAnimMontage* MontageToPlay,
			int32 MontageIndex,
			float Rate = 1.0f,
			FName StartSection = NAME_None,
			bool bStopWhenAbilityEnds = true);

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
	//@HitStop 적용 함수
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출")
		void ApplyHitStop(AActor* Target);

	//@현재 몽타주 인덱스를 기반으로 HitStop 적용
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출")
		void ApplyHitStopForCurrentMontage(AActor* Target, int32 MontageIndex = -1);

protected:
	//@충돌 위치에 GameplayCue 이펙트 실행
	void ExecuteImpactGameplayCue(const FHitResult& HitResult, AActor* SourceActor);

protected:
	//@현재 실행 중인 몽타주 인덱스
	UPROPERTY(BlueprintReadWrite, Category = "어빌리티 | 애니메이션")
		int32 CurrentMontageIndex = 0;

private:
	// 트레이스 상태 관리를 위한 동기화 객체
	FCriticalSection TraceStateLock;

protected:
	// 트레이스 관련 상태 변수들
	UPROPERTY(Transient)
		uint8 bIsTracing : 1;

	// 약참조로 변경하여 메모리 안전성 향상
	UPROPERTY(Transient)
		TArray<TWeakObjectPtr<AActor>> ActorsToIgnore;

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

protected:
	//@HitStop 설정 모드
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출")
		EHitStopSettingMode HitStopSettingMode = EHitStopSettingMode::Global;

	//@HitStop 활성화 여부 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출",
		meta = (EditCondition = "HitStopSettingMode == EHitStopSettingMode::Global"))
		bool bEnableHitStop = false;

	//@HitStop 모드 설정 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출",
		meta = (EditCondition = "HitStopSettingMode == EHitStopSettingMode::Global && bEnableHitStop"))
		ETimeDilationMode HitStopMode = ETimeDilationMode::HitStop;

	//@HitStop 강도 설정 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출",
		meta = (EditCondition = "HitStopSettingMode == EHitStopSettingMode::Global && bEnableHitStop"))
		ETimeDilationIntensity HitStopIntensity = ETimeDilationIntensity::Low;

	//@글로벌 HitStop 적용 여부 (false면 캐릭터에만 적용) (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출",
		meta = (EditCondition = "HitStopSettingMode == EHitStopSettingMode::Global && bEnableHitStop"))
		bool bGlobalHitStop = false;

	//@몽타주별 HitStop 설정 (몽타주별 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출",
		meta = (EditCondition = "HitStopSettingMode == EHitStopSettingMode::PerMontage"))
		TArray<FMontageHitStopSettings> MontageHitStopSettings;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출")
		FGameplayTag ImpactEffectCueTag;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@오버라이드
	virtual void OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag) override;
	//@오버라이드
	virtual void OnChainActionFinished_Implementation(FGameplayTag ChainActionEventTag) override;
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Getter")
		ACharacterBase* GetCharacterFromActorInfo() const;
#pragma endregion
	
};
