#include "InputConfig.h"
#include "Logging/StructuredLog.h"

#include "InputAction.h"
#include "InputMappingContext.h"

#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDeviceRedirector.h"

DEFINE_LOG_CATEGORY(LogInputConfig)

#pragma region Default Settings
UInputConfig::UInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

#if WITH_EDITOR
void UInputConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FInputConfigValidationResult ValidationResult = ValidateInputConfig();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);

        //@Try automatic fix if possible
        if (!TryAutoFixValidationError(ValidationResult))
        {
            UE_LOGFMT(LogInputConfig, Warning,
                "InputConfig 무결성 검사 실패 - 수동 수정 필요: {0}",
                *ValidationResult.ErrorMessage);
        }
    }
}

void UInputConfig::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    FInputConfigValidationResult ValidationResult = ValidateInputConfig();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);
        TryAutoFixValidationError(ValidationResult);
    }
}

FInputConfigValidationResult UInputConfig::ValidateInputConfig() const
{

    //@Step 1: Basic settings validation
    FInputConfigValidationResult Result = ValidateBasicSettings();
    if (!Result.bIsValid) return Result;

    //@Step 2: InputMappingContext validation
    Result = ValidateInputMappingContext();
    if (!Result.bIsValid) return Result;

    //@Step 3: InputAction arrays validation
    Result = ValidateInputActionArrays();
    if (!Result.bIsValid) return Result;

    //@Step 4: InputTag uniqueness check
    Result = ValidateInputTagUniqueness();
    if (!Result.bIsValid) return Result;

    //@Step 5: InputAction consistency check
    Result = ValidateInputActionConsistency();
    if (!Result.bIsValid) return Result;

    //@All validations passed
    return FInputConfigValidationResult();
}

FInputConfigValidationResult UInputConfig::ValidateBasicSettings() const
{
    //@IMCTag validity check
    if (!IsValidIMCTag(IMCTag))
    {
        return FInputConfigValidationResult(
            EInputConfigValidationError::InvalidIMCTag,
            FString::Printf(TEXT("IMCTag가 유효하지 않습니다: %s"), *IMCTag.ToString())
        );
    }

    //@MappingPriority range check
    if (MappingPriority < 0)
    {
        return FInputConfigValidationResult(
            EInputConfigValidationError::InvalidMappingPriority,
            FString::Printf(TEXT("MappingPriority는 음수일 수 없습니다: %d"), MappingPriority)
        );
    }

    //@Warn about extreme priority values
    if (MappingPriority > 100)
    {
        UE_LOGFMT(LogInputConfig, Warning,
            "MappingPriority가 매우 높습니다 ({0}). 다른 중요한 입력 컨텍스트를 덮어쓸 수 있습니다.",
            MappingPriority);
    }

    return FInputConfigValidationResult();
}

FInputConfigValidationResult UInputConfig::ValidateInputMappingContext() const
{
    //@Null InputMappingContext check
    if (!IsValid(InputMappingContext))
    {
        return FInputConfigValidationResult(
            EInputConfigValidationError::NullInputMappingContext,
            TEXT("InputMappingContext가 null이거나 유효하지 않습니다")
        );
    }

    return FInputConfigValidationResult();
}

FInputConfigValidationResult UInputConfig::ValidateInputActionArrays() const
{

    //@Category of InputActions
    const TArray<FInputActionInfo>* InputArrays[] = {
        &NativeInputActions,    //@Category 0
        &AbilityInputActions,   //@Category 1  
        &UIInputActions         //@Category 2
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 3; ++CategoryIndex)
    {
        const TArray<FInputActionInfo>& InputArray = *InputArrays[CategoryIndex];

        for (int32 ActionIndex = 0; ActionIndex < InputArray.Num(); ++ActionIndex)
        {
            const FInputActionInfo& ActionInfo = InputArray[ActionIndex];

            if (!IsValid(ActionInfo.InputAction))
            {
                return FInputConfigValidationResult(
                    EInputConfigValidationError::NullInputAction,
                    FString::Printf(TEXT("%s 배열의 %d번 인덱스에서 Null InputAction 발견"),
                        GetCategoryName(CategoryIndex), ActionIndex),
                    CategoryIndex, ActionIndex
                );
            }

            if (!IsValidInputTag(ActionInfo.InputTag))
            {
                return FInputConfigValidationResult(
                    EInputConfigValidationError::InvalidInputTag,
                    FString::Printf(TEXT("%s 배열의 %d번 인덱스에서 유효하지 않은 InputTag: %s"),
                        GetCategoryName(CategoryIndex), ActionIndex, *ActionInfo.InputTag.ToString()),
                    CategoryIndex, ActionIndex
                );
            }
        }
    }

    return FInputConfigValidationResult();
}

