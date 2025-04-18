﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "06_Input/InputConfig.h"

#include "BaseInputComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInputComponent, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@Input Component 초기화 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotifyInputComponentInitFinished);

DECLARE_MULTICAST_DELEGATE_OneParam(FUIInputTagTriggered, const FGameplayTag&);
DECLARE_MULTICAST_DELEGATE_OneParam(FUIInputTagReleased, const FGameplayTag&);

DECLARE_MULTICAST_DELEGATE_TwoParams(FNativeInputTagTriggeredWithValue, const FGameplayTag&, const float);

DECLARE_MULTICAST_DELEGATE_TwoParams(FUIInputTagTriggeredWithValue, const FGameplayTag&, const float);
#pragma endregion

/**
 * @목적 : Enhanced Input System 활용을 위한 사용자 정의 Input Component를 정의합니다.
 * @설명 : 일반적인 Input Component 대신, Enhanced Input System 활용이 가능한 사용자 정의 Input Component를 정의하고,
 *		   해당 컴포넌트를 ACharacterBase(사용자 캐릭터 클래스)의 Default Input Component로 설정합니다.	
 * @참조 : APlayerCharacter() 생성자
 */
UCLASS()
class AGEOFWOLVES_API UBaseInputComponent : public UEnhancedInputComponent
{
	//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()
		
//@Defualt Setting
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

protected:
	//@외부 바인딩

protected:
	//@내부 바인딩
	void InternalBindToInputActions(const APlayerController* PC);

public:
	UFUNCTION()
		void InitializeInputComponent();
#pragma endregion 

//@Property/Info...etc
#pragma region IMC(Input Mapping Context)
protected:
	//@Bind IA Template
	template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindInputActions(const TArray<FInputActionInfo>& InputActionInfos, UserClass* Object,
		PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
	{
		if (InputActionInfos.IsEmpty()) return;

		for (const auto& InputActionInfo : InputActionInfos)
		{
			if (InputActionInfo.InputAction && InputActionInfo.InputTag.IsValid())
			{
				// Axis 타입 체크
				if (InputActionInfo.InputAction->ValueType == EInputActionValueType::Axis1D)
				{
					// Value가 첫 번째 매개변수, Tag가 두 번째 매개변수로 오도록 수정
					BindAction(InputActionInfo.InputAction, ETriggerEvent::Triggered,
						this, &UBaseInputComponent::OnUIInputTagValueTriggered, InputActionInfo.InputTag);
				}
				else
				{
					if (PressedFunc)
					{
						BindAction(InputActionInfo.InputAction, ETriggerEvent::Triggered,
							Object, PressedFunc, InputActionInfo.InputTag);
					}

					if (ReleasedFunc)
					{
						BindAction(InputActionInfo.InputAction, ETriggerEvent::Completed,
							Object, ReleasedFunc, InputActionInfo.InputTag);
					}
				}
			}
		}
	}

	//@Bind Native IA Template 
	template<typename UserClass, typename FuncType>
	void BindNativeInputAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag,
		ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		check(InputConfig);

		if (const auto& InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
		{
			BindAction(InputAction, TriggerEvent, Object, Func);
		}
	}


	void BindAxis1DNativeInputAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag,
		ETriggerEvent TriggerEvent)
	{
		check(InputConfig);

		if (const auto& InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
		{
			BindAction(InputAction, TriggerEvent,
				this, &UBaseInputComponent::OnNativeInputTagValueTriggered, InputTag);
		}
	}

	void BindNativeInputActions(const UInputConfig* InputConfig);
	void BindAbilityInputActions(const UInputConfig* InputConfig);
	void BindUIInputActions(const UInputConfig* InputConfig);

	void RemoveBinds(TArray<uint32>& BindHandles);

private:
	//@현재 최 우선순위로 설정된 IMC의 Gameplay Tag
	FGameplayTag CurrentIMCTag;

public:
	FORCEINLINE const FGameplayTag& GetCurrentIMCTag() { return CurrentIMCTag; }

public:
	// @설명 : Enhanced Input System에 사용자의 IMC(Input Mapping Context)를 등록합니다.
	void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	// @설명 : Enhanced Input System에 등록된 사용자의 특정 IMC(Input Mapping Context)를 제거합니다.
	void RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	//@Enhanced Input System에 등록된 사용자의 IMC의 우선순위를 변경해줍니다.
	UFUNCTION()
		void SwapMappings(const FGameplayTag& NewIMCTag);
#pragma endregion

//@Delegates
#pragma region Delegate
public:
	//@Input Component의 초기화 완료 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Input Component")
		FNotifyInputComponentInitFinished NotifyInputComponentInitFinished;
public:
	//@UI Input Tag의 활성화 이벤튼
	FUIInputTagTriggered UIInputTagTriggered;
	//@UI Input Tag의 해제 이벤트
	FUIInputTagReleased UIInputTagReleased;

	//@Native Input Action의 Axis1D 값 처리 이벤트
	FNativeInputTagTriggeredWithValue NativeInputTagTriggeredWithValue;

	//@UI Input Tag의 활성화 및 수치값 발생 IA 이벤트
	FUIInputTagTriggeredWithValue UIInputTagTriggeredWithValue;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	// @목적 : 사용자의 Move IA에 대응되는 캐릭터 이동 조작을 정의하는 콜백 함수
	void Input_Move(const FInputActionValue& Value);
	// @목적 : 사용자의 Looking IA에 대응되는 카메라 시야 방향 조작을 정의하는 콜백 함수 
	void Input_Look(const FInputActionValue& Value);
	// @목적 : 사용자의 LockOn IA에 대응되는 Lock On 기능을 정의하는 콜백 함수 
	void Input_LockOn(const FInputActionValue& Value);

protected:
	// @목적 : 사용자의 Ability IA관련 키 눌림에 대응되는 콜백 함수
	void OnAbilityInputTagPressed(FGameplayTag InputTag);
	// @목적 : 사용자의 Ability IA관련 키 해제에 대응되는 콜백 함수
	void OnAbilityInputTagReleased(FGameplayTag InputTag);

protected:
	//@UI Input Tag Triggered 이벤트를 구독하는 콜백
	void OnUIInputTagTriggered(FGameplayTag InputTag);
	//@UI Input Tag Released 이벤트를 구독하는 콜백
	void OnUIInputTagReleased(FGameplayTag InputTag);

protected:
	//@Native IA 중 Axis1D 처리 이벤틀르 구독
	void OnNativeInputTagValueTriggered(const FInputActionValue& Value, FGameplayTag InputTag);

protected:
	//@UI IA 중 Axis1D 처리 이벤트를 구독
	void OnUIInputTagValueTriggered(const FInputActionValue& Value, FGameplayTag InputTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	FVector2D InputVector;

public:
	UFUNCTION(BlueprintCallable	)
	FORCEINLINE FVector2D GetInputVector() { return InputVector; }
#pragma endregion

};