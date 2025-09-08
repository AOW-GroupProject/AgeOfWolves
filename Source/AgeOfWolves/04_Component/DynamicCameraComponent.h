// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicCameraComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDynamicCamera, Log, All)

//@전방 선언
#pragma region Forward Declaration
class USpringArmComponent;
class UCameraComponent;
class ULockOnComponent;
class APlayerCharacter;
#pragma endregion

//@열거형
#pragma region Enums
UENUM(BlueprintType)
enum class ECameraAngle : uint8
{
	BackView	   UMETA(DisplayName = "BackView"),
	LeftShoulder   UMETA(DisplayName = "LeftShoulder"),
	RightShoulder  UMETA(DisplayName = "RightShoulder"),
	CloseUp        UMETA(DisplayName = "CloseUp"),
	BustShot       UMETA(DisplayName = "BustShot"),
	FullShot       UMETA(DisplayName = "FullShot"),
	WideShot       UMETA(DisplayName = "WideShot"),
	LeftSide       UMETA(DisplayName = "LeftSide"),
	RightSide      UMETA(DisplayName = "RightSide"),
	HighAngle      UMETA(DisplayName = "HighAngle"),
	MAX
};

UENUM(BlueprintType)
enum class EBlendCurve : uint8
{
	Snap        UMETA(DisplayName = "Snap"),
	Linear      UMETA(DisplayName = "Linear"),
	EaseIn      UMETA(DisplayName = "EaseIn"),
	EaseOut     UMETA(DisplayName = "EaseOut"),
	EaseInOut   UMETA(DisplayName = "EaseInOut"),
	EaseInQuad  UMETA(DisplayName = "EaseInQuad"),
	Bounce      UMETA(DisplayName = "Bounce")
};

UENUM(BlueprintType)
enum class ECameraFallbackPolicy : uint8
{
	SnapToDefault         UMETA(DisplayName = "SnapToDefault"),
	EaseInToDefault       UMETA(DisplayName = "EaseInToDefault"),
	EaseOutToDefault      UMETA(DisplayName = "EaseOutToDefault"),
	EaseInOutToDefault    UMETA(DisplayName = "EaseInOutToDefault"),
	SnapToStartAngle      UMETA(DisplayName = "SnapToStartAngle"),
	EaseInToStartAngle    UMETA(DisplayName = "EaseInToStartAngle"),
	EaseOutToStartAngle   UMETA(DisplayName = "EaseOutToStartAngle"),
	EaseInOutToStartAngle UMETA(DisplayName = "EaseInOutToStartAngle"),
	UseRestoreSettings    UMETA(DisplayName = "UseRestoreSettings"),
	ContinueToTarget      UMETA(DisplayName = "ContinueToTarget"),
};

#pragma endregion



//@구조체
#pragma region Structs
USTRUCT(BlueprintType)
struct FCameraTransitionInfo
{
	GENERATED_BODY()

	//@ 전환 시점의 카메라 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraAngle StartAngle = ECameraAngle::BackView;

	//@ 전환 목표의 카메라 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraAngle TargetAngle = ECameraAngle::BackView;

	//@ 전환 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "10.0", UIMin = "0.1", UIMax = "10.0"))
	float TransitionDuration = 1.0f;

	//@ 전환에 사용할 보간 곡선
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBlendCurve BlendCurve = EBlendCurve::EaseInOut;

	//@ 목표 상태 도달 후 복원 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldRestoreAfterDuration = false;

	//@ 복원 시 전환 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShouldRestoreAfterDuration"))
	float RestoreDuration = 1.0f;

	//@ 복원 이전 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShouldRestoreAfterDuration"))
	float RestoreDelay = 0.0f;

	//@ 복원 시 전환에 사용할 보간 곡선
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBlendCurve RestoreBlendCurve = EBlendCurve::EaseIn;
	
	//@ 전환 요청 시 작업 우선 순위, 미구현
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	int32 TransitionPriority = 0;

	//@ 중단 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInterruptible = true;

	//@ 취소 처리 정책
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraFallbackPolicy FallbackPolicy = ECameraFallbackPolicy::SnapToDefault;
};

USTRUCT(BlueprintType)
struct FCurrentCameraTransitionInfo
{
	GENERATED_BODY()

	//@ 전환 시작 SpringArm
	UPROPERTY()
	USpringArmComponent* StartSpringArm;

	//@ 전환 목표 SpringArm
	UPROPERTY()
	USpringArmComponent* TargetSpringArm;

	//@ 전환 시간
	UPROPERTY()
	float TransitionDuration = 1.0f;

	//@ 전환에 사용할 보간 곡선
	UPROPERTY()
	EBlendCurve BlendCurve = EBlendCurve::EaseInOut;

	//@ 복원 여부
	UPROPERTY()
	bool bShouldRestore = false;

