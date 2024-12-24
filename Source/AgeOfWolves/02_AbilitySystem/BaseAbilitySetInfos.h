// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "BaseAbilitySetInfos.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBaseAbilitySetInfos, Log, All)

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
 *	@UBaseAbilitySetInfos
 * 
 *	모든 Base Ability Set을 담고 있는 Data Asset
 */
UCLASS()
class AGEOFWOLVES_API UBaseAbilitySetInfos : public UPrimaryDataAsset
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseAbilitySetInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GA 목록")
		TArray<TObjectPtr<UBaseAbilitySet>> BaseAbilitySets;
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
	void GetAbilitySets(TArray<UBaseAbilitySet*>& OutArray);

public:
	UBaseAbilitySet* GetBaseAbilitySet(const FGameplayTag& Tag) const;
#pragma endregion

};
