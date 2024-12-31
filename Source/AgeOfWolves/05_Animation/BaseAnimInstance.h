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
*	현재 이동 상태 관련 열거형
*/
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle = 0    UMETA(DisplayName = "Idle"),
	Walking     UMETA(DisplayName = "Walking"),
	Sprinting   UMETA(DisplayName = "Sprinting"),
	MAX         UMETA(DisplayName = "MAX"),
};

//@TODO: FL, FR, BL, BR 추가 예정 -> Start 애니메이션 추가 시
/*
*	@EMovementDirection
*
*	이동 방향 관련 열거형
*/
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Fwd = 0		UMETA(DisplayName = "Fwd"),
	Bwd			UMETA(DisplayName = "Bwd"),
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
	MAX			UMETA(DisplayName = "MAX"),
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
 */
UCLASS()
class AGEOFWOLVES_API UBaseAnimInstance : public UAnimInstance
{
	//@친추 클래스
#pragma region Friend Class
	friend class UANS_Deceleration;
	friend class UAN_UpdateStopMotionType;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
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

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UFUNCTION(BlueprintCallable)
		void FindMovementState();

	UFUNCTION(BlueprintCallable)
		void FindMovementDirectionAngle();

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ChangeBoneTransform(float DeltaTime);
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) { }

	UFUNCTION(BlueprintNativeEvent)
		void ClearBoneTransform(float DeltaTime);
	virtual void ClearBoneTransform_Implementation(float DeltaTime) { }

protected:
	//@Movement Setting 업데이트
	UFUNCTION()
		void UpdateMovementSettings();

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
		void UpdateStopMotionType(EStopMotionType Type);

protected:
	//@Combat State 속성 변화 이벤트 관찰
	void ListenToCombatStateAttributeChange();

protected:
	//@직전 이동 상태에 따른 스탑 모션
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "이동 | 이동 상태", meta = (AlloPrivateAccess = "true"))
		EStopMotionType StopMotionType;

	//@직전 이동 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "이동 | 이동 상태", meta = (AlloPrivateAccess = "true"))
		EMovementState LastMovementState;

	//@이동 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "이동 | 이동 상태", meta = (AlloPrivateAccess = "true"))
		EMovementState MovementState;

	//@이동 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "이동 | 이동 방향", meta = (AlloPrivateAccess = "true"))
		EMovementDirection MovementDirection;

protected:
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
	const float WalkingSpeed = 200.f;
	const float SprintingSpeed = 500.f;

protected:
	UPROPERTY()
		bool bIsSprintingCooldown;
	UPROPERTY()
		float SprintingCooldownTime;

	UPROPERTY(EditAnywhere, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
		float SprintingCooldownDuration;
	// 현재 누적된 Cooldown 시간을 추적
	float CurrentCooldownTime;

protected:
	UPROPERTY()
		bool bIsInDeceleration;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bModifyBoneTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BoneTransformLerpSpeed;

protected:
	//@전투/비전투 여부
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		ECombatType CombatType;

protected:
	//@루트 모션 재생 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		bool bIsPlayingRootMotionMontage;

#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Lock On 상태 변화 이벤트 구독
	UFUNCTION()
		void OnLockOnStateChanged(bool bIsLockOn);

protected:
	UFUNCTION()
		void OnDecelerationStateChanged(bool bIsDecelerating);

protected:
	//@Attribute 변화 이벤트 구독
	UFUNCTION()
		void OnCombatStateAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@Owner Character 캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TWeakObjectPtr<ACharacterBase> OwnerCharacterBaseRef;
	
	//@Character Movement 캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementCompRef;

	//@Combat State 속성 수치 변화 이벤트 관찰자
	UPROPERTY()
		TObjectPtr<UAsyncTaskAttributeChanged> CombatStateAttributeListenerRef;

public:
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
		FORCEINLINE float GetDirectionAngle() const { return DirectionAngle; }

	UFUNCTION(BlueprintCallable, Category = "Animation | Combat")
		ECombatType GetCombatType() const { return CombatType; }

	UFUNCTION(BlueprintCallable, Category = "Animation | Combat")
		FORCEINLINE void SetIsPlayingRootMotionMontage(bool InBool) { bIsPlayingRootMotionMontage = InBool; }
#pragma endregion

};