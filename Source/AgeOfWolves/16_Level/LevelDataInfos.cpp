#include "LevelDataInfos.h"
#include "Logging/StructuredLog.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDeviceRedirector.h"

DEFINE_LOG_CATEGORY(LogLevelDataInfos)

//@Default Setting
#pragma region Default Setting
ULevelDataInfos::ULevelDataInfos(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

#if WITH_EDITOR
void ULevelDataInfos::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    //@프로퍼티 변경 시 즉시 무결성 검사
    FLevelValidationResult ValidationResult = ValidateLevelData();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);

        //@자동 수정 시도
        if (!TryAutoFixValidationError(ValidationResult))
        {
            UE_LOGFMT(LogLevelDataInfos, Warning,
                "레벨 데이터 무결성 검사 실패 - 수동 수정 필요: {0}",
                *ValidationResult.ErrorMessage);
        }
    }
    else
    {
        UE_LOGFMT(LogLevelDataInfos, Log, "레벨 데이터 무결성 검사 통과");
    }
}

void ULevelDataInfos::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    //@배열이나 구조체 내부 변경 시에도 검증
    FLevelValidationResult ValidationResult = ValidateLevelData();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);
        TryAutoFixValidationError(ValidationResult);
    }
}

FLevelValidationResult ULevelDataInfos::ValidateLevelData() const
{
    //@1단계: 레벨 타입 검증
    FLevelValidationResult Result = ValidateLevelTypes();
    if (!Result.bIsValid) return Result;

    //@2단계: 기본 설정 검증
    Result = ValidateBasicSettings();
    if (!Result.bIsValid) return Result;

    //@3단계: 레벨 태그 검증 (기존 ID 검증 대체)
    Result = ValidateLevelTags();
    if (!Result.bIsValid) return Result;

    //@4단계: 레벨 이름 검증
    Result = ValidateLevelNames();
    if (!Result.bIsValid) return Result;

    //@5단계: 레벨 에셋 검증
    Result = ValidateLevelAssets();
    if (!Result.bIsValid) return Result;

    //@6단계: 논리적 일관성 검증
    Result = ValidateLogicalConsistency();
    if (!Result.bIsValid) return Result;

    //@7단계: PlayerStart 태그 검증 - 추후에 진행
    //Result = ValidatePlayerStartTags();
    //if (!Result.bIsValid) return Result;

    return FLevelValidationResult(); //@모든 검증 통과
}

FLevelValidationResult ULevelDataInfos::ValidateBasicSettings() const
{
    //@레벨 데이터 목록이 비어있는지 확인
    if (LevelDataList.Num() == 0)
    {
        return FLevelValidationResult(
            ELevelValidationError::EmptyLevelData,
            TEXT("레벨 데이터가 비어있습니다. 최소한 하나의 레벨은 필요합니다.")
        );
    }

    //@너무 많은 레벨이 있는지 확인 (성능 고려)
    if (LevelDataList.Num() > 100)
    {
        UE_LOGFMT(LogLevelDataInfos, Warning,
            "레벨 데이터가 매우 많습니다 ({0}개). 성능에 영향을 줄 수 있습니다.", LevelDataList.Num());
    }

    return FLevelValidationResult();
}

