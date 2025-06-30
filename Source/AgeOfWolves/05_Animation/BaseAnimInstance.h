#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"

#include "BaseAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimInstance, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class UCharacterMovementComponent;
class UMotionWarpingComponent;
class UAsyncTaskAttributeChanged;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EMovementState
*
*	확장된 이동 상태 관련 열거형 - 상태 기계 기반 애니메이션 시스템
*	각 상태는 특정한 애니메이션 단계를 나타내며, 명확한 전이 조건을 가집니다.
*/
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle = 0        UMETA(DisplayName = "Idle"),           // 정지 상태 - 기본 대기 자세
	Start           UMETA(DisplayName = "Start"),          // 시작 상태 - 이동 시작 애니메이션 (가속 구간)
	Cycle_Walk      UMETA(DisplayName = "Cycle_Walk"),     // 걷기 순환 - 지속적인 걷기 애니메이션
	Cycle_Sprint    UMETA(DisplayName = "Cycle_Sprint"),   // 달리기 순환 - 지속적인 달리기 애니메이션
	Stop            UMETA(DisplayName = "Stop"),           // 정지 상태 - 이동 종료 애니메이션 (감속 구간)
	MAX             UMETA(DisplayName = "MAX"),
};

/*
*	@EMovementDirection
*
*	이동 방향 관련 열거형
*/
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Fwd = 0		UMETA(DisplayName = "Fwd"),
	FL			UMETA(DisplayNmae = "Forward Left"),
	FR			UMETA(DisplayNmae = "Forward Right"),
	Bwd			UMETA(DisplayName = "Bwd"),
	BL			UMETA(DisplayNmae = "Forward Left"),
	BR			UMETA(DisplayNmae = "Forward Right"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	MAX			UMETA(DisplayName = "MAX"),
};

/*
*	@EStopMotionType
*
*	멈춤 상태 열거형
*/
UENUM(BlueprintType)
enum class EStopMotionType : uint8
{
	None = 0		UMETA(DisplayName = "None"),
	WalkStop	UMETA(DisplayName = "Walk Stop"),
	SprintStop	UMETA(DisplayName = "Sprint Stop"),
	MAX			UMETA(DisplayName = "MAX"),
};

