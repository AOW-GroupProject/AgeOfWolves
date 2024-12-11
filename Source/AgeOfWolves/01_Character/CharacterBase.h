// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "CharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UPawnData;
class UBaseAttributeSet;
class UBaseAbilitySystemComponent;
class UMotionWarpingComponent;
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

UCLASS()
class AGEOFWOLVES_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	//~UObject interface
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	//~End Of UObject Interface
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UMotionWarpingComponent* MotionWarpingComponent;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@ASC 캐싱
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void SetAbilitySystemComponent(UAbilitySystemComponent* ASC);
#pragma endregion
};