FInputConfigValidationResult UInputConfig::ValidateInputTagUniqueness() const
{

    TMap<FGameplayTag, TPair<int32, int32>> UsedInputTags; //@Tag -> (Category, Index)

    //@Check all categories
    const TArray<FInputActionInfo>* InputArrays[] = {
        &NativeInputActions,    //@Category 0
        &AbilityInputActions,   //@Category 1
        &UIInputActions         //@Category 2
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 3; ++CategoryIndex)
    {
        const TArray<FInputActionInfo>& InputArray = *InputArrays[CategoryIndex];

        for (int32 ActionIndex = 0; ActionIndex < InputArray.Num(); ++ActionIndex)
        {
            const FInputActionInfo& ActionInfo = InputArray[ActionIndex];

            if (ActionInfo.InputTag.IsValid())
            {
                if (auto* ExistingLocation = UsedInputTags.Find(ActionInfo.InputTag))
                {
                    bool bSameCategory = (ExistingLocation->Key == CategoryIndex);

                    return FInputConfigValidationResult(
                        bSameCategory ? EInputConfigValidationError::DuplicateInputTagSameCategory
                        : EInputConfigValidationError::DuplicateInputTagCrossCategory,
                        FString::Printf(TEXT("중복된 InputTag '%s'이 %s[%d]와 %s[%d]에서 발견됨"),
                            *ActionInfo.InputTag.ToString(),
                            GetCategoryName(ExistingLocation->Key), ExistingLocation->Value,
                            GetCategoryName(CategoryIndex), ActionIndex),
                        CategoryIndex, ActionIndex
                    );
                }

                UsedInputTags.Add(ActionInfo.InputTag, TPair<int32, int32>(CategoryIndex, ActionIndex));
            }
        }
    }

    return FInputConfigValidationResult();
}

FInputConfigValidationResult UInputConfig::ValidateInputActionConsistency() const
{

    const TArray<FInputActionInfo>* InputArrays[] = {
        &NativeInputActions,
        &AbilityInputActions,
        &UIInputActions
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 3; ++CategoryIndex)
    {
        const TArray<FInputActionInfo>& InputArray = *InputArrays[CategoryIndex];

        //@Empty array warning (might be intentional)
        if (InputArray.Num() == 0)
        {
            UE_LOGFMT(LogInputConfig, Warning,
                "{0} 배열이 비어있습니다. 의도적일 수 있지만 확인해주세요.",
                GetCategoryName(CategoryIndex));
        }

        for (int32 ActionIndex = 0; ActionIndex < InputArray.Num(); ++ActionIndex)
        {
            const FInputActionInfo& ActionInfo = InputArray[ActionIndex];

            //@Valid tag but null action
            if (ActionInfo.InputTag.IsValid() && !IsValid(ActionInfo.InputAction))
            {
                return FInputConfigValidationResult(
                    EInputConfigValidationError::InputTagWithoutAction,
                    FString::Printf(TEXT("유효한 InputTag '%s'에 null InputAction이 설정됨: %s[%d]"),
                        *ActionInfo.InputTag.ToString(), GetCategoryName(CategoryIndex), ActionIndex),
                    CategoryIndex, ActionIndex
                );
            }

            //@Valid action but invalid tag
            if (IsValid(ActionInfo.InputAction) && !ActionInfo.InputTag.IsValid())
            {
                return FInputConfigValidationResult(
                    EInputConfigValidationError::InputActionWithoutTag,
                    FString::Printf(TEXT("유효한 InputAction에 유효하지 않은 InputTag가 설정됨: %s[%d]"),
                        GetCategoryName(CategoryIndex), ActionIndex),
                    CategoryIndex, ActionIndex
                );
            }
        }
    }

    return FInputConfigValidationResult();
}

