#include "08_UI/UICollection.h"
#include "Logging/StructuredLog.h"

#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY(LogUICollection)

#pragma region Default SEtting
UUICollection::UUICollection(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    HUDUIInformations.Empty();
    MenuUIInformations.Empty();
    InteractionUIInformations.Empty();
    SystemUIInformations.Empty();
}

#if WITH_EDITOR
void UUICollection::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FUICollectionValidationResult ValidationResult = ValidateUICollection();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);

        //@Try automatic fix if possible
        if (!TryAutoFixValidationError(ValidationResult))
        {
            UE_LOGFMT(LogUICollection, Warning,
                "UICollection 무결성 검사 실패 - 수동 수정 필요: {0}",
                *ValidationResult.ErrorMessage);
        }
    }
}

void UUICollection::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    FUICollectionValidationResult ValidationResult = ValidateUICollection();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);
        TryAutoFixValidationError(ValidationResult);
    }
}

FUICollectionValidationResult UUICollection::ValidateUICollection() const
{

    //@Step 1: Basic UI arrays validation
    FUICollectionValidationResult Result = ValidateUIArrays();
    if (!Result.bIsValid) return Result;

    //@Step 2: UI tag uniqueness check
    Result = ValidateUITagUniqueness();
    if (!Result.bIsValid) return Result;

    //@Step 3: Input tag uniqueness check
    Result = ValidateInputTagUniqueness();
    if (!Result.bIsValid) return Result;

    //@Step 4: UI logical consistency check
    Result = ValidateUILogicalConsistency();
    if (!Result.bIsValid) return Result;

    //@Step 5: Category consistency check
    Result = ValidateCategoryConsistency();
    if (!Result.bIsValid) return Result;

    //@Step 6: BeginPlay UI limits check
    Result = ValidateBeginPlayUILimits();
    if (!Result.bIsValid) return Result;

    //@All validations passed
    return FUICollectionValidationResult();
}

FUICollectionValidationResult UUICollection::ValidateUIArrays() const
{

    //@Check each category of UI informations
    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,         //@Category 0 - HUD
        &MenuUIInformations,        //@Category 1 - Menu
        &InteractionUIInformations, //@Category 2 - Interaction
        &SystemUIInformations       //@Category 3 - System
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];

        //@Check each UI information in the array
        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            //@Null UI class check
            if (!IsValid(UIInfo.UIClass))
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::NullUIClass,
                    FString::Printf(TEXT("%s 배열의 %d번 인덱스에서 Null UI 클래스 발견"),
                        GetCategoryName(CategoryIndex), UIIndex),
                    CategoryIndex, UIIndex
                );
            }

            //@Invalid UI tag check
            if (!IsValidUITag(UIInfo.UITag))
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::InvalidUITag,
                    FString::Printf(TEXT("%s 배열의 %d번 인덱스에서 유효하지 않은 UI 태그: %s"),
                        GetCategoryName(CategoryIndex), UIIndex, *UIInfo.UITag.ToString()),
                    CategoryIndex, UIIndex
                );
            }

            if (UIInfo.bInputBinded)
            {
                for (int32 InputTagIndex = 0; InputTagIndex < UIInfo.InputTags.Num(); ++InputTagIndex)
                {
                    const FGameplayTag& InputTag = UIInfo.InputTags[InputTagIndex];

                    if (!IsValidInputTag(InputTag))
                    {
                        return FUICollectionValidationResult(
                            EUICollectionValidationError::InvalidInputTag,
                            FString::Printf(TEXT("%s 배열의 %d번 UI에서 유효하지 않은 입력 태그: %s"),
                                GetCategoryName(CategoryIndex), UIIndex, *InputTag.ToString()),
                            CategoryIndex, UIIndex, InputTagIndex
                        );
                    }
                }
            }
        }
    }

    return FUICollectionValidationResult();
}

