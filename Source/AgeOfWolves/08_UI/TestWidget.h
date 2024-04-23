// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"

#include "TestWidget.generated.h"

class APlayerStateBase;

/**
 * @목적 : 캐릭터의 Attribute 관련 인터페이스 구현의 테스트를 위해 임시로 선언한 UI 클래스
 * @설명 : 캐릭터의 ASC의 Attribute 값 변화 이벤트에 UI 클래스의 콜백 함수를 등록하여, 특정 Attribute 값을 실시간으로 가져옵니다.
 * @참조 : -
 */
UCLASS()
class AGEOFWOLVES_API UTestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTestWidget(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	//~ End UUserWidget Interface

#pragma region Attribute Value
protected:
	/*
	* @목적 : OwningPawn의 PlayerState의 Attribute 값 변화 이벤트에 등록할 콜백 함수입니다.
	* @설명 : 해당 콜백 함수를 통해 HP, SP, MP 등의 캐릭터의 실시간 Attribute 값들을 가져와 UI에 반영할 수 있습니다.
	*/
	UFUNCTION()
		void OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion 

};
