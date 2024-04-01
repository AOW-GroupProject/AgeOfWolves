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
class UBaseAbilitySystemComponent;
class UBaseAttributeSet;

UCLASS()
class AGEOFWOLVES_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

#pragma region Default Setting

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void PossessedBy(AController* NewController) override;

#pragma endregion

#pragma region Gameplay Ability System

protected:
	/*
	* [목적] : Character 객체는 ASC를 WeakObjectPtr 형식으로 참조합니다.
	* [설명] : ASC는 Player State에서 관리하여 커플링을 완화하고, 캐릭터 객체는 TWeakObjectPtr 형식의 느슨한 참조 형식의 포인터만 갖고 있습니다.
	*/
	TWeakObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent;

public:
	/*
	* [목적] :	AbilitySytemComponent를 가져오는 Getter를 정의합니다.
	* [설명] :	해당 함수는 인터페이스 함수로, IAbilitySytemInterface 클래스에서 선언되어, 해당 함수 호출 시 세부적인 액터 클래스의 이름을 알 필요 없이,
	*			IAbilitySystemInterface를 가져와 호출할 수 있습니다. 따라서, GetAbilitySystemComponent() 인터페이스 함수는 인터페이스 클래슬르 통해
	*			호출하는 것이 권장됩니다.
	*/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

#pragma endregion

};