void UInputConfig::ShowValidationError(const FInputConfigValidationResult& ValidationResult) const
{

    //@Structured log output for debugging
    UE_LOGFMT(LogInputConfig, Error,
        "InputConfig 무결성 검사 실패 | 오류 타입: {0} | 메시지: {1} | 카테고리: {2} | 인덱스: {3}",
        *UEnum::GetValueAsString(ValidationResult.ErrorType),
        *ValidationResult.ErrorMessage,
        ValidationResult.ProblemCategory,
        ValidationResult.ProblemIndex);

    //@ensureMsgf for debugger attention
    ensureMsgf(false, TEXT("InputConfig 무결성 검사 실패: %s"), *ValidationResult.ErrorMessage);

}

bool UInputConfig::TryAutoFixValidationError(const FInputConfigValidationResult& ValidationResult)
{

    switch (ValidationResult.ErrorType)
    {
    case EInputConfigValidationError::InvalidMappingPriority:
    {
        //@Negative priority to zero is safe
        if (MappingPriority < 0)
        {
            const_cast<UInputConfig*>(this)->MappingPriority = 0;
            UE_LOGFMT(LogInputConfig, Log,
                "자동 수정: 음수 MappingPriority를 0으로 변경했습니다");
            return true;
        }
        break;
    }

    case EInputConfigValidationError::InputActionWithoutTag:
    {
        UE_LOGFMT(LogInputConfig, Log,
            "InputActionWithoutTag에 대한 자동 수정을 사용할 수 없습니다 - 수동 검토가 필요합니다");
        return false;
    }

    case EInputConfigValidationError::InputTagWithoutAction:
    {
        UE_LOGFMT(LogInputConfig, Log,
            "InputTagWithoutAction에 대한 자동 수정을 사용할 수 없습니다 - 수동 할당이 필요합니다");
        return false;
    }

    default:
    {
        UE_LOGFMT(LogInputConfig, Log,
            "{0}에 대한 자동 수정을 사용할 수 없습니다 - 수동 교정이 필요합니다",
            *UEnum::GetValueAsString(ValidationResult.ErrorType));

        return false;
    }
    }

    return false;
}

bool UInputConfig::IsValidInputTag(const FGameplayTag& Tag) const
{

    if (!Tag.IsValid())
    {
        return false;
    }

    FString TagString = Tag.ToString();
    if (TagString.IsEmpty() || TagString == TEXT("None"))
    {
        return false;
    }

    if (!TagString.StartsWith(TEXT("Input.")))
    {
        UE_LOGFMT(LogInputConfig, VeryVerbose,
            "InputTag는 'Input.' 접두사로 시작해야 합니다: {0}", *TagString);
    }

    return true;
}

bool UInputConfig::IsValidIMCTag(const FGameplayTag& Tag) const
{
    if (!Tag.IsValid())
    {
        return false;
    }

    FString TagString = Tag.ToString();
    if (TagString.IsEmpty() || TagString == TEXT("None"))
    {
        return false;
    }

    if (!TagString.StartsWith(TEXT("Input.IMC")))
    {
        UE_LOGFMT(LogInputConfig, VeryVerbose,
            "IMCTag는 'Input.IMC' 접두사로 시작해야 합니다: {0}", *TagString);
    }

    return true;
}

const TCHAR* UInputConfig::GetCategoryName(int32 CategoryIndex) const
{
    switch (CategoryIndex)
    {
    case 0: return TEXT("Native");
    case 1: return TEXT("Ability");
    case 2: return TEXT("UI");
    default: return TEXT("Unknown");
    }
}

TArray<FInputActionInfo>* UInputConfig::GetInputActionArrayByCategory(int32 Category)
{
    switch (Category)
    {
    case 0: return &const_cast<UInputConfig*>(this)->NativeInputActions;
    case 1: return &const_cast<UInputConfig*>(this)->AbilityInputActions;
    case 2: return &const_cast<UInputConfig*>(this)->UIInputActions;
    default: return nullptr;
    }
}

void UInputConfig::ValidateInputConfigManually()
{
    FInputConfigValidationResult ValidationResult = ValidateInputConfig();

    if (ValidationResult.bIsValid)
    {
        UE_LOGFMT(LogInputConfig, Log, "✅ InputConfig 무결성 검사 성공: 모든 설정이 유효합니다");

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ InputConfig 무결성 검사 성공!"));
        }
    }
    else
    {
        ShowValidationError(ValidationResult);
    }
}

