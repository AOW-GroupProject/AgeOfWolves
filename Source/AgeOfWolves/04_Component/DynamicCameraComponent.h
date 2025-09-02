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
	Snap        UMETA(DisplayName="Snap"),
	Linear      UMETA(DisplayName="Linear"),
	EaseIn      UMETA(DisplayName="EaseIn"),
	EaseOut     UMETA(DisplayName="EaseOut"),
	EaseInOut   UMETA(DisplayName="EaseInOut"),
	EaseInQuad  UMETA(DisplayName="EaseInQuad"),
	Bounce      UMETA(DisplayName="Bounce")
};
#pragma endregion

//@구조체
#pragma region Structs
USTRUCT(BlueprintType)
struct FCameraTransitionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraAngle StartAngle = ECameraAngle::BackView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraAngle TargetAngle = ECameraAngle::BackView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator StartRotate = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator EndRotate= FRotator(0.f, 0.f,0.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "10.0", UIMin = "0.1", UIMax = "10.0"))
	float TransitionDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBlendCurve BlendCurve = EBlendCurve::EaseInOut;

	//@ 목표 상태 도달 후 복원 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldRestoreAfterDuration = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShouldRestoreAfterDuration"))
	float RestoreDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShouldRestoreAfterDuration"))
	float RestoreDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	int32 TransitionPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInterruptible = true;
};

USTRUCT(BlueprintType)
struct FCurrentCameraTransitionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsRestoreTransition = false;
	
	UPROPERTY()
	USpringArmComponent* StartSpringArm;

	UPROPERTY()
	USpringArmComponent* TargetSpringArm;

	FRotator StartRotate = FRotator::ZeroRotator;

	FRotator EndRotate= FRotator::ZeroRotator;

	UPROPERTY()
	float TransitionDuration = 1.0f;

	UPROPERTY()
	EBlendCurve BlendCurve = EBlendCurve::EaseInOut;
	
	UPROPERTY()
	float RestoreDelay = 0.0f;

	UPROPERTY()
	int32 TransitionPriority = 0;

	UPROPERTY()
	bool bInterruptible = true;
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	
protected:
	// 현재 실행 중인 전환 정보
	UPROPERTY()
	FCurrentCameraTransitionInfo CurrentTransition;
	
	//@ 작업큐 진행상태 체크 Flag
	bool bIsTransitioning = false;

	//@  전환 시간 추적
	float TransitionElapsedTime = 0.0f;
	
	//@  카메라 전환 시작 
	void StartTransition();

	//@  카메라 전환 완료 
	void CompleteCurrentTransition();

	//@  카메라 전환 작업 
	void ProcessCurrentTransition(float DeltaTime);
	
	//@ EBlendCurve에 따른 Ease 함수 적용
	float ApplyEaseFunction(float Alpha, EBlendCurve BlendCurve);
	
	//@ 카메라-소켓 TMap 유효성 검사 
	bool IsAngleValid(ECameraAngle Angle) const;

	bool IsMontagePlaying();
	
	USpringArmComponent* FindSpringArm(ECameraAngle Angle) const;
	
	TQueue<FCurrentCameraTransitionInfo> TransitionQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CameraAngle")
	TMap<ECameraAngle, FName> CameraAngleNameMap;

	UPROPERTY()
	TMap<ECameraAngle, USpringArmComponent*> CameraAngleSpringArmMap;

	UPROPERTY()
	FTransform CachedStartRelativeTransform;

	UPROPERTY()
	bool bCachedUsePawnControlRotation;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
	//@Pawn 캐싱
	UPROPERTY()
		TWeakObjectPtr<APlayerCharacter> PlayerCharacterRef;

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CameraComponentRef;
#pragma endregion
};