/*
*	@ECombatType
*
*	전투 상태 열거형
*/
UENUM(BlueprintType)
enum class ECombatType : uint8
{
	NonCombat = 0		UMETA(DisplayName = "NonCombat"),
	NormalCombat		UMETA(DisplayName = "NormalCombat"),
	BattoujutsuCombat	UMETA(DisplayName = "BattoujutsuCombat"),
	GuardCombat			UMETA(DisplayName = "Guard Combat"),
	ReverseGuardCombat	UMETA(DisplayName = "Reverse Guard Combat"),
	MAX					UMETA(DisplayName = "MAX"),
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UBaseAnimInstance
 *
 *	Anim Instance의 기본 유형을 정의합니다.
 *	상태 기계 기반의 고급 이동 애니메이션 시스템을 제공합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBaseAnimInstance : public UAnimInstance
{
	//@친구 클래스
#pragma region Friend Class
	friend class UAN_UpdateStopMotionType;
	friend class UANS_NotifyPlayingRootMotion;
	friend class ABaseAIController;
#pragma endregion

	GENERATED_BODY()

//@기본 설정
#pragma region Default Setting
public:
	UBaseAnimInstance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩

protected:
	//@초기화
#pragma endregion

//@속성/정보...등
#pragma region Property or Subwidgets or Infos...etc
protected:
	// === 레거시 이동 관련 함수들 (하위 호환성 유지) ===
	UFUNCTION(BlueprintCallable)
	void FindMovementDirectionAngle();

protected:
	// === 상태 기계 핵심 로직 ===

	/*
	 * 상태 기계의 메인 업데이트 함수
	 * 매 프레임마다 현재 상태를 분석하고 가능한 전이를 체크합니다.
	 */
	void UpdateMovementStateMachine();

	/*
	 * 각 상태별 전이 처리 함수들
	 * 현재 상태에서 가능한 모든 전이를 우선순위에 따라 체크합니다.
	 */
	void HandleIdleStateTransitions();
	void HandleStartStateTransitions();
	void HandleCycleStateTransitions();
	void HandleStopStateTransitions();

	/*
	 * Combat State에 따른 우회 조건 업데이트
	 * 전투 상황에서는 Start/Stop 단계를 건너뛸 수 있습니다.
	 */
	void UpdateCombatStateFlags();

	/*
	 * 상태 변경 시 호출되는 콜백 함수
	 * 추가적인 설정 업데이트나 로깅을 처리합니다.
	 */
	void OnMovementStateChanged();

protected:
	// === 상태 전이 조건 함수들 (Animation Blueprint에서 호출 가능) ===

	/*
	 * Idle 상태에서 Start 상태로 전이 가능한지 체크
	 * 일반적인 이동 시작 조건을 확인합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool CanTransitionFromIdleToStart() ;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation State")
	bool bCanTransitionFromIdleToStart = false;

protected:
	/*
	 * Idle 상태에서 바로 Cycle 상태로 전이 가능한지 체크
	 * Combat State가 2일 때만 가능한 특수한 전이입니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool CanTransitionFromIdleToCycle();

	/*
	 * Start 상태에서 Cycle 상태로 전이 가능한지 체크
	 * Animation Blueprint에서 타이밍 조건과 함께 사용됩니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool CanTransitionFromStartToCycle();

	/*
	 * Cycle 상태들 간의 전환 가능성 체크 (Walk <-> Sprint)
	 * 속도 변화에 따른 즉시 전환을 처리합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool CanTransitionBetweenCycles();

	/*
	 * 현재 상태에서 Stop 상태로 전이 가능한지 체크
	 * 모든 활성 상태에서 정지 조건을 확인합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool CanTransitionToStop();

	/*
	 * Stop 상태에서 Idle 상태로 전이 가능한지 체크
	 * Animation Blueprint에서 타이밍 조건과 함께 사용됩니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool CanTransitionFromStopToIdle();

protected:
	// === 본 변형 관련 (레거시) ===
	UFUNCTION(BlueprintNativeEvent)
	void ChangeBoneTransform(float DeltaTime);
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent)
	void ClearBoneTransform(float DeltaTime);
	virtual void ClearBoneTransform_Implementation(float DeltaTime) {}

protected:
	// === 이동 설정 업데이트 ===
	UFUNCTION()
	void UpdateMovementSettings();

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	void UpdateStopMotionType(EStopMotionType Type);

protected:
	// === Root Motion 처리 ===
	void HandleStartRootMotion();
	void HandleEndRootMotion();

protected:
	// === Combat State 속성 변화 이벤트 관찰 ===
	void ListenToCombatStateAttributeChange();

protected:
	// === 상태 기계 관련 변수들 ===

	/*
	 * Combat State 기반 우회 플래그들
	 * 전투 상황에서 일반적인 상태 전이 규칙을 우회할 수 있습니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State Machine", meta = (AllowPrivateAccess = "true"))
	bool bCanSkipStartState;  // Start 상태를 건너뛸 수 있는지

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State Machine", meta = (AllowPrivateAccess = "true"))
	bool bCanSkipStopState;   // Stop 상태를 건너뛸 수 있는지

protected:
	// === 기존 이동 상태 변수들 (확장됨) ===

	//@직전 이동 상태에 따른 스탑 모션
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "이동 | 이동 상태", meta = (AllowPrivateAccess = "true"))
	EStopMotionType StopMotionType;

	//@직전 이동 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "이동 | 이동 상태", meta = (AllowPrivateAccess = "true"))
	EMovementState LastMovementState;

	//@현재 이동 상태 (확장된 enum 사용)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "이동 | 이동 상태", meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;

	//@이동 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "이동 | 이동 방향", meta = (AllowPrivateAccess = "true"))
	EMovementDirection MovementDirection;

	//@직전 이동 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "이동 | 이동 방향", meta = (AllowPrivateAccess = "true"))
	EMovementDirection LastMovementDirection;

protected:
	// === 기본 이동 데이터들 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFalling;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector Velocity;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DirectionAngle;
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bEnableDirectionalMovement;

protected:
	// === 속도 임계값들 ===
	const float WalkingSpeed = 200.f;
	const float SprintingSpeed = 500.f;

protected:
	// === 가속도 임계값 상수들 ===
	// 기존 IsNearlyZero() 대신 사용할 더 관대한 임계값들

	// 일반적인 이동 입력 감지용 (기존보다 관대)
	static constexpr float MovementInputThreshold = 50.0f;

	// 정지 조건 체크용 (더욱 관대 - 방향 전환 시 Stop 방지)
	static constexpr float StopConditionThreshold = 25.0f;

	// Idle 전환용 (가장 엄격 - 완전히 멈췄을 때만)
	static constexpr float IdleTransitionThreshold = 10.0f;

protected:
	// === 본 변형 관련 ===
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bModifyBoneTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BoneTransformLerpSpeed;

protected:
	// === 전투 상태 ===
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECombatType CombatType;

protected:
	// === Root Motion 관련 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsPlayingRootMotionMontageWithFullBodySlot;

	UPROPERTY()
	bool bIsRootMotionCooldown;

	UPROPERTY()
	float RootMotionCooldownTime;

	UPROPERTY(EditAnywhere, Category = "Movement|RootMotion", meta = (AllowPrivateAccess = "true"))
	float RootMotionCooldownDuration;

	float CurrentRootMotionCooldownTime;

	//@TODO: 임시
protected:
	UPROPERTY()
	UStaticMesh* OriginalFullWeaponMesh;

private:
	// === 내부 헬퍼 함수들 ===

public:
	// 새로 추가 또는 이동
	UFUNCTION(BlueprintPure, Category = "Movement State Machine")
	bool IsInGuardCombatState() const;

	/*
	 * 현재 이동 입력이 있는지 확인하는 헬퍼 함수
	 * 가속도와 bShouldMove 값을 종합적으로 판단합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	bool HasMovementInput() const;

	/*
	 * 현재 속도에 따라 목표 Cycle 상태를 결정하는 함수
	 * Walk 또는 Sprint 중 어느 상태로 가야 하는지 판단합니다.
	 */
	EMovementState DetermineTargetCycleState() const;
#pragma endregion

//@델리게이트
#pragma region Delegates
#pragma endregion

//@콜백
#pragma region Callbacks
protected:
	//@Lock On 상태 변화 이벤트 구독
	UFUNCTION()
	void OnLockOnStateChanged(bool bIsLockOn, AActor* LockOnTargetActor);

protected:
	//@Attribute 변화 이벤트 구독
	UFUNCTION()
	void OnCombatStateAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);

protected:
	UFUNCTION()
	void MontageStarted(UAnimMontage* Montage);
	UFUNCTION()
	void MontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion

//@유틸리티 (Setter, Getter 등)
#pragma region Utility
protected:
	// === 참조 오브젝트들 ===

