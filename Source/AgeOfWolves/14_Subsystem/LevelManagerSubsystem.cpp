#include "LevelManagerSubsystem.h"
#include "Logging/StructuredLog.h"


DEFINE_LOG_CATEGORY(LogLevelManager)

//@Default Setting
#pragma region Default Setting
ULevelManagerSubsystem::ULevelManagerSubsystem()
{}

void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@레벨 데이터 에셋 로드
    LevelDataInfos = LoadObject<ULevelDataInfos>(nullptr, TEXT("/Game/Blueprints/10_Level/DA_LevelDataInfos"));

    if (LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Log, "레벨 데이터 정보 로드 성공 - 총 {0}개의 레벨 데이터 확인됨",
            LevelDataInfos->GetLevelCount());

        //@데이터 무결성 검증
        if (IsLevelDataValid())
        {
            UE_LOGFMT(LogLevelManager, Log, "레벨 데이터 무결성 검증 완료");
        }
        else
        {
            UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터에 무결성 문제 발견 - 런타임 오류 가능성 있음");
        }
    }
    else
    {
        UE_LOGFMT(LogLevelManager, Error, "레벨 데이터 정보 로드 실패 - 경로 확인 필요: /Game/Blueprints/10_Level/DA_LevelDataInfos");
    }
}
#pragma endregion

//@Utility Functions
#pragma region Utility
TArray<FLevelData> ULevelManagerSubsystem::GetLevelsByType(const ELevelType& LevelType) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return TArray<FLevelData>();
    }

    TArray<FLevelData> Result = LevelDataInfos->GetLevelsByType(LevelType);
    UE_LOGFMT(LogLevelManager, Log, "{0} 타입의 레벨 {1}개를 반환합니다.",
        *UEnum::GetValueAsString(LevelType), Result.Num());

    return Result;
}

bool ULevelManagerSubsystem::GetLevelByID(const FGuid& LevelID, FLevelData& OutLevelData) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    bool bFound = LevelDataInfos->GetLevelByID(LevelID, OutLevelData);

    if (!bFound)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 ID {0}를 찾지 못했습니다.", *LevelID.ToString());
    }

    return bFound;
}

bool ULevelManagerSubsystem::GetLevelByName(const FText& LevelName, FLevelData& OutLevelData) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    bool bFound = LevelDataInfos->GetLevelByName(LevelName, OutLevelData);

    if (!bFound)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 이름 '{0}'을 찾지 못했습니다.", *LevelName.ToString());
    }

    return bFound;
}

TArray<FLevelData> ULevelManagerSubsystem::GetEnabledLevels() const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return TArray<FLevelData>();
    }

    TArray<FLevelData> EnabledLevels = LevelDataInfos->GetEnabledLevels();
    UE_LOGFMT(LogLevelManager, Log, "활성화된 레벨 {0}개를 반환합니다.", EnabledLevels.Num());

    return EnabledLevels;
}

bool ULevelManagerSubsystem::IsLevelEnabled(const FGuid& LevelID) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    return LevelDataInfos->IsLevelEnabled(LevelID);
}

bool ULevelManagerSubsystem::GetFirstLevelOfType(const ELevelType& LevelType, FLevelData& OutLevelData) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    bool bFound = LevelDataInfos->GetFirstLevelOfTypeBlueprint(LevelType, OutLevelData);

    if (bFound)
    {
        UE_LOGFMT(LogLevelManager, Log, "{0} 타입의 첫 번째 레벨 '{1}'을 반환합니다.",
            *UEnum::GetValueAsString(LevelType), *OutLevelData.LevelName.ToString());
    }
    else
    {
        UE_LOGFMT(LogLevelManager, Warning, "{0} 타입의 활성화된 레벨을 찾지 못했습니다.",
            *UEnum::GetValueAsString(LevelType));
    }

    return bFound;
}

int32 ULevelManagerSubsystem::GetTotalLevelCount() const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return 0;
    }

    return LevelDataInfos->GetLevelCount();
}

int32 ULevelManagerSubsystem::GetLevelCountByType(const ELevelType& LevelType) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return 0;
    }

    return LevelDataInfos->GetLevelCountByType(LevelType);
}

bool ULevelManagerSubsystem::IsLevelDataValid() const
{
    //@데이터 에셋 로드 상태 및 유효성 확인
    if (!LevelDataInfos)
    {
        return false;
    }

    return LevelDataInfos->GetLevelCount() > 0;
}
#pragma endregion