FLevelValidationResult ULevelDataInfos::ValidateLevelTags() const
{
    TSet<FGameplayTag> UsedLevelTags;

    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@유효하지 않은 태그 검사
        if (!LevelData.LevelTag.IsValid())
        {
            return FLevelValidationResult(
                ELevelValidationError::InvalidLevelTag,
                FString::Printf(TEXT("유효하지 않은 레벨 태그입니다 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }

        //@중복 태그 검사
        if (UsedLevelTags.Contains(LevelData.LevelTag))
        {
            return FLevelValidationResult(
                ELevelValidationError::DuplicateLevelTag,
                FString::Printf(TEXT("중복된 레벨 태그 발견: %s (인덱스: %d)"),
                    *LevelData.LevelTag.ToString(), LevelIndex),
                LevelIndex
            );
        }

        //@태그와 카테고리 일치성 검사
        FString ExpectedPrefix;
        switch (LevelData.LevelCategory)
        {
        case ELevelType::World:
            ExpectedPrefix = TEXT("Level.World");
            break;
        case ELevelType::BossArena:
            ExpectedPrefix = TEXT("Level.BossArena");
            break;
        case ELevelType::Title:
            ExpectedPrefix = TEXT("Level.Title");
            break;
        case ELevelType::Test:
            ExpectedPrefix = TEXT("Level.Test");
            break;
        default:
            return FLevelValidationResult(
                ELevelValidationError::InvalidLevelType,
                FString::Printf(TEXT("지원하지 않는 레벨 타입입니다 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }

        FString TagString = LevelData.LevelTag.ToString();
        if (!TagString.StartsWith(ExpectedPrefix))
        {
            return FLevelValidationResult(
                ELevelValidationError::MismatchedTagCategory,
                FString::Printf(TEXT("레벨 태그가 카테고리와 일치하지 않습니다. 예상: %s.*, 실제: %s (인덱스: %d)"),
                    *ExpectedPrefix, *TagString, LevelIndex),
                LevelIndex
            );
        }

        UsedLevelTags.Add(LevelData.LevelTag);
    }

    return FLevelValidationResult();
}

FLevelValidationResult ULevelDataInfos::ValidateLevelNames() const
{
    TMap<FString, int32> UsedLevelNames;

    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@비어있는 이름 검사
        if (!IsValidLevelName(LevelData.LevelName))
        {
            return FLevelValidationResult(
                ELevelValidationError::EmptyLevelName,
                FString::Printf(TEXT("비어있거나 유효하지 않은 레벨 이름 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }

        //@중복 이름 검사
        FString LevelNameString = LevelData.LevelName.ToString();
        if (int32* ExistingIndex = UsedLevelNames.Find(LevelNameString))
        {
            return FLevelValidationResult(
                ELevelValidationError::DuplicateLevelName,
                FString::Printf(TEXT("중복된 레벨 이름: '%s' (인덱스 %d와 %d)"),
                    *LevelNameString, *ExistingIndex, LevelIndex),
                LevelIndex
            );
        }

        UsedLevelNames.Add(LevelNameString, LevelIndex);
    }

    return FLevelValidationResult();
}

FLevelValidationResult ULevelDataInfos::ValidateLevelTypes() const
{
    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@유효하지 않은 레벨 타입 검사
        if (LevelData.LevelCategory == ELevelType::MAX)
        {
            return FLevelValidationResult(
                ELevelValidationError::InvalidLevelType,
                FString::Printf(TEXT("유효하지 않은 레벨 타입 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }
    }

    //@필수 레벨 타입 존재 여부 확인
    bool bHasTitleLevel = false;
    for (const FLevelData& LevelData : LevelDataList)
    {
        if (LevelData.LevelCategory == ELevelType::Title && LevelData.bIsEnabled)
        {
            bHasTitleLevel = true;
            break;
        }
    }

    if (!bHasTitleLevel)
    {
        UE_LOGFMT(LogLevelDataInfos, Warning,
            "활성화된 타이틀 레벨이 없습니다. 게임 시작에 문제가 발생할 수 있습니다.");
    }

    return FLevelValidationResult();
}

FLevelValidationResult ULevelDataInfos::ValidateLevelAssets() const
{
    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@Null 에셋 검사
        if (LevelData.LevelAsset.IsNull())
        {
            return FLevelValidationResult(
                ELevelValidationError::NullLevelAsset,
                FString::Printf(TEXT("레벨 에셋이 설정되지 않았습니다 (인덱스: %d, 이름: %s)"),
                    LevelIndex, *LevelData.LevelName.ToString()),
                LevelIndex
            );
        }

        //@에셋 유효성 검사
        if (!IsLevelAssetValid(LevelData.LevelAsset))
        {
            return FLevelValidationResult(
                ELevelValidationError::InvalidLevelAsset,
                FString::Printf(TEXT("유효하지 않은 레벨 에셋 (인덱스: %d, 이름: %s, 에셋: %s)"),
                    LevelIndex, *LevelData.LevelName.ToString(), *LevelData.LevelAsset.ToString()),
                LevelIndex
            );
        }

        //@에셋 존재 여부 확인 (로드하지 않고 체크)
        if (!LevelData.LevelAsset.ToSoftObjectPath().IsValid())
        {
            return FLevelValidationResult(
                ELevelValidationError::MissingLevelAsset,
                FString::Printf(TEXT("존재하지 않는 레벨 에셋 (인덱스: %d, 이름: %s)"),
                    LevelIndex, *LevelData.LevelName.ToString()),
                LevelIndex
            );
        }
    }

    return FLevelValidationResult();
}

FLevelValidationResult ULevelDataInfos::ValidateLogicalConsistency() const
{
    int32 DefaultLevelCount = 0;

    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@기본 레벨 개수 카운트
        if (LevelData.bIsDefaultLevel && LevelData.bIsEnabled)
        {
            DefaultLevelCount++;
        }

        //@비활성화된 필수 레벨 검사
        if (!LevelData.bIsEnabled && LevelData.LevelCategory == ELevelType::Title)
        {
            return FLevelValidationResult(
                ELevelValidationError::DisabledRequiredLevel,
                FString::Printf(TEXT("타이틀 레벨이 비활성화되어 있습니다 (인덱스: %d, 이름: %s)"),
                    LevelIndex, *LevelData.LevelName.ToString()),
                LevelIndex
            );
        }

        //@일관성 없는 상태 검사 (활성화된 레벨이 유효하지 않은 에셋을 가지는 경우)
        if (LevelData.bIsEnabled && LevelData.LevelAsset.IsNull())
        {
            return FLevelValidationResult(
                ELevelValidationError::InconsistentLevelState,
                FString::Printf(TEXT("활성화된 레벨에 에셋이 설정되지 않았습니다 (인덱스: %d, 이름: %s)"),
                    LevelIndex, *LevelData.LevelName.ToString()),
                LevelIndex
            );
        }
    }

    //@기본 레벨이 복수로 지정되었는지 확인
    if (DefaultLevelCount > 1)
    {
        return FLevelValidationResult(
            ELevelValidationError::MultipleDefaultLevels,
            FString::Printf(TEXT("기본 레벨이 %d개 설정되어 있습니다. 하나만 설정해야 합니다."), DefaultLevelCount)
        );
    }

    //@타입별 레벨 분포 검사
    TMap<ELevelType, int32> TypeCounts;
    for (const FLevelData& LevelData : LevelDataList)
    {
        if (LevelData.bIsEnabled)
        {
            TypeCounts.FindOrAdd(LevelData.LevelCategory, 0)++;
        }
    }

    //@보스 아레나가 너무 많은지 확인
    if (TypeCounts.FindRef(ELevelType::BossArena) > 10)
    {
        UE_LOGFMT(LogLevelDataInfos, Warning,
            "보스 아레나 레벨이 많습니다 ({0}개). 메모리 사용량을 확인하세요.",
            TypeCounts.FindRef(ELevelType::BossArena));
    }

    return FLevelValidationResult();
}

FLevelValidationResult ULevelDataInfos::ValidatePlayerStartTags() const
{
    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@활성화된 레벨은 기본 PlayerStart 태그가 필요
        if (LevelData.bIsEnabled && !LevelData.DefaultPlayerStartTag.IsValid())
        {
            return FLevelValidationResult(
                ELevelValidationError::InvalidPlayerStartTag,
                FString::Printf(TEXT("활성화된 레벨에 기본 PlayerStart 태그가 설정되지 않음 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }

        //@기본 태그가 사용 가능한 태그 목록에 포함되어야 함
        if (LevelData.DefaultPlayerStartTag.IsValid() &&
            !LevelData.AvailablePlayerStartTags.Contains(LevelData.DefaultPlayerStartTag))
        {
            return FLevelValidationResult(
                ELevelValidationError::MissingPlayerStartTags,
                FString::Printf(TEXT("기본 PlayerStart 태그가 사용 가능한 태그 목록에 없음 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }
    }

    return FLevelValidationResult();
}

void ULevelDataInfos::ShowValidationError(const FLevelValidationResult& ValidationResult) const
{
    //@로그 시스템에 상세 오류 정보 기록
    UE_LOGFMT(LogLevelDataInfos, Error,
        "레벨 데이터 검증 실패 | 오류 타입: {0} | 메시지: {1} | 레벨 인덱스: {2}",
        *UEnum::GetValueAsString(ValidationResult.ErrorType),
        *ValidationResult.ErrorMessage,
        ValidationResult.ProblemLevelIndex);

    //@개발자에게 즉각적인 피드백 제공
    ensureMsgf(false, TEXT("레벨 데이터 검증 실패: %s"), *ValidationResult.ErrorMessage);
}

bool ULevelDataInfos::TryAutoFixValidationError(const FLevelValidationResult& ValidationResult)
{
    switch (ValidationResult.ErrorType)
    {
    case ELevelValidationError::InvalidLevelTag:
    {
        //@유효하지 않은 태그를 카테고리에 맞는 기본 태그로 교체
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);

            // 카테고리에 따른 기본 태그 생성
            FString NewTagString;
            switch (LevelData.LevelCategory)
            {
            case ELevelType::World:
                NewTagString = FString::Printf(TEXT("Level.World.DefaultLevel_%d"), ValidationResult.ProblemLevelIndex);
                break;
            case ELevelType::BossArena:
                NewTagString = FString::Printf(TEXT("Level.BossArena.DefaultArena_%d"), ValidationResult.ProblemLevelIndex);
                break;
            case ELevelType::Title:
                NewTagString = FString::Printf(TEXT("Level.Title.DefaultTitle_%d"), ValidationResult.ProblemLevelIndex);
                break;
            default:
                NewTagString = FString::Printf(TEXT("Level.World.DefaultLevel_%d"), ValidationResult.ProblemLevelIndex);
                break;
            }

            LevelData.LevelTag = FGameplayTag::RequestGameplayTag(FName(*NewTagString));

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 유효하지 않은 레벨 태그를 새로운 태그로 교체했습니다 (인덱스: {0}, 새 태그: {1})",
                ValidationResult.ProblemLevelIndex, *NewTagString);
            return true;
        }
        break;
    }

    case ELevelValidationError::DuplicateLevelTag:
    {
        //@중복된 태그를 고유한 태그로 교체
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);

            // 기존 태그를 기반으로 고유한 변형 생성
            FString OriginalTagString = LevelData.LevelTag.ToString();
            FString NewTagString;

            // 이미 숫자 접미사가 있는지 확인
            int32 UnderscoreIndex = OriginalTagString.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            if (UnderscoreIndex != INDEX_NONE)
            {
                // 기존 접미사를 제거하고 새로운 번호 추가
                FString BaseTag = OriginalTagString.Left(UnderscoreIndex);
                NewTagString = FString::Printf(TEXT("%s_%d"), *BaseTag, ValidationResult.ProblemLevelIndex);
            }
            else
            {
                // 접미사가 없다면 추가
                NewTagString = FString::Printf(TEXT("%s_%d"), *OriginalTagString, ValidationResult.ProblemLevelIndex);
            }

            LevelData.LevelTag = FGameplayTag::RequestGameplayTag(FName(*NewTagString));

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 중복된 레벨 태그를 고유한 태그로 교체했습니다 (인덱스: {0}, 새 태그: {1})",
                ValidationResult.ProblemLevelIndex, *NewTagString);
            return true;
        }
        break;
    }

    case ELevelValidationError::MismatchedTagCategory:
    {
        //@태그와 카테고리가 일치하지 않는 경우 태그를 카테고리에 맞게 수정
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);

            FString OldTagString = LevelData.LevelTag.ToString();
            FString NewTagString;

            // 기존 태그에서 Level. 이후 부분을 추출
            FString TagSuffix;
            int32 DotIndex = OldTagString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart);
            if (DotIndex != INDEX_NONE)
            {
                int32 SecondDotIndex = OldTagString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart, DotIndex + 1);
                if (SecondDotIndex != INDEX_NONE)
                {
                    // Level.Category. 이후 부분을 보존
                    TagSuffix = OldTagString.Mid(SecondDotIndex + 1);
                }
            }

            // 태그 접미사가 없으면 기본값 사용
            if (TagSuffix.IsEmpty())
            {
                TagSuffix = FString::Printf(TEXT("CorrectedLevel_%d"), ValidationResult.ProblemLevelIndex);
            }

            // 카테고리에 맞는 올바른 접두사로 교체
            switch (LevelData.LevelCategory)
            {
            case ELevelType::World:
                NewTagString = FString::Printf(TEXT("Level.World.%s"), *TagSuffix);
                break;
            case ELevelType::BossArena:
                NewTagString = FString::Printf(TEXT("Level.BossArena.%s"), *TagSuffix);
                break;
            case ELevelType::Title:
                NewTagString = FString::Printf(TEXT("Level.Title.%s"), *TagSuffix);
                break;
            default:
                NewTagString = FString::Printf(TEXT("Level.World.%s"), *TagSuffix);
                break;
            }

            LevelData.LevelTag = FGameplayTag::RequestGameplayTag(FName(*NewTagString));

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 태그를 카테고리에 맞게 수정했습니다 (인덱스: {0}, 이전: {1}, 새 태그: {2})",
                ValidationResult.ProblemLevelIndex, *OldTagString, *NewTagString);
            return true;
        }
        break;
    }

    case ELevelValidationError::MultipleDefaultLevels:
    {
        //@복수의 기본 레벨이 설정된 경우, 첫 번째를 제외하고 모두 해제
        int32 DefaultLevelFoundCount = 0;

        for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[LevelIndex]);

            if (LevelData.bIsDefaultLevel && LevelData.bIsEnabled)
            {
                DefaultLevelFoundCount++;

                if (DefaultLevelFoundCount > 1)
                {
                    // 첫 번째 기본 레벨을 제외하고 모두 해제
                    LevelData.bIsDefaultLevel = false;

                    UE_LOGFMT(LogLevelDataInfos, Log,
                        "자동 수정: 중복된 기본 레벨 플래그를 해제했습니다 (인덱스: {0}, 이름: {1})",
                        LevelIndex, *LevelData.LevelName.ToString());
                }
            }
        }

        if (DefaultLevelFoundCount > 1)
        {
            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정 완료: {0}개의 중복 기본 레벨 플래그를 해제했습니다",
                DefaultLevelFoundCount - 1);
            return true;
        }
        break;
    }

    case ELevelValidationError::EmptyLevelName:
    {
        //@비어있는 이름을 태그 기반 기본 이름으로 교체
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);

            // 태그에서 의미 있는 이름 추출 시도
            FString NewLevelName;
            if (LevelData.LevelTag.IsValid())
            {
                FString TagString = LevelData.LevelTag.ToString();
                int32 LastDotIndex = TagString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
                if (LastDotIndex != INDEX_NONE)
                {
                    NewLevelName = TagString.Mid(LastDotIndex + 1);
                    // 언더스코어를 공백으로 변경하여 더 읽기 쉽게 만듦
                    NewLevelName = NewLevelName.Replace(TEXT("_"), TEXT(" "));
                }
                else
                {
                    NewLevelName = TagString;
                }
            }
            else
            {
                NewLevelName = FString::Printf(TEXT("Level %d"), ValidationResult.ProblemLevelIndex);
            }

            LevelData.LevelName = FText::FromString(NewLevelName);

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 비어있는 레벨 이름을 태그 기반 이름으로 설정했습니다 (인덱스: {0}, 새 이름: {1})",
                ValidationResult.ProblemLevelIndex, *NewLevelName);
            return true;
        }
        break;
    }

    case ELevelValidationError::InvalidLevelType:
    {
        //@유효하지 않은 레벨 타입을 World로 설정하고 태그도 함께 수정
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);

            ELevelType OldCategory = LevelData.LevelCategory;
            LevelData.LevelCategory = ELevelType::World;

            // 태그도 새로운 카테고리에 맞게 수정
            FString NewTagString = FString::Printf(TEXT("Level.World.CorrectedLevel_%d"), ValidationResult.ProblemLevelIndex);
            LevelData.LevelTag = FGameplayTag::RequestGameplayTag(FName(*NewTagString));

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 유효하지 않은 레벨 타입을 World로 설정하고 태그를 수정했습니다 (인덱스: {0}, 새 태그: {1})",
                ValidationResult.ProblemLevelIndex, *NewTagString);
            return true;
        }
        break;
    }

    case ELevelValidationError::InconsistentLevelState:
    {
        //@활성화된 레벨이 유효하지 않은 에셋을 가지는 경우 비활성화
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);
            LevelData.bIsEnabled = false;

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 일관성 없는 레벨을 비활성화했습니다 (인덱스: {0})",
                ValidationResult.ProblemLevelIndex);
            return true;
        }
        break;
    }

    default:
    {
        UE_LOGFMT(LogLevelDataInfos, Log,
            "자동 수정 불가: {0} 오류는 수동 수정이 필요합니다. 레벨 {1}을 검토하세요.",
            *UEnum::GetValueAsString(ValidationResult.ErrorType),
            ValidationResult.ProblemLevelIndex);
        return false;
    }
    }

    return false;
}

