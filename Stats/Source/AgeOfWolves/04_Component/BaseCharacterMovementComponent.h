// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BaseCharacterMovementComponent.generated.h"


//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UBaseCharacterMovementComponent
 * 
 *	캐릭터 이동 컴포넌트의 기본 유형이 되는 Movement Component입니다.
 */
UCLASS()
class AGEOFWOLVES_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

		GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	//End UActorComponent Interface
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@Max Walk Speed 업데이트
	UFUNCTION(BlueprintCallable, Category = "Character Movement")
		void ChangeMoveSpeed(float InOldVal, float InNewVal);

private:
	//@현재 Max Walk Speed가 650.f 이상인지 여부
	bool IsSprinting;

private:
	//@걷기 가속도
	const float WalkAcceleration = 800.f;
	//@달리기 가속도
	const float SprintAcceleration = 1024.f;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Max Walk Speed 변화 이벤트를 구독합니다.
	UFUNCTION()
		void MoveSpeedChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
