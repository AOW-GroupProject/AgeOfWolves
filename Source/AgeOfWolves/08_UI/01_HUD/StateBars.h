// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"

#include "StateBars.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStateBars, Log, All)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FStateBarsInitFinished);

class UProgressBar;
class UVerticalBox;

/**
 * UstateBars
 * 
 * @목적: HUD의 구성 요소로 캐릭터의 Health, Mana, Stamina 수치를 화면에 나타내는 UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API UStateBars : public UUserWidget
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	UStateBars(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
protected:
	//@외부 바인딩
	void ExternalBindingToAttributeSet();
public:
	//@초기화
	UFUNCTION()
		void InitializeStateBars();
#pragma endregion 

#pragma region SubWidgets
protected:
	TMap<FString, UProgressBar*> MStateBars;

	UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
		UVerticalBox* StateBarList;

	UPROPERTY(BlueprintReadWrite, Category="State Bar", meta=(BindWidget))
		UProgressBar* HP;
	UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
		UProgressBar* MP;
	UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
		UProgressBar* SP;

#pragma region Delegates
public:
	FStateBarsInitFinished StateBarsInitFinished;

#pragma endregion

#pragma region Callbacks
protected:
	/*
	* @목적 : OwningPawn의 PlayerState의 Attribute 값 변화 이벤트에 등록할 콜백 함수입니다.
	* @설명 : 해당 콜백 함수를 통해 HP, SP, MP 등의 캐릭터의 실시간 Attribute 값들을 가져와 UI에 반영할 수 있습니다.
	*/
	UFUNCTION()
		void OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion 

};