void UInputConfig::ValidateNativeActionsOnly()
{
    //@Validate only Native input actions
    bool bHasErrors = false;

    for (int32 i = 0; i < NativeInputActions.Num(); ++i)
    {
        const FInputActionInfo& ActionInfo = NativeInputActions[i];

        if (!IsValid(ActionInfo.InputAction))
        {
            UE_LOGFMT(LogInputConfig, Error, "Native 액션 인덱스 {0}에 null InputAction이 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidInputTag(ActionInfo.InputTag))
        {
            UE_LOGFMT(LogInputConfig, Error, "Native 액션 인덱스 {0}에 유효하지 않은 InputTag가 있습니다: {1}",
                i, *ActionInfo.InputTag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogInputConfig, Log, "✅ Native InputActions 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ Native Actions 무결성 검사 성공!"));
        }
    }
}

void UInputConfig::ValidateAbilityActionsOnly()
{
    //@Validate only Ability input actions
    bool bHasErrors = false;

    for (int32 i = 0; i < AbilityInputActions.Num(); ++i)
    {
        const FInputActionInfo& ActionInfo = AbilityInputActions[i];

        if (!IsValid(ActionInfo.InputAction))
        {
            UE_LOGFMT(LogInputConfig, Error, "Ability 액션 인덱스 {0}에 null InputAction이 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidInputTag(ActionInfo.InputTag))
        {
            UE_LOGFMT(LogInputConfig, Error, "Ability 액션 인덱스 {0}에 유효하지 않은 InputTag가 있습니다: {1}",
                i, *ActionInfo.InputTag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogInputConfig, Log, "✅ Ability InputActions 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ Ability Actions 무결성 검사 성공!"));
        }
    }
}

void UInputConfig::ValidateUIActionsOnly()
{
    //@Validate only UI input actions
    bool bHasErrors = false;

    for (int32 i = 0; i < UIInputActions.Num(); ++i)
    {
        const FInputActionInfo& ActionInfo = UIInputActions[i];

        if (!IsValid(ActionInfo.InputAction))
        {
            UE_LOGFMT(LogInputConfig, Error, "UI 액션 인덱스 {0}에 null InputAction이 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidInputTag(ActionInfo.InputTag))
        {
            UE_LOGFMT(LogInputConfig, Error, "UI 액션 인덱스 {0}에 유효하지 않은 InputTag가 있습니다: {1}",
                i, *ActionInfo.InputTag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogInputConfig, Log, "✅ UI InputActions 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ UI Actions 무결성 검사 성공!"));
        }
    }
}
#endif // WITH_EDITOR
#pragma endregion

#pragma region Utility
const UInputAction* UInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const auto& InputActionStruct : NativeInputActions)
    {
        if (InputActionStruct.InputAction && (InputActionStruct.InputTag == InputTag))
        {
            return InputActionStruct.InputAction;
        }
    }
    if (bLogNotFound)
    {
        UE_LOGFMT(LogInputConfig, Error, "InputConfig {1}에서 InputTag {0}에 대한 NativeInputAction을 찾을 수 없습니다.", *InputTag.ToString(), *GetNameSafe(this));
    }
    return nullptr;
}

const UInputAction* UInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const auto& InputActionStruct : AbilityInputActions)
    {
        if (InputActionStruct.InputAction && (InputActionStruct.InputTag == InputTag))
        {
            return InputActionStruct.InputAction;
        }
    }
    if (bLogNotFound)
    {
        UE_LOGFMT(LogInputConfig, Error, "InputConfig {1}에서 InputTag {0}에 대한 AbilityInputAction을 찾을 수 없습니다.", *InputTag.ToString(), *GetNameSafe(this));
    }
    return nullptr;
}

const UInputAction* UInputConfig::FindUIInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const auto& InputActionStruct : UIInputActions)
    {
        if (InputActionStruct.InputAction && (InputActionStruct.InputTag == InputTag))
        {
            return InputActionStruct.InputAction;
        }
    }
    if (bLogNotFound)
    {
        UE_LOGFMT(LogInputConfig, Error, "InputConfig {1}에서 InputTag {0}에 대한 UIInputAction을 찾을 수 없습니다.", *InputTag.ToString(), *GetNameSafe(this));
    }
    return nullptr;
}
#pragma endregion