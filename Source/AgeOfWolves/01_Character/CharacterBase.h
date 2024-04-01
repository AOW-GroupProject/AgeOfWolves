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
	* [����] : Character ��ü�� ASC�� WeakObjectPtr �������� �����մϴ�.
	* [����] : ASC�� Player State���� �����Ͽ� Ŀ�ø��� ��ȭ�ϰ�, ĳ���� ��ü�� TWeakObjectPtr ������ ������ ���� ������ �����͸� ���� �ֽ��ϴ�.
	*/
	TWeakObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent;

public:
	/*
	* [����] :	AbilitySytemComponent�� �������� Getter�� �����մϴ�.
	* [����] :	�ش� �Լ��� �������̽� �Լ���, IAbilitySytemInterface Ŭ�������� ����Ǿ�, �ش� �Լ� ȣ�� �� �������� ���� Ŭ������ �̸��� �� �ʿ� ����,
	*			IAbilitySystemInterface�� ������ ȣ���� �� �ֽ��ϴ�. ����, GetAbilitySystemComponent() �������̽� �Լ��� �������̽� Ŭ������ ����
	*			ȣ���ϴ� ���� ����˴ϴ�.
	*/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

#pragma endregion

};

