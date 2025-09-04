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
class UAT_CompensateDamage;
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
	Line		UMETA(DisplayName = "Line Trace"),
	Sphere		UMETA(DisplayName = "Sphere Trace"),
	Box			UMETA(DisplayName = "Box Trace"),
	Cylinder	UMETA(DisplayName = "Cylinder Trace")
};

/*
*	@EHitStopSettingMode
*
*	히트 스탑 적용 모드를 설정합니다.
*/
UENUM(BlueprintType)
enum class EFXApplyRange : uint8
{
	//@어빌리티 전체에 하나의 공통 히트스탑 설정 적용
	Global UMETA(DisplayName = "전역 설정"),

	//@몽타주별로 개별 히트스탑 설정 적용
	PerMontage UMETA(DisplayName = "몽타주별 설정")
};

//@충돌 이펙트 유형
UENUM(BlueprintType)
enum class ECollisionEffectType : uint8
{
	Slash       UMETA(DisplayName = "Slash"),
	Impact      UMETA(DisplayName = "Impact"),
	Spark       UMETA(DisplayName = "Spark"),
	Decal       UMETA(DisplayName = "Decal"),
	Distortion  UMETA(DisplayName = "Distortion"),
	Blood       UMETA(DisplayName = "Blood"),
	MAX         UMETA(DisplayName = "MAX")
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
struct FTimeFXSetting
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

/*
*   @FCollisionFXSetting
*
*   충돌 이펙트 정보를 담은 구조체
*/
USTRUCT(BlueprintType)
struct FCollisionFXSetting
{
	GENERATED_BODY()

public:
	// 생성자
	FCollisionFXSetting()
		: bEnabled(false)
		, EffectType(ECollisionEffectType::Impact)
		, EffectCueTag(FGameplayTag::EmptyTag)
		, bUseImpactLocation(true)  // 기본적으로 충돌 위치 사용
		, bUseSocket(false)
		, SocketName(NAME_None)
		, CustomLocation(FVector::ZeroVector)
		, EffectRotation(FRotator::ZeroRotator)
		, EffectScale(FVector(1.0f, 1.0f, 1.0f))
	{
	}

	// 커스텀 생성자
	FCollisionFXSetting(
		ECollisionEffectType InEffectType,
		FGameplayTag InEffectCueTag,
		bool InUseImpactLocation = true,
		bool InUseSocket = false,
		FName InSocketName = NAME_None
	)
		: bEnabled(false)
		, EffectType(InEffectType)
		, EffectCueTag(InEffectCueTag)
		, bUseImpactLocation(InUseImpactLocation)
		, bUseSocket(InUseSocket)
		, SocketName(InSocketName)
		, CustomLocation(FVector::ZeroVector)
		, EffectRotation(FRotator::ZeroRotator)
		, EffectScale(FVector(1.0f, 1.0f, 1.0f))
	{
	}

public:
	// 이펙트 활성화 여부 (최상위 옵션)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	bool bEnabled = false;

	// 이펙트 유형
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX", meta = (EditCondition = "bEnabled"))
	ECollisionEffectType EffectType = ECollisionEffectType::Impact;

	// 이펙트 GameplayCue 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX", meta = (EditCondition = "bEnabled"))
	FGameplayTag EffectCueTag;

	// 위치 설정 섹션

	// 충돌 위치 직접 사용 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX|위치", meta = (EditCondition = "bEnabled"))
	bool bUseImpactLocation = true;

	// 소켓 사용 여부 (충돌 위치를 사용하지 않을 때만 표시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX|위치", meta = (EditCondition = "bEnabled && !bUseImpactLocation"))
	bool bUseSocket = false;

	// 소켓 이름 (소켓을 사용할 때만 표시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX|위치", meta = (EditCondition = "bEnabled && !bUseImpactLocation && bUseSocket"))
	FName SocketName;

	// 커스텀 위치 (소켓도 충돌 위치도 사용하지 않을 때만 표시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX|위치", meta = (EditCondition = "bEnabled && !bUseImpactLocation && !bUseSocket"))
	FVector CustomLocation = FVector::ZeroVector;

	// 회전과 스케일 섹션

	// 이펙트 회전값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX|변형", meta = (EditCondition = "bEnabled"))
	FRotator EffectRotation = FRotator::ZeroRotator;

	// 이펙트 스케일
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX|변형", meta = (EditCondition = "bEnabled"))
	FVector EffectScale = FVector(1.0f, 1.0f, 1.0f);

	// Getter 함수
	FORCEINLINE ECollisionEffectType GetEffectType() const { return EffectType; }
	FORCEINLINE FGameplayTag GetEffectCueTag() const { return EffectCueTag; }
	FORCEINLINE bool UseImpactLocation() const { return bUseImpactLocation; }
	FORCEINLINE bool UseSocket() const { return !bUseImpactLocation && bUseSocket; }
	FORCEINLINE FName GetSocketName() const { return SocketName; }
	FORCEINLINE FVector GetCustomLocation() const { return CustomLocation; }
	FORCEINLINE FRotator GetEffectRotation() const { return EffectRotation; }
	FORCEINLINE FVector GetEffectScale() const { return EffectScale; }
	FORCEINLINE bool IsEnabled() const { return bEnabled; }

