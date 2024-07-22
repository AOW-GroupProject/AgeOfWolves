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

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUIInputTriggered, const FGameplayTag&, UIInputTag);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUIInputReleased, const FGameplayTag&, UIInputTag);

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
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End Of UActorComponent interface
public:
	/*
	* @목적 : 사용자의 입력 시스템의 초기화 작업을 수행하는 함수입니다.
	* @설명 : 사용자 캐릭터의 Pawn Data의 InputConfing(Data Asset)을 통해 Enhanced Input System의 초기화 작업을 진행합니다.
	*/
	bool LoadPlayerInputSetup();

	// @목적 : 사용자 입력 시스템 초기화 작업 완료 이벤트
	FPlayerInputInit OnPlayerInputInitFinished;
#pragma endregion 

#pragma region IMC(Input Mapping Context)
public:
	// @설명 : Enhanced Input System에 사용자의 IMC(Input Mapping Context)를 등록합니다.
	void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	// @설명 : Enhanced Input System에 등록된 사용자의 특정 IMC(Input Mapping Context)를 제거합니다.
	void RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
#pragma endregion

#pragma region Binding
	void BindNativeInputActions(const UInputConfig* InputConfig);
	void BindAbilityInputActions(const UInputConfig* InputConfig);
	void BindUIInputActions(const UInputConfig* InputConfig);
	template<typename UserClass, typename FuncType>
	void BindNativeInputAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		check(InputConfig);

		if (const auto& InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
		{
			BindAction(InputAction, TriggerEvent, Object, Func);
		}
	}
	template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindInputActions(const TArray<FInputActionInfo>& InputActionInfos, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
	{
		if (InputActionInfos.IsEmpty()) return;

		for (const auto& InputActionInfo : InputActionInfos)
		{
			if (InputActionInfo.InputAction && InputActionInfo.InputTag.IsValid())
			{
				if (PressedFunc)
				{
					BindAction(InputActionInfo.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, InputActionInfo.InputTag);
				}

				if (ReleasedFunc)
				{
					BindAction(InputActionInfo.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputActionInfo.InputTag);
				}
			}
		}
	}
	// @breif : Remove BindHandles Interacting with corressponding Ability Input Action
	void RemoveBinds(TArray<uint32>& BindHandles);
#pragma endregion

#pragma region Native Input Action
protected:
	// @목적 : 사용자의 Move IA에 대응되는 캐릭터 이동 조작을 정의하는 콜백 함수
	void Input_Move(const FInputActionValue& Value);
	// @목적 : 사용자의 Looking IA에 대응되는 카메라 시야 방향 조작을 정의하는 콜백 함수 
	void Input_Look(const FInputActionValue& Value);
	// @목적 : 사용자의 LockOn IA에 대응되는 Lock On 기능을 정의하는 콜백 함수 
	void Input_LockOn(const FInputActionValue& Value);
	// @목적 : 사용자의 LeftMouse IA에 대응되는 Left Mouse 입력 처리를 담당하는 콜백 함수 
	void Input_LeftMousePressed(const FInputActionValue& Value);

	void Input_CountMouseLeftInput();

	// @설명 : 현재 플레이어의 입력 벡터
	FVector2D InputVector;

public:
	FORCEINLINE AActor* GetTargetEnemy() { return TargetEnemy; }
	FORCEINLINE bool GetbLockOn() { return bLockOn; }
	FORCEINLINE float GetMaxLockOnDistance() { return MaxLockOnDistance; }
	FORCEINLINE FVector2D GetInputVector() { return InputVector; }
#pragma endregion

#pragma region Ability Input Action
protected:
	// @목적 : 사용자의 Ability IA관련 키 눌림에 대응되는 콜백 함수
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// @목적 : 사용자의 Ability IA관련 키 해제에 대응되는 콜백 함수
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
#pragma endregion

#pragma region UI Input Action
public:
<<<<<<< HEAD
	FOnUIInputTriggered OnUIInputTriggered;
	FOnUIInputReleased OnUIInputReleased;

private: 
	void Input_UIInputTagTriggered(FGameplayTag InputTag);
	void Input_UIInputTagReleased(FGameplayTag InputTag);
#pragma endregion

=======
	FORCEINLINE FVector2D GetInputVector() { return InputVector; }
>>>>>>> develop
};