FUICollectionValidationResult UUICollection::ValidateUITagUniqueness() const
{

    TMap<FGameplayTag, TPair<int32, int32>> UsedUITags; //@Tag -> (Category, Index)

    //@Check all categories
    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,         //@Category 0
        &MenuUIInformations,        //@Category 1
        &InteractionUIInformations, //@Category 2
        &SystemUIInformations       //@Category 3
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];

        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            if (UIInfo.UITag.IsValid())
            {
                if (auto* ExistingLocation = UsedUITags.Find(UIInfo.UITag))
                {
                    //@Determine if it's same category or cross-category duplicate
                    bool bSameCategory = (ExistingLocation->Key == CategoryIndex);

                    return FUICollectionValidationResult(
                        bSameCategory ? EUICollectionValidationError::DuplicateUITagSameCategory
                        : EUICollectionValidationError::DuplicateUITagCrossCategory,
                        FString::Printf(TEXT("중복된 UI 태그 '%s'이 %s[%d]와 %s[%d]에서 발견됨"),
                            *UIInfo.UITag.ToString(),
                            GetCategoryName(ExistingLocation->Key), ExistingLocation->Value,
                            GetCategoryName(CategoryIndex), UIIndex),
                        CategoryIndex, UIIndex
                    );
                }

                UsedUITags.Add(UIInfo.UITag, TPair<int32, int32>(CategoryIndex, UIIndex));
            }
        }
    }

    return FUICollectionValidationResult();
}

FUICollectionValidationResult UUICollection::ValidateInputTagUniqueness() const
{

    TMap<FGameplayTag, TPair<TPair<int32, int32>, int32>> UsedInputTags; //@Tag -> ((Category, UIIndex), InputTagIndex)

    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,
        &MenuUIInformations,
        &InteractionUIInformations,
        &SystemUIInformations
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];

        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            if (UIInfo.bInputBinded)
            {
                for (int32 InputTagIndex = 0; InputTagIndex < UIInfo.InputTags.Num(); ++InputTagIndex)
                {
                    const FGameplayTag& InputTag = UIInfo.InputTags[InputTagIndex];

                    if (InputTag.IsValid())
                    {
                        if (auto* ExistingLocation = UsedInputTags.Find(InputTag))
                        {
                            const auto& ExistingCategoryIndex = ExistingLocation->Key.Key;
                            const auto& ExistingUIIndex = ExistingLocation->Key.Value;
                            const auto& ExistingInputTagIndex = ExistingLocation->Value;

                            return FUICollectionValidationResult(
                                EUICollectionValidationError::DuplicateInputTag,
                                FString::Printf(TEXT("중복된 입력 태그 '%s'이 %s[%d]와 %s[%d]에서 발견됨"),
                                    *InputTag.ToString(),
                                    GetCategoryName(ExistingCategoryIndex), ExistingUIIndex,
                                    GetCategoryName(CategoryIndex), UIIndex),
                                CategoryIndex, UIIndex, InputTagIndex
                            );
                        }

                        UsedInputTags.Add(InputTag, TPair<TPair<int32, int32>, int32>(
                            TPair<int32, int32>(CategoryIndex, UIIndex), InputTagIndex));
                    }
                }
            }
        }
    }

    return FUICollectionValidationResult();
}

