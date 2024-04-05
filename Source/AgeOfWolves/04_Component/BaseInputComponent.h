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
 * @���� : Enhanced Input System Ȱ���� ���� ����� ���� Input Component�� �����մϴ�.
 * @���� : �Ϲ����� Input Component ���, Enhanced Input System Ȱ���� ������ ����� ���� Input Component�� �����ϰ�,
 *		   �ش� ������Ʈ�� ACharacterBase(����� ĳ���� Ŭ����)�� Default Input Component�� �����մϴ�.	
 * @���� : APlayerCharacter() ������
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
	* @���� : ������� �Է� �ý����� �ʱ�ȭ �۾��� �����ϴ� �Լ��Դϴ�.
	* @���� : ����� ĳ������ Pawn Data�� InputConfing(Data Asset)�� ���� Enhanced Input System�� �ʱ�ȭ �۾��� �����մϴ�.
	*/
	void InitializePlayersInputActionsSetup();

public:
	// @���� : ����� �Է� �ý��� �ʱ�ȭ �۾� �Ϸ� �̺�Ʈ
	FPlayerInputInit OnPlayerInputInitFinished;

#pragma endregion 

#pragma region Binding
public:
	// @���� : Enhanced Input System�� ������� IMC(Input Mapping Context)�� ����մϴ�.
	void AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	// @���� : Enhanced Input System�� ��ϵ� ������� Ư�� IMC(Input Mapping Context)�� �����մϴ�.
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
	// @���� : ������� Move IA�� �����Ǵ� ĳ���� �̵� ������ �����ϴ� �ݹ� �Լ�
	void Input_Move(const FInputActionValue& Value);
	// @���� : ������� Looking IA�� �����Ǵ� ī�޶� �þ� ���� ������ �����ϴ� �ݹ� �Լ� 
	void Input_Look(const FInputActionValue& Value);

#pragma endregion

#pragma region Callbacks bind to Ability Input Action

protected:
	// @���� : ������� Ability IA���� Ű ������ �����Ǵ� �ݹ� �Լ�
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// @���� : ������� Ability IA���� Ű ������ �����Ǵ� �ݹ� �Լ�
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

#pragma endregion
};