bool ULevelDataInfos::IsValidLevelName(const FText& LevelName) const
{
    if (LevelName.IsEmpty())
    {
        return false;
    }

    FString LevelNameString = LevelName.ToString().TrimStartAndEnd();
    if (LevelNameString.IsEmpty() || LevelNameString == TEXT("None"))
    {
        return false;
    }

    return true;
}

bool ULevelDataInfos::IsLevelAssetValid(const TSoftObjectPtr<UWorld>& LevelAsset) const
{
    if (LevelAsset.IsNull())
    {
        return false;
    }

    //@에셋 경로가 유효한지 확인
    FSoftObjectPath AssetPath = LevelAsset.ToSoftObjectPath();
    if (!AssetPath.IsValid())
    {
        return false;
    }

    //@월드 에셋 확장자 확인
    FString AssetPathString = AssetPath.ToString();
    if (!AssetPathString.EndsWith(TEXT(".umap")))
    {
        return false;
    }

    return true;
}

void ULevelDataInfos::ValidateLevelDataManually()
{
    FLevelValidationResult ValidationResult = ValidateLevelData();

    if (ValidationResult.bIsValid)
    {
        UE_LOGFMT(LogLevelDataInfos, Log, "✅ 레벨 데이터 무결성 검사 성공: 모든 설정이 유효합니다");

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                TEXT("✅ 레벨 데이터 검증 성공! 모든 레벨 설정이 올바릅니다."));
        }
    }
    else
    {
        ShowValidationError(ValidationResult);
    }
}

