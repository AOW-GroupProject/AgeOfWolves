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
    IndicatorUIInformations.Empty(); // 새로 추가
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

    //@Step 7: 새로 추가된 최소 표시 시간 검증
    Result = ValidateMinimumDisplayTime();
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
        &SystemUIInformations,      //@Category 3 - System
        &IndicatorUIInformations    //@Category 4 - Indicator (새로 추가)
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 5; ++CategoryIndex)
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
        &SystemUIInformations,       //@Category 3
        &IndicatorUIInformations
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 5; ++CategoryIndex)
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
        &SystemUIInformations,
        &IndicatorUIInformations    // 새로 추가
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 5; ++CategoryIndex)
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
        &SystemUIInformations,
        &IndicatorUIInformations
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 5; ++CategoryIndex)
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
        &HUDUIInformations,
        &MenuUIInformations,
        &InteractionUIInformations,
        &SystemUIInformations,
        &IndicatorUIInformations  // 새로 추가
    };

    const EUICategory ExpectedCategories[] = {
        EUICategory::HUD,
        EUICategory::Menu,
        EUICategory::Interaction,
        EUICategory::System,
        EUICategory::Indicator  // 새로 추가
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 5; ++CategoryIndex)
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

FUICollectionValidationResult UUICollection::ValidateMinimumDisplayTime() const
{
    const TArray<FUIInformation>* UIArrays[] = {
        &HUDUIInformations,         //@Category 0 - HUD
        &MenuUIInformations,        //@Category 1 - Menu  
        &InteractionUIInformations, //@Category 2 - Interaction
        &SystemUIInformations       //@Category 3 - System
    };

    for (int32 CategoryIndex = 0; CategoryIndex < 4; ++CategoryIndex)
    {
        const TArray<FUIInformation>& UIArray = *UIArrays[CategoryIndex];

        for (int32 UIIndex = 0; UIIndex < UIArray.Num(); ++UIIndex)
        {
            const FUIInformation& UIInfo = UIArray[UIIndex];

            //@System UI가 아닌데 최소 표시 시간을 사용하려고 하는 경우
            if (UIInfo.bUseMinimumDisplayTime && UIInfo.UICategory != EUICategory::System)
            {
                return FUICollectionValidationResult(
                    EUICollectionValidationError::MinimumDisplayTimeOnNonSystemUI,
                    FString::Printf(TEXT("%s UI에서 최소 표시 시간을 사용하려고 했습니다. 최소 표시 시간은 System UI에서만 사용할 수 있습니다 (인덱스: %d)"),
                        GetCategoryName(CategoryIndex), UIIndex),
                    CategoryIndex, UIIndex
                );
            }

            //@최소 표시 시간이 활성화되어 있으면서 유효 범위를 벗어나는 경우
            if (UIInfo.bUseMinimumDisplayTime)
            {
                if (UIInfo.MinimumDisplayTimeSeconds < 2 || UIInfo.MinimumDisplayTimeSeconds >= 10)
                {
                    return FUICollectionValidationResult(
                        EUICollectionValidationError::InvalidMinimumDisplayTime,
                        FString::Printf(TEXT("%s[%d] UI의 최소 표시 시간이 유효 범위(2-9초)를 벗어났습니다: %d초"),
                            GetCategoryName(CategoryIndex), UIIndex, UIInfo.MinimumDisplayTimeSeconds),
                        CategoryIndex, UIIndex
                    );
                }

                UE_LOGFMT(LogUICollection, Log,
                    "{0}[{1}] UI에 최소 표시 시간 {2}초가 설정되었습니다",
                    GetCategoryName(CategoryIndex), UIIndex, UIInfo.MinimumDisplayTimeSeconds);
            }
        }
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
    case EUICollectionValidationError::MinimumDisplayTimeOnNonSystemUI:
    {
        //@System UI가 아닌데 최소 표시 시간을 사용하려는 경우 - 비활성화
        if (ValidationResult.ProblemCategory >= 0 && ValidationResult.ProblemIndex >= 0)
        {
            TArray<FUIInformation>* UIArray = GetUIArrayByCategory(ValidationResult.ProblemCategory);
            if (UIArray && ValidationResult.ProblemIndex < UIArray->Num())
            {
                FUIInformation& UIInfo = (*UIArray)[ValidationResult.ProblemIndex];

                if (UIInfo.bUseMinimumDisplayTime && UIInfo.UICategory != EUICategory::System)
                {
                    UIInfo.bUseMinimumDisplayTime = false;
                    UE_LOGFMT(LogUICollection, Log,
                        "자동 수정: {0}[{1}] UI의 최소 표시 시간을 비활성화했습니다 (System UI가 아님)",
                        GetCategoryName(ValidationResult.ProblemCategory), ValidationResult.ProblemIndex);
                    return true;
                }
            }
        }
        break;
    }

    case EUICollectionValidationError::InvalidMinimumDisplayTime:
    {
        //@최소 표시 시간이 유효 범위를 벗어나는 경우 - 기본값으로 설정
        if (ValidationResult.ProblemCategory >= 0 && ValidationResult.ProblemIndex >= 0)
        {
            TArray<FUIInformation>* UIArray = GetUIArrayByCategory(ValidationResult.ProblemCategory);
            if (UIArray && ValidationResult.ProblemIndex < UIArray->Num())
            {
                FUIInformation& UIInfo = (*UIArray)[ValidationResult.ProblemIndex];

                if (UIInfo.bUseMinimumDisplayTime)
                {
                    int32 OldValue = UIInfo.MinimumDisplayTimeSeconds;
                    UIInfo.MinimumDisplayTimeSeconds = 5; //@기본값으로 설정

                    UE_LOGFMT(LogUICollection, Log,
                        "자동 수정: {0}[{1}] UI의 최소 표시 시간을 {2}초에서 {3}초로 변경했습니다",
                        GetCategoryName(ValidationResult.ProblemCategory), ValidationResult.ProblemIndex,
                        OldValue, UIInfo.MinimumDisplayTimeSeconds);
                    return true;
                }
            }
        }
        break;
    }

    // ... 기존 자동 수정 로직들은 그대로 유지 ...

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
    switch (CategoryIndex)
    {
    case 0: return TEXT("HUD");
    case 1: return TEXT("Menu");
    case 2: return TEXT("Interaction");
    case 3: return TEXT("System");
    case 4: return TEXT("Indicator");  // 새로 추가
    default: return TEXT("Unknown");
    }
}

const TCHAR* UUICollection::GetCategoryName(EUICategory Category) const
{
    switch (Category)
    {
    case EUICategory::HUD: return TEXT("HUD");
    case EUICategory::Menu: return TEXT("Menu");
    case EUICategory::Interaction: return TEXT("Interaction");
    case EUICategory::System: return TEXT("System");
    case EUICategory::Indicator: return TEXT("Indicator");  // 새로 추가
    case EUICategory::MAX: return TEXT("MAX");
    default: return TEXT("Unknown");
    }
}

TArray<FUIInformation>* UUICollection::GetUIArrayByCategory(int32 Category)
{
    switch (Category)
    {
    case 0: return &HUDUIInformations;
    case 1: return &MenuUIInformations;
    case 2: return &InteractionUIInformations;
    case 3: return &SystemUIInformations;
    case 4: return &IndicatorUIInformations;  // 새로 추가
    default: return nullptr;
    }
}

const TArray<FUIInformation>* UUICollection::GetUIArrayByCategory(int32 Category) const
{
    switch (Category)
    {
    case 0: return &HUDUIInformations;
    case 1: return &MenuUIInformations;
    case 2: return &InteractionUIInformations;
    case 3: return &SystemUIInformations;
    case 4: return &IndicatorUIInformations;  // 새로 추가
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

void UUICollection::ValidateIndicatorUIOnly()
{
    bool bHasErrors = false;

    for (int32 i = 0; i < IndicatorUIInformations.Num(); ++i)
    {
        const FUIInformation& UIInfo = IndicatorUIInformations[i];

        if (!IsValid(UIInfo.UIClass))
        {
            UE_LOGFMT(LogUICollection, Error, "Indicator UI 인덱스 {0}에 null UI 클래스가 있습니다", i);
            bHasErrors = true;
        }

        if (!IsValidUITag(UIInfo.UITag))
        {
            UE_LOGFMT(LogUICollection, Error, "Indicator UI 인덱스 {0}에 유효하지 않은 UI 태그가 있습니다: {1}",
                i, *UIInfo.UITag.ToString());
            bHasErrors = true;
        }
    }

    if (!bHasErrors)
    {
        UE_LOGFMT(LogUICollection, Log, "✅ Indicator UI 무결성 검사 성공");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ Indicator UI 무결성 검사 성공!"));
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
    case EUICategory::Indicator:  // 새로 추가
        return IndicatorUIInformations;
    default:
        UE_LOGFMT(LogUICollection, Warning, "유효하지 않은 UI Category 입니다!");
        static const TArray<FUIInformation> EmptyArray;
        return EmptyArray;
    }
}
#pragma endregion