FUICollectionValidationResult UUICollection::ValidateUILogicalConsistency() const
{
    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,
        &MenuUIInformations,
        &InteractionUIInformations,
        &SystemUIInformations
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];

        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            //@Input binding enabled but no input tags
            if (UIInfo.bInputBinded && UIInfo.InputTags.Num() == 0)
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::InputBindingInconsistency,
                    FString::Printf(TEXT("입력 바인딩이 활성화되어 있지만 입력 태그가 없습니다: %s[%d]"),
                        GetCategoryName(CategoryIndex), UIIndex),
                    CategoryIndex, UIIndex
                );
            }

            //@Input binding disabled but has input tags
            if (!UIInfo.bInputBinded && UIInfo.InputTags.Num() > 0)
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::InputBindingInconsistency,
                    FString::Printf(TEXT("입력 바인딩이 비활성화되어 있지만 입력 태그가 설정되어 있습니다: %s[%d]"),
                        GetCategoryName(CategoryIndex), UIIndex),
                    CategoryIndex, UIIndex
                );
            }

            //@Warning for BeginPlay UI with input binding (potential double activation)
            if (UIInfo.bShownOnBeginPlay && UIInfo.bInputBinded)
            {
                UE_LOGFMT(LogUICollection, Warning,
                    "{0}[{1}] UI가 BeginPlay에 표시되면서 입력 바인딩도 활성화되어 있습니다. 중복 활성화가 발생할 수 있습니다.",
                    GetCategoryName(CategoryIndex), UIIndex);
            }
        }
    }

    return FUICollectionValidationResult();
}

FUICollectionValidationResult UUICollection::ValidateCategoryConsistency() const
{
    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,         //@Should be EUICategory::HUD
        &MenuUIInformations,        //@Should be EUICategory::Menu
        &InteractionUIInformations, //@Should be EUICategory::Interaction
        &SystemUIInformations       //@Should be EUICategory::System
    };

    const EUICategory ExpectedCategories[] = {
        EUICategory::HUD,
        EUICategory::Menu,
        EUICategory::Interaction,
        EUICategory::System
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];
        const EUICategory ExpectedCategory = ExpectedCategories[CategoryIndex];

        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            //@Check if UI category matches the array it's stored in
            if (UIInfo.UICategory != ExpectedCategory && UIInfo.UICategory != EUICategory::MAX)
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::CategoryMismatch,
                    FString::Printf(TEXT("카테고리 불일치: %s 배열의 UI가 %s 카테고리로 설정되어 있습니다 (인덱스 %d)"),
                        GetCategoryName(CategoryIndex),
                        GetCategoryName(UIInfo.UICategory),
                        UIIndex),
                    CategoryIndex, UIIndex
                );
            }

            //@Check for invalid category (MAX)
            if (UIInfo.UICategory == EUICategory::MAX)
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::InvalidUICategory,
                    FString::Printf(TEXT("유효하지 않은 UI 카테고리 (MAX): %s[%d]"),
                        GetCategoryName(CategoryIndex), UIIndex),
                    CategoryIndex, UIIndex
                );
            }
        }
    }

    return FUICollectionValidationResult();
}

FUICollectionValidationResult UUICollection::ValidateBeginPlayUILimits() const
{
    int32 BeginPlayUICount = 0;
    const int32 RecommendedMaxBeginPlayUIs = 3; //@Configurable limit

    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,
        &MenuUIInformations,
        &InteractionUIInformations,
        &SystemUIInformations
    };

    //@Count BeginPlay UIs across all categories
    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];

        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            if (UIInfo.bShownOnBeginPlay)
            {
                BeginPlayUICount++;
            }
        }
    }

    if (BeginPlayUICount > RecommendedMaxBeginPlayUIs)
    {
        return FUICollectionValidationResult(
            EUICollectionValidationError::TooManyBeginPlayUIs,
            FString::Printf(TEXT("BeginPlay에 표시되는 UI가 너무 많습니다 (%d개, 권장: %d개 이하). 플레이어 경험에 영향을 미칠 수 있습니다."),
                BeginPlayUICount, RecommendedMaxBeginPlayUIs)
        );
    }

    //@Log info about BeginPlay UI count
    if (BeginPlayUICount > 0)
    {
        UE_LOGFMT(LogUICollection, Log,
            "BeginPlay에 표시될 UI 개수: {0}개", BeginPlayUICount);
    }

    return FUICollectionValidationResult();
}

