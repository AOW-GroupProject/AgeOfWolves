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
{
}

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
    //@1단계: 기본 설정 검증
    FLevelValidationResult Result = ValidateBasicSettings();
    if (!Result.bIsValid) return Result;

    //@2단계: 레벨 ID 검증
    Result = ValidateLevelIDs();
    if (!Result.bIsValid) return Result;

    //@3단계: 레벨 이름 검증
    Result = ValidateLevelNames();
    if (!Result.bIsValid) return Result;

    //@4단계: 레벨 타입 검증
    Result = ValidateLevelTypes();
    if (!Result.bIsValid) return Result;

    //@5단계: 레벨 에셋 검증
    Result = ValidateLevelAssets();
    if (!Result.bIsValid) return Result;

    //@6단계: 논리적 일관성 검증
    Result = ValidateLogicalConsistency();
    if (!Result.bIsValid) return Result;

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

FLevelValidationResult ULevelDataInfos::ValidateLevelIDs() const
{
    TSet<FGuid> UsedLevelIDs;

    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

        //@유효하지 않은 GUID 검사
        if (!LevelData.LevelID.IsValid())
        {
            return FLevelValidationResult(
                ELevelValidationError::InvalidLevelID,
                FString::Printf(TEXT("유효하지 않은 레벨 ID입니다 (인덱스: %d)"), LevelIndex),
                LevelIndex
            );
        }

        //@중복 GUID 검사
        if (UsedLevelIDs.Contains(LevelData.LevelID))
        {
            return FLevelValidationResult(
                ELevelValidationError::DuplicateLevelID,
                FString::Printf(TEXT("중복된 레벨 ID 발견: %s (인덱스: %d)"),
                    *LevelData.LevelID.ToString(), LevelIndex),
                LevelIndex
            );
        }

        UsedLevelIDs.Add(LevelData.LevelID);
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
    for (int32 LevelIndex = 0; LevelIndex < LevelDataList.Num(); ++LevelIndex)
    {
        const FLevelData& LevelData = LevelDataList[LevelIndex];

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
    case ELevelValidationError::InvalidLevelID:
    {
        //@유효하지 않은 GUID를 새로운 GUID로 교체
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);
            FGuid OldID = LevelData.LevelID;
            LevelData.LevelID = FGuid::NewGuid();

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 유효하지 않은 레벨 ID를 새로운 ID로 교체했습니다 (인덱스: {0})",
                ValidationResult.ProblemLevelIndex);
            return true;
        }
        break;
    }

    case ELevelValidationError::DuplicateLevelID:
    {
        //@중복된 GUID를 새로운 GUID로 교체
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);
            LevelData.LevelID = FGuid::NewGuid();

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 중복된 레벨 ID를 새로운 ID로 교체했습니다 (인덱스: {0})",
                ValidationResult.ProblemLevelIndex);
            return true;
        }
        break;
    }

    case ELevelValidationError::EmptyLevelName:
    {
        //@비어있는 이름을 기본 이름으로 교체
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);
            LevelData.LevelName = FText::FromString(FString::Printf(TEXT("Level_%d"), ValidationResult.ProblemLevelIndex));

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 비어있는 레벨 이름을 기본 이름으로 설정했습니다 (인덱스: {0})",
                ValidationResult.ProblemLevelIndex);
            return true;
        }
        break;
    }

    case ELevelValidationError::InvalidLevelType:
    {
        //@유효하지 않은 레벨 타입을 World로 설정
        if (ValidationResult.ProblemLevelIndex >= 0 && ValidationResult.ProblemLevelIndex < LevelDataList.Num())
        {
            FLevelData& LevelData = const_cast<FLevelData&>(LevelDataList[ValidationResult.ProblemLevelIndex]);
            LevelData.LevelCategory = ELevelType::World;

            UE_LOGFMT(LogLevelDataInfos, Log,
                "자동 수정: 유효하지 않은 레벨 타입을 World로 설정했습니다 (인덱스: {0})",
                ValidationResult.ProblemLevelIndex);
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

void ULevelDataInfos::GenerateNewLevelIDs()
{
    int32 RegeneratedCount = 0;

    for (FLevelData& LevelData : LevelDataList)
    {
        if (!LevelData.LevelID.IsValid())
        {
            LevelData.LevelID = FGuid::NewGuid();
            RegeneratedCount++;
        }
    }

    UE_LOGFMT(LogLevelDataInfos, Log, "새로운 레벨 ID {0}개를 생성했습니다", RegeneratedCount);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            FString::Printf(TEXT("새로운 레벨 ID %d개를 생성했습니다"), RegeneratedCount));
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

const FLevelData* ULevelDataInfos::FindLevelByID(const FGuid& LevelID) const
{
    return LevelDataList.FindByPredicate([LevelID](const FLevelData& LevelData)
        {
            return LevelData.LevelID == LevelID;
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

bool ULevelDataInfos::GetLevelByID(const FGuid& LevelID, FLevelData& OutLevelData) const
{
    const FLevelData* FoundLevel = FindLevelByID(LevelID);
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

bool ULevelDataInfos::IsLevelEnabled(const FGuid& LevelID) const
{
    const FLevelData* FoundLevel = FindLevelByID(LevelID);
    return FoundLevel ? FoundLevel->bIsEnabled : false;
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