	//@Owner Character 캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<ACharacterBase> OwnerCharacterBaseRef;

	//@Character Movement 캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementCompRef;

	//@Combat State 속성 수치 변화 이벤트 관찰자
	UPROPERTY()
	TObjectPtr<UAsyncTaskAttributeChanged> CombatStateAttributeListenerRef;

protected:
	// === Combat Type 변환 유틸리티들 ===
	float CombatTypeToFloat(ECombatType Type)
	{
		return static_cast<float>(Type);
	}

	ECombatType FloatToCombatType(float Value)
	{
		// 반올림하여 가장 가까운 정수값으로 변환
		int32 IntValue = FMath::RoundToInt(Value);
		return static_cast<ECombatType>(FMath::Clamp(IntValue, 0, static_cast<int32>(ECombatType::MAX) - 1));
	}

protected:
	// === Root Motion 유틸리티 ===
	bool IsFullBodySlotMontage(const UAnimMontage* Montage) const;

public:
	// === 기본 Getter 함수들 ===

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE EMovementState GetLastMovementState() const { return LastMovementState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EMovementDirection GetMovementDirection() const { return MovementDirection; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE EStopMotionType GetStopMotionType() const { return StopMotionType; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE EMovementDirection GetLastMovementDirection() const { return LastMovementDirection; }

	UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
	FORCEINLINE float GetDirectionAngle() const { return DirectionAngle; }

	UFUNCTION(BlueprintCallable, Category = "Animation | Combat")
	ECombatType GetCombatType() const { return CombatType; }

	UFUNCTION(BlueprintCallable, Category = "Animation | Combat")
	FORCEINLINE bool GetIsPlayingRootMotionMontageWithFullBodySlot() const { return bIsPlayingRootMotionMontageWithFullBodySlot; }

public:
	// === 상태 기계 관련 Getter 함수들 ===

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetCanSkipStartState() const { return bCanSkipStartState; }

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool GetCanSkipStopState() const { return bCanSkipStopState; }

public:
	// === 상태 확인 편의 함수들 (Animation Blueprint에서 사용하기 쉽도록) ===

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInIdleState() const { return MovementState == EMovementState::Idle; }

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInStartState() const { return MovementState == EMovementState::Start; }

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInCycleState() const { return MovementState == EMovementState::Cycle_Walk || MovementState == EMovementState::Cycle_Sprint; }

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInStopState() const { return MovementState == EMovementState::Stop; }

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInWalkCycle() const { return MovementState == EMovementState::Cycle_Walk; }

	UFUNCTION(BlueprintPure, Category = "Movement State Machine", meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsInSprintCycle() const { return MovementState == EMovementState::Cycle_Sprint; }

public:
	// === 디버깅 및 개발 지원 함수들 ===

	/*
	 * 현재 상태 기계의 모든 핵심 정보를 문자열로 반환
	 * 디버깅이나 로깅 목적으로 사용합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement State Machine")
	FString GetCurrentStateDebugInfo() const;

public:
	// === Setter 함수들 ===

	UFUNCTION(BlueprintCallable, Category = "Animation")
	FORCEINLINE void SetIsPlayingRootMotionMontageWithFullBodySlot(bool InBool) { bIsPlayingRootMotionMontageWithFullBodySlot = InBool; }
#pragma endregion

};