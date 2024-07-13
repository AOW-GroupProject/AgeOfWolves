#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "08_UI/UICollection.h"

#include "UIManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUIManager, Log, All)

/**
 * UUIManagerSubsystem
 * 
 * UI 관련 정보를 관리하는 Subsystem입니다.
 */
UCLASS()
class AGEOFWOLVES_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UUIManagerSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
public:
	//@특정 UI Category에 해당되는 모든 UI 정보를 제공하는 함수
	const TArray<FUIInformation>* GetUICategoryInformations(const EUICategory& UICategory) const;
	//@특정 UI의 정보를 제공하는 함수
	const FUIInformation* GetUIInformation(const EUICategory& UICategory, const FGameplayTag& UITag) const;
protected:
	UPROPERTY()
		UUICollection* UICollection;

};
