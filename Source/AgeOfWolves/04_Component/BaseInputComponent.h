// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "06_Input/InputConfig.h"

#include "BaseInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;

DECLARE_LOG_CATEGORY_EXTERN(LogInputComponent, Log, All)

DECLARE_DYNAMIC_DELEGATE(FPlayerInputInit);

/**
 * @목적 : Enhanced Input System 활용을 위한 사용자 정의 Input Component를 정의합니다.
 * @설명 : 일반적인 Input Component 대신, Enhanced Input System 활용이 가능한 사용자 정의 Input Component를 정의하고,
 *		   해당 컴포넌트를 ACharacterBase(사용자 캐릭터 클래스)의 Default Input Component로 설정합니다.	
 * @참조 : APlayerCharacter() 생성자
 */
UCLASS()
class AGEOFWOLVES_API UBaseInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	UBaseInputComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//~UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End Of UActorComponent interface

	/*
	* @목적 : 사용자의 입력 시스템의 초기화 작업을 수행하는 함수입니다.
	* @설명 : 사용자 캐릭터의 Pawn Data의 InputConfing(Data Asset)을 통해 Enhanced Input System의 초기화 작업을 진행합니다.
	*/
	void InitializePlayersInputActionsSetup();

public:
	// @목적 : 사용자 입력 시스템 초기화 작업 완료 이벤트
	FPlayerInputInit OnPlayerInputInitFinished;

#pragma endregion 

#pragma region Binding
public:
	// @설명 : Enhanced Input System에 사용자의 IMC(Input Mapping Context)를 등록합니다.
	void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	// @설명 : Enhanced Input System에 등록된 사용자의 특정 IMC(Input Mapping Context)를 제거합니다.
	void RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	// @brief : Tempalte function in which Native Input Action matching to the Input Tag will bind to User's Callback Function
	template<typename UserClass, typename FuncType>
	void BindNativeInputAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		check(InputConfig);

		if (const auto& InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
		{
			BindAction(InputAction, TriggerEvent, Object, Func);
		}
	}
	// @breif : Template function in which Ability Input Action matching to the Input Tag will bind to User's Callback Function
	template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityInputActions(const UInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
	{
		check(InputConfig);

		for (const auto& AbilityInputActionInfo : InputConfig->AbilityInputActions)
		{
			if (AbilityInputActionInfo.InputAction && AbilityInputActionInfo.InputTag.IsValid())
			{
				if (PressedFunc)
				{
					BindHandles.Add(BindAction(AbilityInputActionInfo.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, AbilityInputActionInfo.InputTag).GetHandle());
				}

				if (ReleasedFunc)
				{
					BindHandles.Add(BindAction(AbilityInputActionInfo.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, AbilityInputActionInfo.InputTag).GetHandle());
				}
			}
		}
	}

	// @breif : Remove BindHandles Interacting with corressponding Ability Input Action
	void RemoveBinds(TArray<uint32>& BindHandles);


#pragma endregion

#pragma region Callbacks bind to Native Input Action

protected:
	// @목적 : 사용자의 Move IA에 대응되는 캐릭터 이동 조작을 정의하는 콜백 함수
	void Input_Move(const FInputActionValue& Value);
	// @목적 : 사용자의 Looking IA에 대응되는 카메라 시야 방향 조작을 정의하는 콜백 함수 
	void Input_Look(const FInputActionValue& Value);
	// @목적 : 사용자의 LockOn IA에 대응되는 Lock On 기능을 정의하는 콜백 함수 
	void Input_LockOn(const FInputActionValue& Value);

public:
	// @목적 : LockOn을 시작하는 함수
	void StartLockOn();
	// @목적 : LockOn을 취소하는 함수
	void CancelLockOn();

protected:
	// @목적 : 사용자의 ChageLockOnTarget IA에 대응되는 Lock On Target 변경을 정의하는 콜백 함수 (사용하지 않음)
	void Input_ChangeLockOnTarget(const FInputActionValue& Value);


	// @목적 : TargetEnemy 방향으로 Controller Rotation을 설정하는 함수
	void SetControllerRotationTowardTarget();
	// @목적 : 주변 Enemy를 찾고 TargetEnemy를 설정하는 함수
	// @설명 : TargetEnemy를 찾았으면 true를 반환한다.
	bool FindTargetEnemy();

	// @설명 : 케릭터가 LockOn시 적을 찾는 감지 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Action | Native Input Action")
	float MaxDetectRadius = 1000.f;
	// @설명 : 케릭터가 LockOn을 유지하는 최대 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Action | Native Input Action")
	float  MaxLockOnDistance = 2000.f;

	// @설명 : Input_LockOn 함수 실행 시 찾은 적 Actor 담는 Array
	UPROPERTY()
	TArray<AActor*> NearByEnemies;
	// @설명 : nput_LockOn 함수 실행 시 계산한 내적값과 찾은 적 Actor를 담는 Map
	UPROPERTY()
	TMap<float, AActor*> EnemyMap;
	// @설명 : 현재 플레이어가 LockOn 중인 적 Actor
	UPROPERTY()
	AActor* TargetEnemy;
	// @설명 : 현재 플레이어가 LockOn 중인지 나타내는 bool 변수
	bool bLockOn = false;
	// @설명 : 현재 플레이어의 입력 벡터
	FVector2D InputVector;

	UPROPERTY(BlueprintReadWrite)
	int32 InputCount;

	void Input_CountMouseLeftInput();

#pragma endregion

#pragma region Callbacks bind to Ability Input Action

protected:
	// @목적 : 사용자의 Ability IA관련 키 눌림에 대응되는 콜백 함수
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// @목적 : 사용자의 Ability IA관련 키 해제에 대응되는 콜백 함수
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

#pragma endregion

public:
	FORCEINLINE AActor* GetTargetEnemy() { return TargetEnemy; }
	FORCEINLINE bool GetbLockOn() { return bLockOn; }
	FORCEINLINE float GetMaxLockOnDistance() { return MaxLockOnDistance; }
	FORCEINLINE FVector2D GetInputVector() { return InputVector; }
};