void UUICollection::ShowValidationError(const FUICollectionValidationResult& ValidationResult) const
{

    //@Structured log output for debugging
    UE_LOGFMT(LogUICollection, Error,
        "UICollection 무결성 검사 실패 | 오류 타입: {0} | 메시지: {1} | 카테고리: {2} | 인덱스: {3} | 입력태그 인덱스: {4}",
        *UEnum::GetValueAsString(ValidationResult.ErrorType),
        *ValidationResult.ErrorMessage,
        ValidationResult.ProblemCategory,
        ValidationResult.ProblemIndex,
        ValidationResult.ProblemInputTagIndex);

    //@ensureMsgf for debugger attention
    ensureMsgf(false, TEXT("UICollection 무결성 검사 실패: %s"), *ValidationResult.ErrorMessage);
}

bool UUICollection::TryAutoFixValidationError(const FUICollectionValidationResult& ValidationResult)
{

    switch (ValidationResult.ErrorType)
    {
    case EUICollectionValidationError::InputBindingInconsistency:
    {
        //@Input binding enabled but no input tags - disable input binding
        if (ValidationResult.ProblemCategory >= 0 && ValidationResult.ProblemIndex >= 0)
        {
            TArray<FUIInformation>* UIArray = GetUIArrayByCategory(ValidationResult.ProblemCategory);
            if (UIArray && ValidationResult.ProblemIndex < UIArray->Num())
            {
                FUIInformation& UIInfo = (*UIArray)[ValidationResult.ProblemIndex];

                if (UIInfo.bInputBinded && UIInfo.InputTags.Num() == 0)
                {
                    UIInfo.bInputBinded = false;
                    UE_LOGFMT(LogUICollection, Log,
                        "자동 수정: {0}[{1}] UI의 입력 바인딩을 비활성화했습니다 (입력 태그가 없음)",
                        GetCategoryName(ValidationResult.ProblemCategory), ValidationResult.ProblemIndex);
                    return true;
                }
            }
        }
        break;
    }

    case EUICollectionValidationError::CategoryMismatch:
    {
        //@Fix category mismatch by updating UI category to match its array
        if (ValidationResult.ProblemCategory >= 0 && ValidationResult.ProblemIndex >= 0)
        {
            TArray<FUIInformation>* UIArray = GetUIArrayByCategory(ValidationResult.ProblemCategory);
            if (UIArray && ValidationResult.ProblemIndex < UIArray->Num())
            {
                FUIInformation& UIInfo = (*UIArray)[ValidationResult.ProblemIndex];
                EUICategory OldCategory = UIInfo.UICategory;

                //@Set correct category based on array position
                switch (ValidationResult.ProblemCategory)
                {
                case 0: UIInfo.UICategory = EUICategory::HUD; break;
                case 1: UIInfo.UICategory = EUICategory::Menu; break;
                case 2: UIInfo.UICategory = EUICategory::Interaction; break;
                case 3: UIInfo.UICategory = EUICategory::System; break;
                }

                UE_LOGFMT(LogUICollection, Log,
                    "자동 수정: {0}[{1}] UI의 카테고리를 {2}에서 {3}으로 변경했습니다",
                    GetCategoryName(ValidationResult.ProblemCategory), ValidationResult.ProblemIndex,
                    GetCategoryName(OldCategory), GetCategoryName(UIInfo.UICategory));
                return true;
            }
        }
        break;
    }

    case EUICollectionValidationError::InvalidUICategory:
    {
        //@Fix invalid category (MAX) by setting correct category
        if (ValidationResult.ProblemCategory >= 0 && ValidationResult.ProblemIndex >= 0)
        {
            TArray<FUIInformation>* UIArray = GetUIArrayByCategory(ValidationResult.ProblemCategory);
            if (UIArray && ValidationResult.ProblemIndex < UIArray->Num())
            {
                FUIInformation& UIInfo = (*UIArray)[ValidationResult.ProblemIndex];

                if (UIInfo.UICategory == EUICategory::MAX)
                {
                    //@Set correct category based on array position
                    switch (ValidationResult.ProblemCategory)
                    {
                    case 0: UIInfo.UICategory = EUICategory::HUD; break;
                    case 1: UIInfo.UICategory = EUICategory::Menu; break;
                    case 2: UIInfo.UICategory = EUICategory::Interaction; break;
                    case 3: UIInfo.UICategory = EUICategory::System; break;
                    }

                    UE_LOGFMT(LogUICollection, Log,
                        "자동 수정: {0}[{1}] UI의 유효하지 않은 카테고리를 {2}로 설정했습니다",
                        GetCategoryName(ValidationResult.ProblemCategory), ValidationResult.ProblemIndex,
                        GetCategoryName(UIInfo.UICategory));
                    return true;
                }
            }
        }
        break;
    }

    default:
    {
        UE_LOGFMT(LogUICollection, Log,
            "{0}에 대한 자동 수정을 사용할 수 없습니다 - 수동 교정이 필요합니다",
            *UEnum::GetValueAsString(ValidationResult.ErrorType));

        return false;
    }
    }

    return false;
}

