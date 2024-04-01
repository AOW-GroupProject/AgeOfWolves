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
 * @breif : Component used to manage Input Actions' bindings using 'Input Config' Data Asset 
 */
UCLASS()
class AGEOFWOLVES_API UBaseInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	UBaseInputComponent(const FObjectInitializer& ObjectInitializer);

	//void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	//void RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

#pragma region Default Setting

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// @brief : Initialize Player Input(IMC, Native IA, Ability IA)
	void InitPlayerInput();

public:
	// @brief : Handle for Player Input Mapping Initialization finished
	FPlayerInputInit OnPlayerInputInitFinished;

#pragma endregion 

#pragma region Binding

public:
	void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
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

#pragma endregion

#pragma region Callbacks bind to Ability Input Action

protected:
	// @목적 : 사용자의 Ability IA관련 키 눌림에 대응되는 콜백 함수
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// @목적 : 사용자의 Ability IA관련 키 해제에 대응되는 콜백 함수
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

#pragma endregion
};