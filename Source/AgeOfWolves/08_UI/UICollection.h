#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "UICollection.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUICollection, Log, All)

class UUserWidget;

//@UI validation error types
UENUM(BlueprintType)
enum class EUICollectionValidationError : uint8
{
    None                            UMETA(DisplayName = "No Error"),

    //@Duplicate errors
    DuplicateUITagSameCategory      UMETA(DisplayName = "Duplicate UITag Same Category"),
    DuplicateUITagCrossCategory     UMETA(DisplayName = "Duplicate UITag Cross Category"),
    DuplicateInputTag               UMETA(DisplayName = "Duplicate InputTag"),

    //@Null reference errors
    NullUIClass                     UMETA(DisplayName = "Null UI Class"),

    //@Invalid value errors
    InvalidUITag                    UMETA(DisplayName = "Invalid UITag"),
    InvalidInputTag                 UMETA(DisplayName = "Invalid InputTag"),
    InvalidUICategory               UMETA(DisplayName = "Invalid UI Category"),

    //@Logical consistency errors
    InputBindingInconsistency       UMETA(DisplayName = "Input Binding Inconsistency"),
    TooManyBeginPlayUIs             UMETA(DisplayName = "Too Many BeginPlay UIs"),
    CategoryMismatch                UMETA(DisplayName = "Category Mismatch"),

    //@새로 추가된 최소 표시 시간 관련 에러들
    InvalidMinimumDisplayTime       UMETA(DisplayName = "Invalid Minimum Display Time"),
    MinimumDisplayTimeOnNonSystemUI UMETA(DisplayName = "Minimum Display Time On Non-System UI")
};

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
    System		UMETA(DisplayName = "System"),
    MAX,
};

//@UI validation result structure
USTRUCT(BlueprintType)
struct FUICollectionValidationResult
{
    GENERATED_BODY()

public:
    //@Validation success status
    UPROPERTY(BlueprintReadOnly)    
    bool bIsValid = true;

    //@Error type identifier
    UPROPERTY(BlueprintReadOnly)
    EUICollectionValidationError ErrorType = EUICollectionValidationError::None;

    //@Detailed error message
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    //@Problem category index
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemCategory = -1; //@0=HUD, 1=Menu, 2=Interaction, 3=System

    //@Problem array index
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemIndex = -1;

    //@Problem input tag index
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemInputTagIndex = -1;

    //@Default success constructor
    FUICollectionValidationResult()
    {
    }

    //@Error case constructor
    FUICollectionValidationResult(EUICollectionValidationError InErrorType, const FString& InErrorMessage,
        int32 InCategory = -1, int32 InIndex = -1, int32 InInputTagIndex = -1)
        : bIsValid(false)
        , ErrorType(InErrorType)
        , ErrorMessage(InErrorMessage)
        , ProblemCategory(InCategory)
        , ProblemIndex(InIndex)
        , ProblemInputTagIndex(InInputTagIndex)
    {
    }
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

    //@최소 그리기 보장 시간 사용 여부 (System UI에만 적용)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | Minimum Display Time", EditCondition = "UICategory == EUICategory::System"))
    bool bUseMinimumDisplayTime = false;

    //@최소 그리기 보장 시간 (초 단위, 2-10초 범위)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "UI Information | Minimum Display Time", EditCondition = "bUseMinimumDisplayTime == true && UICategory == EUICategory::System", ClampMin = "2", ClampMax = "10"))
    int32 MinimumDisplayTimeSeconds = 5;
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

#if WITH_EDITOR
    //@Editor property change validation
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
#if WITH_EDITOR
    //@Core validation functions
    FUICollectionValidationResult ValidateUICollection() const;
    FUICollectionValidationResult ValidateUIArrays() const;
    FUICollectionValidationResult ValidateUITagUniqueness() const;
    FUICollectionValidationResult ValidateInputTagUniqueness() const;
    FUICollectionValidationResult ValidateUILogicalConsistency() const;
    FUICollectionValidationResult ValidateCategoryConsistency() const;
    FUICollectionValidationResult ValidateBeginPlayUILimits() const;
    FUICollectionValidationResult ValidateMinimumDisplayTime() const;

    //@Error handling functions
    void ShowValidationError(const FUICollectionValidationResult& ValidationResult) const;
    bool TryAutoFixValidationError(const FUICollectionValidationResult& ValidationResult);

    //@Utility validation helpers
    bool IsValidUITag(const FGameplayTag& Tag) const;
    bool IsValidInputTag(const FGameplayTag& Tag) const;
    const TCHAR* GetCategoryName(int32 CategoryIndex) const;
    const TCHAR* GetCategoryName(EUICategory Category) const;
    TArray<FUIInformation>* GetUIArrayByCategory(int32 Category);
    const TArray<FUIInformation>* GetUIArrayByCategory(int32 Category) const;
#endif

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
    //@System UI Informations
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI | System")
    TArray<FUIInformation> SystemUIInformations;

#if WITH_EDITOR
    //@Manual validation functions
    UFUNCTION(CallInEditor, Category = "UI Validation")
    void ValidateUICollectionManually();

    UFUNCTION(CallInEditor, Category = "UI Validation")
    void ValidateHUDUIOnly();

    UFUNCTION(CallInEditor, Category = "UI Validation")
    void ValidateMenuUIOnly();

    UFUNCTION(CallInEditor, Category = "UI Validation")
    void ValidateInteractionUIOnly();

    UFUNCTION(CallInEditor, Category = "UI Validation")
    void ValidateSystemUIOnly();
#endif

};