bool UUICollection::IsValidUITag(const FGameplayTag& Tag) const
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

    if (!TagString.StartsWith(TEXT("UI.")))
    {
        UE_LOGFMT(LogUICollection, VeryVerbose,
            "UI 태그는 'UI.' 접두사로 시작해야 합니다: {0}", *TagString);
    }

    return true;
}

bool UUICollection::IsValidInputTag(const FGameplayTag& Tag) const
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

    //@Input tags should start with "Input." prefix
    if (!TagString.StartsWith(TEXT("Input.")))
    {
        UE_LOGFMT(LogUICollection, VeryVerbose,
            "입력 태그는 'Input.' 접두사로 시작해야 합니다: {0}", *TagString);
    }

    return true;
}

const TCHAR* UUICollection::GetCategoryName(int32 CategoryIndex) const
{
    //@Category index to name mapping
    switch (CategoryIndex)
    {
    case 0: return TEXT("HUD");
    case 1: return TEXT("Menu");
    case 2: return TEXT("Interaction");
    case 3: return TEXT("System");
    default: return TEXT("Unknown");
    }
}

const TCHAR* UUICollection::GetCategoryName(EUICategory Category) const
{
    //@Category enum to name mapping
    switch (Category)
    {
    case EUICategory::HUD: return TEXT("HUD");
    case EUICategory::Menu: return TEXT("Menu");
    case EUICategory::Interaction: return TEXT("Interaction");
    case EUICategory::System: return TEXT("System");
    case EUICategory::MAX: return TEXT("MAX");
    default: return TEXT("Unknown");
    }
}

TArray<FUIInformation>* UUICollection::GetUIArrayByCategory(int32 Category)
{
    //@Helper function to get mutable array reference
    switch (Category)
    {
    case 0: return &HUDUIInformations;
    case 1: return &MenuUIInformations;
    case 2: return &InteractionUIInformations;
    case 3: return &SystemUIInformations;
    default: return nullptr;
    }
}

const TArray<FUIInformation>* UUICollection::GetUIArrayByCategory(int32 Category) const
{
    //@Helper function to get const array reference
    switch (Category)
    {
    case 0: return &HUDUIInformations;
    case 1: return &MenuUIInformations;
    case 2: return &InteractionUIInformations;
    case 3: return &SystemUIInformations;
    default: return nullptr;
    }
}

void UUICollection::ValidateUICollectionManually()
{
    FUICollectionValidationResult ValidationResult = ValidateUICollection();

    if (ValidationResult.bIsValid)
    {
        UE_LOGFMT(LogUICollection, Log, "✅ UICollection 무결성 검사 성공: 모든 설정이 유효합니다");

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ UICollection 무결성 검사 성공!"));
        }
    }
    else
    {
        ShowValidationError(ValidationResult);
    }
}

