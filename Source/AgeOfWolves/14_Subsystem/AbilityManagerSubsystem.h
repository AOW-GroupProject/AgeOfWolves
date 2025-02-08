#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTags.h" 
#include "02_AbilitySystem/BaseAbilitySet.h"

#include "AbilityManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilityManager, Log, All)


//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySetInfos;
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

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩

protected:
	//@초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY()
		TObjectPtr<UBaseAbilitySetInfos> AbilitySetInfos;

	UPROPERTY()
		TMap<FGameplayTag, TObjectPtr<UBaseAbilitySet>> MAbilitySets;
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
	UBaseAbilitySet* GetAbilitySet(FGameplayTag CharacterTag) const;

public:
	UFUNCTION()
		UTexture2D* GetAbilityIconTexture2D(
			const FGameplayTag& AbilityTag,
			const FGameplayTag& CharacterTag = FGameplayTag::RequestGameplayTag("Character.AkaOni")) const;
#pragma endregion

};
