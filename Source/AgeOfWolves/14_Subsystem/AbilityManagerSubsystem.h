// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags.h" 
#include "02_AbilitySystem/BaseAbilitySet.h"

#include "AbilityManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilityManager, Log, All)


//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySet;
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
 *	@UAbilityManagerSubsystem
 * 
 *	Gameplay Ability 정보를 관리하는 서브시스템
 */
UCLASS()
class AGEOFWOLVES_API UAbilityManagerSubsystem : public UGameInstanceSubsystem
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAbilityManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@Ability 정보를 담은 Data Asset
	UPROPERTY()
		TObjectPtr<UBaseAbilitySet> AbilitySet;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UTexture2D* GetAbilityIconTexture2D(FGameplayTag AbilityTag);
#pragma endregion

};