void UUICollection::ValidateHUDUIOnly()
{
    //@Validate only HUD UI informations
    bool bHasErrors = false;

    for (int32 i = 0; i < HUDUIInformations.Num(); ++i)
    {
        const FUIInformation& UIInfo = HUDUIInformations[i];

        if (!IsValid(UIInfo.UIClass))
        {
            UE_LOGFMT(LogUICollection, Error, "HUD UI 인덱스 {0}에 null UI 클래스가 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidUITag(UIInfo.UITag))
        {
            UE_LOGFMT(LogUICollection, Error, "HUD UI 인덱스 {0}에 유효하지 않은 UI 태그가 있습니다: {1}",
                i, *UIInfo.UITag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogUICollection, Log, "✅ HUD UI 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ HUD UI 무결성 검사 성공!"));
        }
    }
}

void UUICollection::ValidateMenuUIOnly()
{
    //@Validate only Menu UI informations
    bool bHasErrors = false;

    for (int32 i = 0; i < MenuUIInformations.Num(); ++i)
    {
        const FUIInformation& UIInfo = MenuUIInformations[i];

        if (!IsValid(UIInfo.UIClass))
        {
            UE_LOGFMT(LogUICollection, Error, "Menu UI 인덱스 {0}에 null UI 클래스가 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidUITag(UIInfo.UITag))
        {
            UE_LOGFMT(LogUICollection, Error, "Menu UI 인덱스 {0}에 유효하지 않은 UI 태그가 있습니다: {1}",
                i, *UIInfo.UITag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogUICollection, Log, "✅ Menu UI 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ Menu UI 무결성 검사 성공!"));
        }
    }
}

void UUICollection::ValidateInteractionUIOnly()
{
    //@Validate only Interaction UI informations
    bool bHasErrors = false;

    for (int32 i = 0; i < InteractionUIInformations.Num(); ++i)
    {
        const FUIInformation& UIInfo = InteractionUIInformations[i];

        if (!IsValid(UIInfo.UIClass))
        {
            UE_LOGFMT(LogUICollection, Error, "Interaction UI 인덱스 {0}에 null UI 클래스가 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidUITag(UIInfo.UITag))
        {
            UE_LOGFMT(LogUICollection, Error, "Interaction UI 인덱스 {0}에 유효하지 않은 UI 태그가 있습니다: {1}",
                i, *UIInfo.UITag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogUICollection, Log, "✅ Interaction UI 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ Interaction UI 무결성 검사 성공!"));
        }
    }
}

void UUICollection::ValidateSystemUIOnly()
{
    //@Validate only System UI informations
    bool bHasErrors = false;

    for (int32 i = 0; i < SystemUIInformations.Num(); ++i)
    {
        const FUIInformation& UIInfo = SystemUIInformations[i];

        if (!IsValid(UIInfo.UIClass))
        {
            UE_LOGFMT(LogUICollection, Error, "System UI 인덱스 {0}에 null UI 클래스가 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidUITag(UIInfo.UITag))
        {
            UE_LOGFMT(LogUICollection, Error, "System UI 인덱스 {0}에 유효하지 않은 UI 태그가 있습니다: {1}",
                i, *UIInfo.UITag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogUICollection, Log, "✅ System UI 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ System UI 무결성 검사 성공!"));
        }
    }
}
#endif // WITH_EDITOR
#pragma endregion

#pragma region Utility
const TArray<FUIInformation>& UUICollection::GetUICategoryInformations(const EUICategory& UICategory) const
{
    switch (UICategory)
    {
    case EUICategory::HUD:
        return HUDUIInformations;
    case EUICategory::Menu:
        return MenuUIInformations;
    case EUICategory::Interaction:
        return InteractionUIInformations;
    case EUICategory::System:
        return SystemUIInformations;
    default:
        UE_LOGFMT(LogUICollection, Warning, "유효하지 않은 UI Category 입니다!");
        static const TArray<FUIInformation> EmptyArray;
        return EmptyArray;
    }
}
#pragma endregion