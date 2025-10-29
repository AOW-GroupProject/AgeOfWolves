// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "PotionGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPotionGA, Log, All);

//@전방 선언
#pragma region Forward Declaration

#pragma endregion

//@열거형
#pragma region Enum
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@델리게이트
#pragma region Delegates
#pragma endregion
UCLASS()
class AGEOFWOLVES_API UPotionGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	//@Default Setting
#pragma region Default Setting
public:
	UPotionGameplayAbility(const FObjectInitializer& ObjectInitializer);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc


#pragma endregion

//@Delegated
#pragma region Delegates
private:

#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility

#pragma endregion
};