void ULevelDataInfos::ValidateLevelAssetsOnly()
{
    FLevelValidationResult Result = ValidateLevelAssets();

    if (Result.bIsValid)
    {
        UE_LOGFMT(LogLevelDataInfos, Log, "✅ 레벨 에셋 검증 완료: 모든 에셋이 유효합니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ 레벨 에셋 검증 성공!"));
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}

void ULevelDataInfos::ValidateLevelNamesOnly()
{
    FLevelValidationResult Result = ValidateLevelNames();

    if (Result.bIsValid)
    {
        UE_LOGFMT(LogLevelDataInfos, Log, "✅ 레벨 이름 검증 완료: 모든 이름이 유효합니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ 레벨 이름 검증 성공!"));
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}

void ULevelDataInfos::SortLevelsByType()
{
    LevelDataList.Sort([](const FLevelData& A, const FLevelData& B)
        {
            //@타입으로 우선 정렬, 같은 타입이면 이름으로 정렬
            if (A.LevelCategory != B.LevelCategory)
            {
                return A.LevelCategory < B.LevelCategory;
            }
            return A.LevelName.ToString() < B.LevelName.ToString();
        });

    UE_LOGFMT(LogLevelDataInfos, Log, "레벨을 타입과 이름 순으로 정렬했습니다");
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            TEXT("레벨 정렬 완료!"));
    }
}
#endif

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc

const FLevelData* ULevelDataInfos::FindLevelByTag(const FGameplayTag& LevelTag) const
{
    return LevelDataList.FindByPredicate([LevelTag](const FLevelData& LevelData)
        {
            return LevelData.LevelTag.MatchesTagExact(LevelTag);
        });
}

const FLevelData* ULevelDataInfos::FindLevelByName(const FText& LevelName) const
{
    FString SearchName = LevelName.ToString();
    return LevelDataList.FindByPredicate([SearchName](const FLevelData& LevelData)
        {
            return LevelData.LevelName.ToString() == SearchName;
        });
}

bool ULevelDataInfos::GetLevelByTag(const FGameplayTag& LevelTag, FLevelData& OutLevelData) const
{
    const FLevelData* FoundLevel = FindLevelByTag(LevelTag);
    if (FoundLevel)
    {
        OutLevelData = *FoundLevel;
        return true;
    }
    return false;
}

bool ULevelDataInfos::GetLevelByName(const FText& LevelName, FLevelData& OutLevelData) const
{
    const FLevelData* FoundLevel = FindLevelByName(LevelName);
    if (FoundLevel)
    {
        OutLevelData = *FoundLevel;
        return true;
    }
    return false;
}

TArray<FLevelData> ULevelDataInfos::GetLevelsByType(ELevelType LevelType) const
{
    TArray<FLevelData> FilteredLevels;

    for (const FLevelData& LevelData : LevelDataList)
    {
        if (LevelData.LevelCategory == LevelType)
        {
            FilteredLevels.Add(LevelData);
        }
    }

    return FilteredLevels;
}