	//@ 복원 시 전환 시간
	UPROPERTY()
	float RestoreDuration = 1.0f;

	//@ 복원 시 사용할 보간 곡선
	UPROPERTY()
	EBlendCurve RestoreBlendCurve = EBlendCurve::EaseIn;

	//@ 복원 전 대기 시간
	UPROPERTY()
	float RestoreDelay = 0.0f;

	//@ 우선순위
	UPROPERTY()
	int32 TransitionPriority = 0;

	//@ 중단 가능 여부
	UPROPERTY()
	bool bInterruptible = true;

	//@ 현재 진행 단계 (전환 중인지, 복원 중인지)
	UPROPERTY()
	bool bIsInRestorePhase = false;

	//@ 복원 대기 시간 경과 추적
	UPROPERTY()
	float RestoreDelayElapsed = 0.0f;
		
	//@ 전환 시간 추적
	UPROPERTY()
	float TransitionElapsedTime = 0.0f;

	//@ 취소 처리 정책
	UPROPERTY()
	ECameraFallbackPolicy FallbackPolicy = ECameraFallbackPolicy::SnapToDefault;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UDynamicCameraComponent
 *
 *	카메라 앵글, 거리, FOV의 변경을 통한 역동적인 카메라 연출을 하는 Component
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UDynamicCameraComponent : public UActorComponent
{
	//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()
	//@Defualt Setting
#pragma region Default Setting
public:
	UDynamicCameraComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@초기화
	UFUNCTION()
	void InitializeDynamicCameraComp(const AController* Controller);

public:
	// 카메라 전환 정보를 큐에 추가하는 함수
	UFUNCTION()
	void AddCameraTransition(const FCameraTransitionInfo& NewTransition);

	// 인터럽트 정보를 큐에 추가하는 함수
	UFUNCTION()
	void AddCameraInterruptTransition(const FCurrentCameraTransitionInfo& NewInterruptTransition);

protected:
	// 현재 실행 중인 전환 정보
	UPROPERTY()
	FCurrentCameraTransitionInfo CurrentTransition;

	//@ 작업큐 진행상태 체크 Flag
	bool bIsTransitioning = false;

	//@  카메라 전환 시작 
	void StartTransition();

	//@  카메라 전환 완료 
	void CompleteCurrentTransition();

	//@  카메라 전환 작업 
	void ProcessCurrentTransition(float DeltaTime);

	//@ EBlendCurve에 따른 Ease 함수 적용
	float ApplyEaseFunction(float Alpha, EBlendCurve BlendCurve);

	//@ EBlendCurve에 따른 Ease 함수 적용
	void ApplyFallbackPolicy(const FCurrentCameraTransitionInfo& NewInterruptTransition, FCurrentCameraTransitionInfo& OutInterruptTransitionInfo);

	//@ 카메라-소켓 TMap 유효성 검사 
	bool IsAngleValid(ECameraAngle Angle) const;

	//@ 전환 시작 시 MontagePlaying 여부
	bool IsNotMontagePlaying();

	//@ ECameraAngle 통한 SpringArm 찾기
	USpringArmComponent* FindSpringArm(ECameraAngle Angle) const;

	//@ 전환 큐 (우선순위큐)
	TArray<FCurrentCameraTransitionInfo> TransitionQueue;

	//@ 블루프린트 에디터에서 미리 등록해야하는 ECameraAngle-SpringArmName Map 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAngle")
	TMap<ECameraAngle, FName> CameraAngleNameMap;

	//@ CameraAngleNameMap의 SpringArmName을 ECameraAngle-SpringArmComponent를 캐싱할  Map 
	UPROPERTY()
	TMap<ECameraAngle, USpringArmComponent*> CameraAngleSpringArmMap;

	// 보간 진행 이전 시작 위치를 캐싱할 Transform
	UPROPERTY()
	FTransform CachedStartRelativeTransform;

	// 보간 대상 위치를 캐싱할 Transform
	UPROPERTY()
	FTransform TargetRelativeTransform;

	void CheckHitInterrupt();

	UPROPERTY()
	bool bIsPlayerInHitState = false;
	
	// 우선순위 비교
	struct FTransitionPriorityPredicate
	{
		bool operator()(const FCurrentCameraTransitionInfo& A, const FCurrentCameraTransitionInfo& B) const
		{
			return A.TransitionPriority < B.TransitionPriority; 
		}
	};

	// 우선순위큐 등록
	void EnqueueTransition(const FCurrentCameraTransitionInfo& NewTransition);

	// 우선순위큐 해제
	bool DequeueTransition(FCurrentCameraTransitionInfo& OutTransition);
#pragma endregion

	//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
	UPROPERTY()
	TWeakObjectPtr<APlayerCharacter> PlayerCharacterRef;

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CameraComponentRef;

	UPROPERTY()
	TWeakObjectPtr<ULockOnComponent> LockOnComponentRef;
#pragma endregion
};