	// 유효성 검사 함수
	FORCEINLINE bool IsValid() const
	{
		if (!bEnabled || !EffectCueTag.IsValid())
			return false;

		// 충돌 위치 사용이면 항상 유효
		if (bUseImpactLocation)
			return true;

		// 소켓 사용이면 소켓 이름 유효성 확인
		if (bUseSocket)
			return !SocketName.IsNone();

		// 커스텀 위치는 항상 유효
		return true;
	}
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
	friend class UANS_CompensateStrongAttack;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAttackGameplayAbility(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	//@어빌리티 종료 시 파훼 태스크 정리
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
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
	//@시간 배율 조작 실행 함수
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작")
	void ExecuteTimeFX(const FHitResult& HitResult, AActor* SourceActor);

	//@HitStop 적용 함수
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작")
	void ApplyHitStop(AActor* Target);

	//@현재 몽타주 인덱스를 기반으로 HitStop 적용
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작")
	void ApplyHitStopForCurrentMontage(AActor* Target, int32 MontageIndex = -1);

protected:
	//@충돌 FX 실행 함수
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| FX")
	void ExecuteCollisionFX(const FHitResult& HitResult, AActor* SourceActor);

	//@GameplayCue 이펙트 실행
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| FX")
	void ExecuteGameplayCueAtLocation(const FGameplayTag& CueTag, const FTransform& SpawnTransform, AActor* SourceActor);

	//@현재 몽타주의 FX 설정으로 이펙트 실행
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| FX")
	void ExecuteCollisionFXForCurrentMontage(const FHitResult& HitResult, AActor* SourceActor, int32 MontageIndex = -1);

protected:
	//@파훼 태스크 생성 및 활성화
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 파훼")
	void ActivateCompensationTask(bool bOnlyTriggerOnce = false);

	//@파훼 태스크 정리
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 파훼")
	void DeactivateCompensationTask();

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
	//@Cylinder Trace용 설정
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌",
		meta = (EditCondition = "TraceType == EWeaponTraceType::Cylinder"))
	float CylinderRadius = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌",
		meta = (EditCondition = "TraceType == EWeaponTraceType::Cylinder"))
	float CylinderHalfHeight = 50.0f;

protected:
	//@HitStop 활성화 여부 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작")
	bool bEnableHitStop = false;

	//@HitStop 설정 모드
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작",
		meta = (EditCondition = "bEnableHitStop"))
	EFXApplyRange HitStopSettingMode = EFXApplyRange::Global;
	//@HitStop 모드 설정 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작",
		meta = (EditCondition = "HitStopSettingMode == EFXApplyRange::Global && bEnableHitStop"))
	ETimeDilationMode HitStopMode = ETimeDilationMode::HitStop;

	//@HitStop 강도 설정 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작",
		meta = (EditCondition = "HitStopSettingMode == EFXApplyRange::Global && bEnableHitStop"))
	ETimeDilationIntensity HitStopIntensity = ETimeDilationIntensity::Low;

	//@글로벌 HitStop 적용 여부 (false면 캐릭터에만 적용) (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작",
		meta = (EditCondition = "HitStopSettingMode == EFXApplyRange::Global && bEnableHitStop"))
	bool bGlobalHitStop = false;

	//@몽타주별 HitStop 설정 (몽타주별 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| 시간 배율 조작",
		meta = (EditCondition = "HitStopSettingMode == EFXApplyRange::PerMontage"))
	TArray<FTimeFXSetting> MontageTimeFXSettings;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| FX")
		bool bEnableCollisionFX = false;

	//@FX 설정 모드
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| FX",
		meta = (EditCondition = "bEnableCollisionFX"))
	EFXApplyRange FXSettingMode = EFXApplyRange::Global;

	//@전역 FX 설정 (전역 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| FX",
		meta = (EditCondition = "bEnableCollisionFX && FXSettingMode == EFXApplyRange::Global"))
	FCollisionFXSetting GlobalFXSetting;

	//@몽타주별 FX 설정 (몽타주별 설정 모드)
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌| 연출| FX",
		meta = (EditCondition = "bEnableCollisionFX && FXSettingMode == EFXApplyRange::PerMontage"))
	TArray<FCollisionFXSetting> MontageFXSettings;

protected:
	//@파훼 태스크 참조
	UPROPERTY(Transient)
	UAT_CompensateDamage* CurrentCompensationTask;
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

public:
	//@소켓 위치와 회전값 가져오기
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌| 연출| FX")
	bool GetSocketTransform(FName SocketName, FTransform& OutTransform) const;

protected:
	//@파훼 태스크 가져오기 (ANS에서 사용)
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 파훼")
	UAT_CompensateDamage* GetCompensationTask() const;

	//@파훼 태스크 활성화 상태 확인
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 파훼")
	bool IsCompensationTaskActive() const;
#pragma endregion

};