#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "08_UI/UICollection.h"

#include "UIManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUIManager, Log, All)


//@전방 선언
#pragma region Forward Declaration
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
 * UUIManagerSubsystem
 * 
 * UI 관련 정보를 관리하는 Subsystem입니다.
 */
UCLASS()
class AGEOFWOLVES_API UUIManagerSubsystem : public UGameInstanceSubsystem
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

		//@Defualt Setting
#pragma region Default Setting
public:
	UUIManagerSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY()
		TObjectPtr<UUICollection> UICollection;
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
	//@특정 UI Category에 해당되는 모든 UI 정보를 제공하는 함수
	const TArray<FUIInformation>* GetUICategoryInformations(const EUICategory& UICategory) const;
	//@특정 UI의 정보를 제공하는 함수
	const FUIInformation* GetUIInformation(const EUICategory& UICategory, const FGameplayTag& UITag) const;
#pragma endregion

};