TArray<FLevelData> ULevelDataInfos::GetEnabledLevels() const
{
    TArray<FLevelData> EnabledLevels;

    for (const FLevelData& LevelData : LevelDataList)
    {
        if (LevelData.bIsEnabled)
        {
            EnabledLevels.Add(LevelData);
        }
    }

    return EnabledLevels;
}

bool ULevelDataInfos::IsLevelEnabled(const FGameplayTag& LevelTag) const
{
    // 먼저 태그 자체의 유효성을 검사
    if (!LevelTag.IsValid())
    {
        UE_LOGFMT(LogLevelDataInfos, Warning,
            "레벨 활성화 상태 확인 실패: 유효하지 않은 레벨 태그입니다");
        return false;
    }

    // 태그로 레벨 데이터 검색
    const FLevelData* FoundLevel = FindLevelByTag(LevelTag);

    if (!FoundLevel)
    {
        UE_LOGFMT(LogLevelDataInfos, Warning,
            "레벨 활성화 상태 확인 실패: 태그 '{0}'에 해당하는 레벨을 찾을 수 없습니다",
            *LevelTag.ToString());
        return false;
    }

    // 디버깅을 위한 상세 로깅 (VeryVerbose 레벨로 성능에 영향 없음)
    UE_LOGFMT(LogLevelDataInfos, VeryVerbose,
        "레벨 활성화 상태 확인: 태그 '{0}', 이름 '{1}', 활성화 상태: {2}",
        *LevelTag.ToString(),
        *FoundLevel->LevelName.ToString(),
        FoundLevel->bIsEnabled ? TEXT("활성화") : TEXT("비활성화"));

    return FoundLevel->bIsEnabled;
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
const FLevelData* ULevelDataInfos::GetDefaultLevel() const
{
    return LevelDataList.FindByPredicate([](const FLevelData& LevelData)
        {
            return LevelData.bIsDefaultLevel && LevelData.bIsEnabled;
        });
}

int32 ULevelDataInfos::GetLevelCountByType(ELevelType LevelType) const
{
    int32 Count = 0;
    for (const FLevelData& LevelData : LevelDataList)
    {
        if (LevelData.LevelCategory == LevelType)
        {
            Count++;
        }
    }
    return Count;
}

const FLevelData* ULevelDataInfos::GetFirstLevelOfType(ELevelType LevelType) const
{
    for (const FLevelData& LevelData : LevelDataList)
    {
        if (LevelData.LevelCategory == LevelType && LevelData.bIsEnabled)
        {
            return &LevelData;
        }
    }
    return nullptr;
}

TArray<FLevelData> ULevelDataInfos::GetActiveLevels() const
{
    return GetEnabledLevels(); //@같은 기능이므로 호출
}

const FLevelData* ULevelDataInfos::GetLevelAtIndex(int32 Index) const
{
    return LevelDataList.IsValidIndex(Index) ? &LevelDataList[Index] : nullptr;
}

bool ULevelDataInfos::GetFirstLevelOfTypeBlueprint(ELevelType LevelType, FLevelData& OutLevelData) const
{
    const FLevelData* FoundLevel = GetFirstLevelOfType(LevelType);
    if (FoundLevel)
    {
        OutLevelData = *FoundLevel;
        return true;
    }
    return false;
}

bool ULevelDataInfos::GetLevelAtIndexBlueprint(int32 Index, FLevelData& OutLevelData) const
{
    const FLevelData* FoundLevel = GetLevelAtIndex(Index);
    if (FoundLevel)
    {
        OutLevelData = *FoundLevel;
        return true;
    }
    return false;
}
#pragma endregion