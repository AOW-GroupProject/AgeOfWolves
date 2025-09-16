#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputConfig.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInputConfig, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UInputAction;
class UInputMappingContext;
#pragma endregion

//@열거형
#pragma region Enums
//@Input validation error types
UENUM(BlueprintType)
enum class EInputConfigValidationError : uint8
{
    None                            UMETA(DisplayName = "No Error"),
    DuplicateInputTagSameCategory   UMETA(DisplayName = "Duplicate InputTag Same Category"),
    DuplicateInputTagCrossCategory  UMETA(DisplayName = "Duplicate InputTag Cross Category"),
    NullInputAction                 UMETA(DisplayName = "Null InputAction"),
    NullInputMappingContext         UMETA(DisplayName = "Null InputMappingContext"),
    InvalidInputTag                 UMETA(DisplayName = "Invalid InputTag"),
    InvalidIMCTag                   UMETA(DisplayName = "Invalid IMC Tag"),
    InvalidMappingPriority          UMETA(DisplayName = "Invalid Mapping Priority"),
    EmptyInputActionArray           UMETA(DisplayName = "Empty InputAction Array"),
    InputTagWithoutAction           UMETA(DisplayName = "InputTag Without Action"),
    InputActionWithoutTag           UMETA(DisplayName = "InputAction Without Tag")
};
#pragma endregion

//@구조체
#pragma region Structs
//@Validation result data
USTRUCT(BlueprintType)
struct FInputConfigValidationResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    bool bIsValid = true;

    UPROPERTY(BlueprintReadOnly)
    EInputConfigValidationError ErrorType = EInputConfigValidationError::None;

    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    UPROPERTY(BlueprintReadOnly)
    int32 ProblemCategory = -1; //@0=Native, 1=Ability, 2=UI

    UPROPERTY(BlueprintReadOnly)
    int32 ProblemIndex = -1;

    //@Success constructor
    FInputConfigValidationResult()
    {
    }

    //@Error constructor
    FInputConfigValidationResult(EInputConfigValidationError InErrorType, const FString& InErrorMessage,
        int32 InCategory = -1, int32 InIndex = -1)
        : bIsValid(false)
        , ErrorType(InErrorType)
        , ErrorMessage(InErrorMessage)
        , ProblemCategory(InCategory)
        , ProblemIndex(InIndex)
    {
    }
};

USTRUCT(BlueprintType)
struct FInputActionInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
    FGameplayTag InputTag;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAction = nullptr;
};
#pragma endregion

/**
 * @UInputConfig
 * 
 * Enhanced Input 활용을 위해 필요한 정보들을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UInputConfig(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
    //@Editor property change validation
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
#if WITH_EDITOR
    //@Core validation functions
    FInputConfigValidationResult ValidateInputConfig() const;
    FInputConfigValidationResult ValidateBasicSettings() const;
    FInputConfigValidationResult ValidateInputMappingContext() const;
    FInputConfigValidationResult ValidateInputActionArrays() const;
    FInputConfigValidationResult ValidateInputTagUniqueness() const;
    FInputConfigValidationResult ValidateInputActionConsistency() const;

    //@Error handling functions
    void ShowValidationError(const FInputConfigValidationResult& ValidationResult) const;
    bool TryAutoFixValidationError(const FInputConfigValidationResult& ValidationResult);

    //@Utility validation helpers
    bool IsValidInputTag(const FGameplayTag& Tag) const;
    bool IsValidIMCTag(const FGameplayTag& Tag) const;
    const TCHAR* GetCategoryName(int32 CategoryIndex) const;
    TArray<FInputActionInfo>* GetInputActionArrayByCategory(int32 Category);
#endif

public:
    UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
    const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
    UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
    const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
    UFUNCTION(BlueprintCallable, Category = "Input | Input Action")
    const UInputAction* FindUIInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
    FGameplayTag IMCTag;
    //@IMC
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
    TObjectPtr<UInputMappingContext> InputMappingContext;
    //@Pirority
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Input Mapping Context")
    int32 MappingPriority;
    //@Native IA
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Native Input Action")
    TArray<FInputActionInfo> NativeInputActions;
    //@Ability IA
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | Ability Input Action")
    TArray<FInputActionInfo> AbilityInputActions;
    //@UI IA
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input | UI Input Action")
    TArray<FInputActionInfo> UIInputActions;

#if WITH_EDITOR
    //@Manual validation functions
    UFUNCTION(CallInEditor, Category = "Input Validation")
    void ValidateInputConfigManually();

    UFUNCTION(CallInEditor, Category = "Input Validation")
    void ValidateNativeActionsOnly();

    UFUNCTION(CallInEditor, Category = "Input Validation")
    void ValidateAbilityActionsOnly();

    UFUNCTION(CallInEditor, Category = "Input Validation")
    void ValidateUIActionsOnly();
#endif
};