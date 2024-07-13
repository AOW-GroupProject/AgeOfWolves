// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "CharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All)

class UPawnData;
class UBaseAttributeSet;
class UBaseAbilitySystemComponent;

UCLASS()
class AGEOFWOLVES_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region Default Setting

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	//~UObject interface
	virtual void BeginPlay() override;
	//~End Of UObject Interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	//~End Of APawn interface
#pragma endregion

#pragma region Gameplay Ability System
protected:
	/*
	* @목적 : Player State로부터 가져온 ASC에 대한 약한 참조를 관리합니다.
	* @설명 : 이후 ASC관련 내용을 정의할 것을 대비하여, 일단 약한 참조만 들고 있습니다.
	* @참조 : -
	*/
	TWeakObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent;

public:
	/*
	* @목적 : IAbilitySystemInterface 인터페이스 클래스의 인터페이스 함수를 오버라이딩 합니다.
	* @설명 : -
	* @참조 : - 
	*/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion

};

