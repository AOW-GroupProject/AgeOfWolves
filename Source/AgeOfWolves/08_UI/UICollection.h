#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "UICollection.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUICollection, Log, All)

class UUserWidget;

/*
* EUICategory
* 
* UI의 카테고리 목록을 열거형으로 정의합니다.
*/
UENUM(BlueprintType)
enum class EUICategory : uint8
{
	HUD = 0		UMETA(DisplayName = "HUD"),
	Menu		UMETA(DisplayName = "Menu"),
	Interaction	UMETA(DisplayName = "Interaction"),
	MAX,
};

/*
* FUIInfo
* 
* UI 관련 정보를 한데 묶어놓은 구조체
*/
USTRUCT(BlueprintType)
struct FUIInformation
{
	GENERATED_BODY()

public:
	//@UI Category
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | UI Category"))
		EUICategory UICategory = EUICategory::MAX;
	//@UI Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | UI Tag"))
		FGameplayTag UITag;
	//@UI Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | UI Class"))
		TSubclassOf<UUserWidget> UIClass;
	//@UI를 초기화 시점에 바로 Screen에 나타낼지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | UI Shown On BeginPlay"))
		bool bShownOnBeginPlay = false;
	//@Input에 바인딩 되어있는지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | Key Binding"))
		bool bInputBinded = false;
	//@Input Tags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | Key Binding", EditCondition = "bInputBinded == true"))
		TArray<FGameplayTag> InputTags;
};

/**
 * UUIcollection
 *
 * 카테고리 별 UI들을 관리하는 Data Asset입니다.
 */
UCLASS()
class AGEOFWOLVES_API UUICollection : public UDataAsset
{
    GENERATED_BODY()

public:
    UUICollection(const FObjectInitializer& ObjectInitializer);

public:
	const TArray<FUIInformation>& GetUICategoryInformations(const EUICategory& UICategory) const;
    
protected:
	//@HUD UI Informations
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI | HUD")
        TArray<FUIInformation> HUDUIInformations;
	//@Menu UI Informations
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI | Menu")
        TArray<FUIInformation> MenuUIInformations;
	//@Interaction UI Informations
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI | Interaction")
        TArray<FUIInformation> InteractionUIInformations;

 };